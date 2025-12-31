#include "Items/ItemBase.h"

UItemBase::UItemBase()
{
	ItemID = "";
	bIsStackable = false;
	MaxStackSize = 1;
	CurrentStackSize = 1;
	bIsEquippable = false;
	bIsInInventory = false;
	bIsEquipped = false;
}

void UItemBase::InitializeItem_Implementation()
{
	// Sanitize Stacking Data
	if (bIsStackable)
	{
		MaxStackSize = FMath::Max(1, MaxStackSize);
		CurrentStackSize = FMath::Clamp(CurrentStackSize, 1, MaxStackSize);
	}
	else
	{
		MaxStackSize = 1;
		CurrentStackSize = 1;
	}

	if (ItemID.IsEmpty()) { ItemID = GenerateUniqueItemID(); }
	if (ItemName.IsEmpty()) { ItemName = FText::FromString(GetClass()->GetName()); }

	// Initialize all attached modules
	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module) { Module->Initialize(this); }
	}
}

// --- Stacking Logic ---

void UItemBase::SetCurrentStackSize(int32 NewSize)
{
	if (bIsStackable)
	{
		CurrentStackSize = FMath::Clamp(NewSize, 0, MaxStackSize);
	}
}

bool UItemBase::CanMergeWith(const UItemBase* OtherItem) const
{
	if (!OtherItem || !bIsStackable) return false;

	// Items are the same type and there is room in the stack
	return ItemID == OtherItem->GetItemID() && CurrentStackSize < MaxStackSize;
}

bool UItemBase::MergeWith(UItemBase* OtherItem)
{
	if (!CanMergeWith(OtherItem)) return false;

	int32 SpaceLeft = MaxStackSize - CurrentStackSize;
	int32 TransferAmount = FMath::Min(SpaceLeft, OtherItem->CurrentStackSize);

	SetCurrentStackSize(CurrentStackSize + TransferAmount);
	OtherItem->SetCurrentStackSize(OtherItem->CurrentStackSize - TransferAmount);

	// Notify modules of the merge
	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module && Module->IsModuleActive()) { Module->OnItemMerged(OtherItem, false); }
	}

	return true;
}

UItemBase* UItemBase::SplitStack(int32 Amount)
{
	if (Amount <= 0 || Amount >= CurrentStackSize) return nullptr;

	CurrentStackSize -= Amount;

	// Create a new instance of the same class
	UItemBase* NewItem = NewObject<UItemBase>(GetOuter(), GetClass());

	// Copy base data
	NewItem->ItemID = ItemID;
	NewItem->ItemName = ItemName;
	NewItem->ItemDescription = ItemDescription;
	NewItem->ItemIcon = ItemIcon;
	NewItem->bIsStackable = bIsStackable;
	NewItem->MaxStackSize = MaxStackSize;
	NewItem->bIsEquippable = bIsEquippable;
	NewItem->SetCurrentStackSize(Amount);

	// Deep copy modules
	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module)
		{
			if (UItemModuleBase* NewModule = Module->DuplicateModule(NewItem))
			{
				NewItem->ItemModules.Add(NewModule);
				if (Module->IsModuleActive()) { Module->OnItemSplit(NewItem, Amount); }
			}
		}
	}

	return NewItem;
}

// --- Inventory & Equipment Management ---

void UItemBase::OnAddedToInventory(AActor* NewOwner)
{
	if (!NewOwner) return;

	OwnerActor = NewOwner;
	bIsInInventory = true;

	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module && Module->IsModuleActive()) { Module->OnItemAddedToInventory(NewOwner); }
	}
}

void UItemBase::OnRemovedFromInventory()
{
	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module && Module->IsModuleActive()) { Module->OnItemRemovedFromInventory(); }
	}

	bIsInInventory = false;
	bIsEquipped = false;
	OwnerActor = nullptr;
}

void UItemBase::OnEquip_Implementation(AActor* OwningActor)
{
	if (!OwningActor || !bIsEquippable) return;

	bIsEquipped = true;

	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module && Module->IsModuleActive()) { Module->OnItemEquipped(OwningActor); }
	}

	OnItemEquipped(OwningActor);
}

void UItemBase::OnUnequip_Implementation(AActor* OwningActor)
{
	bIsEquipped = false;

	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module && Module->IsModuleActive()) { Module->OnItemUnequipped(OwningActor); }
	}

	OnItemUnequipped(OwningActor);
}

// --- Module Support ---

void UItemBase::AddModule(UItemModuleBase* NewModule)
{
	if (!NewModule) return;

	// Ensure unique module classes
	if (GetModuleByClass(NewModule->GetClass())) return;

	ItemModules.Add(NewModule);
	NewModule->Initialize(this);

	// Sync module state with current item state
	if (bIsInInventory) NewModule->OnItemAddedToInventory(OwnerActor);
	if (bIsEquipped) NewModule->OnItemEquipped(OwnerActor);
}

void UItemBase::RemoveModule(UItemModuleBase* ModuleToRemove)
{
	if (!ModuleToRemove) return;

	if (bIsEquipped) ModuleToRemove->OnItemUnequipped(OwnerActor);
	if (bIsInInventory) ModuleToRemove->OnItemRemovedFromInventory();

	ItemModules.Remove(ModuleToRemove);
}

UItemModuleBase* UItemBase::GetModuleByClass(TSubclassOf<UItemModuleBase> ModuleClass) const
{
	if (!ModuleClass) return nullptr;

	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module && Module->IsA(ModuleClass)) return Module;
	}
	return nullptr;
}

void UItemBase::TickModules(float DeltaTime)
{
	for (UItemModuleBase* Module : ItemModules)
	{
		if (Module && Module->IsModuleActive()) { Module->TickModule(DeltaTime); }
	}
}

FString UItemBase::GenerateUniqueItemID()
{
	return FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
}

void UItemBase::OnItemEquipped_Implementation(AActor* Character)
{
}

void UItemBase::OnItemUnequipped_Implementation(AActor* Character)
{
}

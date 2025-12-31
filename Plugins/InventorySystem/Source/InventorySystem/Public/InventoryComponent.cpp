// InventoryComponent.cpp
#include "InventoryComponent.h"
#include "Engine/Engine.h"
#include "Items/WearableItems/WearableItem.h"

// Constructors
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxInventorySlots = 20;
	bAutoStackItems = true;
	bHasEquipmentSlots = true;

	// Varsayılan ekipman slotları
	AvailableEquipSlots = {
		EEquipSlot::EES_Head,
		EEquipSlot::EES_Chest,
		EEquipSlot::EES_Legs,
		EEquipSlot::EES_Feet,
		EEquipSlot::EES_MainHand,
		EEquipSlot::EES_OffHand
	};
}

// Component Lifecycle
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize inventory slots
	InventorySlots.Empty();
	for (int32 i = 0; i < MaxInventorySlots; i++) { InventorySlots.Add(FInventorySlot(nullptr, i)); }

	// Initialize equipment slots if enabled
	if (bHasEquipmentSlots)
	{
		EquipmentSlots.Empty();
		for (const EEquipSlot& Slot : AvailableEquipSlots) { EquipmentSlots.Add(Slot, nullptr); }
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Inventory Management Functions
bool UInventoryComponent::AddItem(UItemBase* Item)
{
	if (!Item)
		return false;

	// Try to stack first if auto stacking is enabled
	if (bAutoStackItems && Item->IsStackable()) { if (TryStackItem(Item)) { return true; } }

	// Find empty slot
	int32 EmptySlot = FindEmptySlot();
	if (EmptySlot == -1)
	{
		// Inventory full
		OnInventoryFull.Broadcast(Item, 1);
		return false;
	}

	return InternalAddItem(Item, EmptySlot);
}

bool UInventoryComponent::RemoveItem(UItemBase* Item)
{
	if (!Item)
		return false;

	int32 SlotIndex = FindItemSlot(Item);
	if (SlotIndex != -1) { return RemoveItemAt(SlotIndex); }

	return false;
}

bool UInventoryComponent::RemoveItemAt(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= InventorySlots.Num())
		return false;

	UItemBase* ItemToRemove = InventorySlots[SlotIndex].Item;
	if (!ItemToRemove)
		return false;

	// Envanter pasif efektlerini kaldır
	if (GetOwner()) { ItemToRemove->OnRemovedFromInventory(); }

	InventorySlots[SlotIndex] = FInventorySlot(nullptr, SlotIndex);
	OnItemRemoved.Broadcast(ItemToRemove, SlotIndex);
	BroadcastInventoryChange();

	return true;
}

UItemBase* UInventoryComponent::GetItemAtIndex(int32 Index) const
{
	if (Index < 0 || Index >= InventorySlots.Num())
		return nullptr;

	return InventorySlots[Index].Item;
}

int32 UInventoryComponent::FindItemSlot(UItemBase* Item) const
{
	if (!Item)
		return -1;

	for (int32 i = 0; i < InventorySlots.Num(); i++) { if (InventorySlots[i].Item == Item) { return i; } }

	return -1;
}

bool UInventoryComponent::HasItem(UItemBase* Item) const { return FindItemSlot(Item) != -1; }

int32 UInventoryComponent::GetItemCount(UItemBase* Item) const
{
	if (!Item)
		return 0;

	int32 Count = 0;
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Item && Slot.Item->GetItemID() == Item->GetItemID()) { Count += Slot.Item->GetCurrentStackSize(); }
	}

	return Count;
}

bool UInventoryComponent::IsInventoryFull() const { return FindEmptySlot() == -1; }

int32 UInventoryComponent::GetEmptySlotCount() const
{
	int32 Count = 0;
	for (const FInventorySlot& Slot : InventorySlots) { if (!Slot.Item) { Count++; } }
	return Count;
}

// Stacking Functions
bool UInventoryComponent::TryStackItem(UItemBase* Item)
{
	if (!Item || !Item->IsStackable())
		return false;

	// Look for existing stacks of this item
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		UItemBase* ExistingItem = InventorySlots[i].Item;
		if (ExistingItem && ExistingItem->CanMergeWith(Item))
		{
			int32 OldStackSize = ExistingItem->GetCurrentStackSize();
			bool bFullyMerged = ExistingItem->MergeWith(Item);

			OnItemStackChanged.Broadcast(ExistingItem, i, OldStackSize, ExistingItem->GetCurrentStackSize());
			BroadcastInventoryChange();

			// If item was fully merged, we can destroy it or return it as consumed
			if (bFullyMerged && Item->GetCurrentStackSize() <= 0) { Item->ConditionalBeginDestroy(); }

			return true;
		}
	}

	return false;
}

bool UInventoryComponent::SplitItemStack(int32 FromSlot, int32 ToSlot, int32 Amount)
{
	if (FromSlot < 0 || FromSlot >= InventorySlots.Num() ||
		ToSlot < 0 || ToSlot >= InventorySlots.Num() ||
		FromSlot == ToSlot || Amount <= 0) { return false; }

	UItemBase* SourceItem = InventorySlots[FromSlot].Item;
	if (!SourceItem || !SourceItem->IsStackable() || SourceItem->GetCurrentStackSize() <= Amount) { return false; }

	// Split the item
	UItemBase* NewItem = SourceItem->SplitStack(Amount);
	if (!NewItem) { return false; }

	// Update source item stack change
	OnItemStackChanged.Broadcast(SourceItem, FromSlot, SourceItem->GetCurrentStackSize() + Amount,
	                             SourceItem->GetCurrentStackSize());

	// Add new item to target slot
	if (InternalAddItem(NewItem, ToSlot))
	{
		BroadcastInventoryChange();
		return true;
	}
	else
	{
		// Failed to add, merge back
		SourceItem->SetCurrentStackSize(SourceItem->GetCurrentStackSize() + Amount);
		NewItem->ConditionalBeginDestroy();
		return false;
	}
}

// Equipment Functions - UPDATED
bool UInventoryComponent::EquipItem(UItemBase* Item)
{
	if (!Item || !Item->IsEquippable() || !bHasEquipmentSlots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Invalid item or equipment disabled"));
		return false;
	}

	// WearableItem olarak cast et
	UWearableItem* WearableItem = Cast<UWearableItem>(Item);
	if (!WearableItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Item is not a WearableItem"));
		return false;
	}

	EEquipSlot TargetSlot = WearableItem->EquipSlot;

	if (!IsEquipSlotAvailable(TargetSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Slot %d is not available"), (int32)TargetSlot);
		return false;
	}

	int32 InventorySlot = FindItemSlot(Item);
	if (InventorySlot == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: Item not found in inventory"));
		return false;
	}

	if (!GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot equip item: No owning Owner"));
		return false;
	}

	// Önce inventory passive'lerini kaldır
	Item->OnRemovedFromInventory();

	// Remove from inventory
	InventorySlots[InventorySlot] = FInventorySlot(nullptr, InventorySlot);

	// Unequip any existing item in that slot
	if (UItemBase* ExistingItem = EquipmentSlots.FindRef(TargetSlot)) { UnequipItem(TargetSlot); }

	// Add to equipment slot
	EquipmentSlots.Add(TargetSlot, Item);

	// Equipped passive efektlerini uygula
	IEquippable::Execute_OnEquip(Item, GetOwner());

	// Delegate'leri broadcast et
	OnItemEquipped.Broadcast(Item, TargetSlot);
	OnItemRemoved.Broadcast(Item, InventorySlot);
	BroadcastInventoryChange();
	return true;
}

bool UInventoryComponent::UnequipItem(EEquipSlot EquipSlot)
{
	if (!bHasEquipmentSlots || !IsEquipSlotAvailable(EquipSlot))
		return false;

	UItemBase* ItemToUnequip = EquipmentSlots.FindRef(EquipSlot);
	if (!ItemToUnequip)
		return false;

	// Equipped passive efektlerini kaldır
	if (GetOwner()) { IEquippable::Execute_OnUnequip(ItemToUnequip, GetOwner()); }

	// Remove from equipment slot
	EquipmentSlots.Add(EquipSlot, nullptr);

	// Delegate broadcast et
	OnItemUnequipped.Broadcast(ItemToUnequip, EquipSlot);

	// Try to add back to inventory
	if (!AddItem(ItemToUnequip)) { ItemToUnequip->ConditionalBeginDestroy(); }

	BroadcastInventoryChange();
	return true;
}

UItemBase* UInventoryComponent::GetEquippedItem(EEquipSlot EquipSlot) const
{
	if (!bHasEquipmentSlots)
		return nullptr;

	// Use Find() for const-correctness
	if (const UItemBase* const* ItemPtr = EquipmentSlots.Find(EquipSlot))
	{
		// Cast away const since we're returning non-const pointer
		// This is safe because we're not modifying the map itself
		return const_cast<UItemBase*>(*ItemPtr);
	}

	return nullptr;
}

TMap<EEquipSlot, UItemBase*> UInventoryComponent::GetAllEquippedItems() const { return EquipmentSlots; }

bool UInventoryComponent::IsEquipSlotAvailable(EEquipSlot Slot) const
{
	return bHasEquipmentSlots && AvailableEquipSlots.Contains(Slot);
}

bool UInventoryComponent::IsEquipSlotEmpty(EEquipSlot Slot) const
{
	if (!IsEquipSlotAvailable(Slot))
		return false;

	UItemBase* Item = EquipmentSlots.FindRef(Slot);
	return Item == nullptr;
}

bool UInventoryComponent::SwapEquipment(EEquipSlot SlotA, EEquipSlot SlotB)
{
	if (!IsEquipSlotAvailable(SlotA) || !IsEquipSlotAvailable(SlotB))
		return false;

	UItemBase* ItemA = EquipmentSlots.FindRef(SlotA);
	UItemBase* ItemB = EquipmentSlots.FindRef(SlotB);

	// Swap items
	EquipmentSlots.Add(SlotA, ItemB);
	EquipmentSlots.Add(SlotB, ItemA);

	BroadcastInventoryChange();
	return true;
}

// Utility Functions
void UInventoryComponent::SortInventory()
{
	// Simple sort by item ID
	InventorySlots.Sort([](const FInventorySlot& A, const FInventorySlot& B)
	{
		if (!A.Item && !B.Item) return false;
		if (!A.Item) return false;
		if (!B.Item) return true;
		return A.Item->GetItemID() < B.Item->GetItemID();
	});

	// Reassign slot indices
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].Item) { InventorySlots[i].SlotIndex = i; }
	}

	BroadcastInventoryChange();
}

void UInventoryComponent::ClearInventory()
{
	// Clear inventory items
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].Item)
		{
			UItemBase* ItemToRemove = InventorySlots[i].Item;

			if (GetOwner()) { ItemToRemove->OnRemovedFromInventory(); }

			InventorySlots[i] = FInventorySlot(nullptr, i);
			OnItemRemoved.Broadcast(ItemToRemove, i);
		}
	}

	// Clear equipment
	if (bHasEquipmentSlots)
	{
		TArray<EEquipSlot> SlotsToUnequip;
		for (const auto& Pair : EquipmentSlots) { if (Pair.Value) { SlotsToUnequip.Add(Pair.Key); } }

		for (EEquipSlot Slot : SlotsToUnequip) { UnequipItem(Slot); }
	}

	BroadcastInventoryChange();
}

TArray<UItemBase*> UInventoryComponent::GetAllItems() const
{
	TArray<UItemBase*> Items;

	for (const FInventorySlot& Slot : InventorySlots) { if (Slot.Item) { Items.Add(Slot.Item); } }

	return Items;
}

int32 UInventoryComponent::GetTotalItemCount() const
{
	int32 Count = 0;

	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Item) { Count += Slot.Item->IsStackable() ? Slot.Item->GetCurrentStackSize() : 1; }
	}

	return Count;
}

// Protected Helper Functions
int32 UInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < InventorySlots.Num(); i++) { if (!InventorySlots[i].Item) { return i; } }
	return -1;
}

int32 UInventoryComponent::FindStackableSlot(UItemBase* Item) const
{
	if (!Item || !Item->IsStackable())
		return -1;

	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		UItemBase* ExistingItem = InventorySlots[i].Item;
		if (ExistingItem && ExistingItem->CanMergeWith(Item)) { return i; }
	}

	return -1;
}

bool UInventoryComponent::InternalAddItem(UItemBase* Item, int32 SlotIndex)
{
	if (!Item || SlotIndex < 0 || SlotIndex >= InventorySlots.Num() || InventorySlots[SlotIndex].Item)
		return false;

	InventorySlots[SlotIndex] = FInventorySlot(Item, SlotIndex);

	// Inventory passive efektlerini uygula
	if (GetOwner()) { Item->OnAddedToInventory(GetOwner()); }

	OnItemAdded.Broadcast(Item, SlotIndex);
	BroadcastInventoryChange();
	return true;
}

void UInventoryComponent::BroadcastInventoryChange() { OnInventoryChanged(); }

bool UInventoryComponent::AddItemByClass(TSubclassOf<UItemBase> ItemClass, int32 Quantity)
{
	if (!ItemClass || Quantity <= 0)
		return false;

	bool bSuccess = true;

	for (int32 i = 0; i < Quantity; i++)
	{
		UItemBase* NewItem = CreateItemInstance(ItemClass);
		if (NewItem)
		{
			if (!AddItem(NewItem))
			{
				NewItem->ConditionalBeginDestroy();
				bSuccess = false;
			}
		}
		else { bSuccess = false; }
	}

	return bSuccess;
}

bool UInventoryComponent::AddItemsByClasses(const TArray<TSubclassOf<UItemBase>>& ItemClasses)
{
	bool bAllSuccess = true;

	for (const TSubclassOf<UItemBase>& ItemClass : ItemClasses)
	{
		if (!AddItemByClass(ItemClass)) { bAllSuccess = false; }
	}

	return bAllSuccess;
}

bool UInventoryComponent::EquipItemByClass(TSubclassOf<UItemBase> ItemClass)
{
	if (!ItemClass)
		return false;

	UItemBase* NewItem = CreateItemInstance(ItemClass);
	if (!NewItem)
		return false;

	// Önce inventory'e ekle
	if (AddItem(NewItem))
	{
		// Sonra equip et
		return EquipItem(NewItem);
	}

	NewItem->ConditionalBeginDestroy();
	return false;
}

void UInventoryComponent::AddStartingItemsInEditor(const TArray<TSubclassOf<UItemBase>>& ItemsToAdd)
{
#if WITH_EDITOR
	for (const TSubclassOf<UItemBase>& ItemClass : ItemsToAdd) { AddItemByClass(ItemClass); }
#endif
}

void UInventoryComponent::Debug_ClearAndAddItems(const TArray<TSubclassOf<UItemBase>>& ItemsToAdd)
{
#if WITH_EDITOR || UE_BUILD_DEBUG
	ClearInventory();
	AddItemsByClasses(ItemsToAdd);
#endif
}

UItemBase* UInventoryComponent::CreateItemInstance(TSubclassOf<UItemBase> ItemClass)
{
	if (!ItemClass)
		return nullptr;

	UItemBase* NewItem = NewObject<UItemBase>(this, ItemClass);
	if (NewItem) { NewItem->InitializeItem(); }

	return NewItem;
}

void UInventoryComponent::SetAvailableEquipSlots(const TArray<EEquipSlot>& Slots)
{
	AvailableEquipSlots = Slots;

	if (bHasEquipmentSlots)
	{
		// Mevcut ekipmanları temizle ve yeni slotları oluştur
		TMap<EEquipSlot, UItemBase*> OldEquipment = EquipmentSlots;
		EquipmentSlots.Empty();

		for (const EEquipSlot& Slot : AvailableEquipSlots)
		{
			// Eğer eski ekipmanda bu slot varsa koru
			UItemBase* ExistingItem = OldEquipment.FindRef(Slot);
			EquipmentSlots.Add(Slot, ExistingItem);
		}
	}
}

UItemBase* UInventoryComponent::GetItemByName(FString NameToSearch) const
{
	if (NameToSearch.IsEmpty()) return nullptr;

	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Item)
		{
			// FText'i FString'e çevirip karşılaştırıyoruz
			FString CurrentItemName = Slot.Item->GetItemName().ToString();

			// Equals ile büyük/küçük harf bakmaksızın (CaseInsensitive) kontrol ediyoruz
			if (CurrentItemName.Equals(NameToSearch, ESearchCase::IgnoreCase))
			{
				return Slot.Item;
			}
		}
	}

	return nullptr;
}

TArray<UItemBase*> UInventoryComponent::GetAllItemsByName(FString NameToSearch) const
{
	TArray<UItemBase*> FoundItems;
	if (NameToSearch.IsEmpty()) return FoundItems;

	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Item)
		{
			FString CurrentItemName = Slot.Item->GetItemName().ToString();

			if (CurrentItemName.Equals(NameToSearch, ESearchCase::IgnoreCase))
			{
				FoundItems.Add(Slot.Item);
			}
		}
	}

	return FoundItems;
}

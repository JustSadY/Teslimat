#include "ItemModuleBase.h"
#include "Items/ItemBase.h"

UItemModuleBase::UItemModuleBase()
{
	OwnerItem = nullptr;
	bIsModuleActive = true;
	ModuleDescription = FText::GetEmpty();
	Priority = 100;
}

void UItemModuleBase::Initialize(UItemBase* InOwnerItem)
{
	if (!InOwnerItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemModuleBase::Initialize - Invalid owner item!"));
		return;
	}

	OwnerItem = InOwnerItem;
	bIsModuleActive = true;
}

void UItemModuleBase::Reset()
{
	bIsModuleActive = true;
}

// ================= Core Events =================

void UItemModuleBase::OnItemAddedToInventory_Implementation(AActor* Owner)
{
	// Base implementation - override in child classes
}

void UItemModuleBase::OnItemRemovedFromInventory_Implementation()
{
	// Base implementation - override in child classes
}

void UItemModuleBase::OnItemEquipped_Implementation(AActor* OwningActor)
{
	// Base implementation - override in child classes
}

void UItemModuleBase::OnItemUnequipped_Implementation(AActor* OwningActor)
{
	// Base implementation - override in child classes
}

void UItemModuleBase::OnItemUsed_Implementation(AActor* User)
{
	// Base implementation - override in child classes
}

void UItemModuleBase::OnItemDropped_Implementation(AActor* Dropper)
{
	// Base implementation - override in child classes
}

// ================= Stack Events =================

void UItemModuleBase::OnItemMerged_Implementation(UItemBase* OtherItem, bool bIsSource)
{
	// Base implementation - override in child classes
}

void UItemModuleBase::OnItemSplit_Implementation(UItemBase* NewItem, int32 Amount)
{
	// Base implementation - override in child classes
}

// ================= Update =================

void UItemModuleBase::TickModule_Implementation(float DeltaTime)
{
	// Base implementation - override in child classes
	// Not: Performans için dikkatli kullanılmalı
}

// ================= Data =================

FText UItemModuleBase::GetModuleTooltip_Implementation() const
{
	return ModuleDescription;
}

TMap<FString, FString> UItemModuleBase::SerializeModuleData_Implementation() const
{
	TMap<FString, FString> Data;
	Data.Add(TEXT("bIsModuleActive"), bIsModuleActive ? TEXT("true") : TEXT("false"));
	Data.Add(TEXT("Priority"), FString::FromInt(Priority));
	return Data;
}

void UItemModuleBase::DeserializeModuleData_Implementation(const TMap<FString, FString>& Data)
{
	if (const FString* ActiveValue = Data.Find(TEXT("bIsModuleActive")))
	{
		bIsModuleActive = (*ActiveValue == TEXT("true"));
	}

	if (const FString* PriorityValue = Data.Find(TEXT("Priority")))
	{
		Priority = FCString::Atoi(**PriorityValue);
	}
}

UItemModuleBase* UItemModuleBase::DuplicateModule(UItemBase* TargetItem)
{
	if (!TargetItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemModuleBase::DuplicateModule - Invalid target item!"));
		return nullptr;
	}

	UItemModuleBase* NewModule = NewObject<UItemModuleBase>(TargetItem, GetClass());

	if (NewModule)
	{
		NewModule->Initialize(TargetItem);
		NewModule->bIsModuleActive = bIsModuleActive;
		NewModule->ModuleDescription = ModuleDescription;
		NewModule->Priority = Priority;

		// Serialize ve deserialize ile veri kopyalama
		TMap<FString, FString> Data = SerializeModuleData();
		NewModule->DeserializeModuleData(Data);
	}

	return NewModule;
}

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Interfaces/Equippable.h"
#include "Modulle/ItemModuleBase.h"
#include "ItemBase.generated.h"

class UTexture2D;

/**
 * UItemBase
 * The foundational class for all items in the system.
 * Uses a component-based approach via ItemModules.
 */
UCLASS(Blueprintable, Abstract)
class INVENTORYSYSTEM_API UItemBase : public UObject, public IEquippable
{
	GENERATED_BODY()

public:
	UItemBase();

	// --- Core Lifecycle ---
	virtual void InitializeItem_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Item|Lifecycle")
	void InitializeItem();

	// --- Identification ---
	UFUNCTION(BlueprintPure, Category = "Item|Info")
	FString GetItemID() const { return ItemID; }

	UFUNCTION(BlueprintPure, Category = "Item|Info")
	FText GetItemName() const { return ItemName; }

	UFUNCTION(BlueprintPure, Category = "Item|Info")
	UTexture2D* GetItemIcon() const { return ItemIcon; }

	UFUNCTION(BlueprintPure, Category = "Item|Info")
	FORCEINLINE bool IsStackable() const { return bIsStackable; }

	UFUNCTION(BlueprintPure, Category = "Item|Stacking")
	FORCEINLINE int32 GetCurrentStackSize() const { return CurrentStackSize; }

	UFUNCTION(BlueprintPure, Category = "Item|Stacking")
	FORCEINLINE int32 GetMaxStackSize() const { return MaxStackSize; }

	UFUNCTION(BlueprintPure, Category = "Item|Capabilities")
	FORCEINLINE bool IsEquippable() const { return bIsEquippable; }

	// --- Stacking Logic ---
	UFUNCTION(BlueprintCallable, Category = "Item|Stacking")
	void SetCurrentStackSize(int32 NewSize);

	UFUNCTION(BlueprintPure, Category = "Item|Stacking")
	bool CanMergeWith(const UItemBase* OtherItem) const;

	UFUNCTION(BlueprintCallable, Category = "Item|Stacking")
	bool MergeWith(UItemBase* OtherItem);

	UFUNCTION(BlueprintCallable, Category = "Item|Stacking")
	UItemBase* SplitStack(int32 Amount);

	// --- Inventory & Equipment State ---
	UFUNCTION(BlueprintCallable, Category = "Item|State")
	void OnAddedToInventory(AActor* NewOwner);

	UFUNCTION(BlueprintCallable, Category = "Item|State")
	void OnRemovedFromInventory();

	virtual void OnEquip_Implementation(AActor* OwningActor) override;
	virtual void OnUnequip_Implementation(AActor* OwningActor) override;

	UFUNCTION(BlueprintPure, Category = "Item|State")
	bool IsInInventory() const { return bIsInInventory; }

	UFUNCTION(BlueprintPure, Category = "Item|State")
	bool IsEquipped() const { return bIsEquipped; }

	UFUNCTION(BlueprintPure, Category = "Item|State")
	AActor* GetOwner() const { return OwnerActor; }

	// --- Module Management ---
	UFUNCTION(BlueprintCallable, Category = "Item|Modules")
	void AddModule(UItemModuleBase* NewModule);

	UFUNCTION(BlueprintCallable, Category = "Item|Modules")
	void RemoveModule(UItemModuleBase* ModuleToRemove);

	UFUNCTION(BlueprintPure, Category = "Item|Modules")
	UItemModuleBase* GetModuleByClass(TSubclassOf<UItemModuleBase> ModuleClass) const;

	void TickModules(float DeltaTime);

	template <typename T>
	T* GetModule() const
	{
		for (UItemModuleBase* Module : ItemModules)
		{
			if (T* Casted = Cast<T>(Module)) return Casted;
		}
		return nullptr;
	}

protected:
	// Basic Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Data")
	FString ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Data")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Data", meta = (MultiLine = true))
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Data")
	UTexture2D* ItemIcon;

	// Stacking Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stacking")
	bool bIsStackable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stacking", meta = (EditCondition = "bIsStackable"))
	int32 MaxStackSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Stacking")
	int32 CurrentStackSize;

	// Capability Flags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Capabilities")
	bool bIsEquippable;

	// Internal State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Internal")
	AActor* OwnerActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Internal")
	bool bIsInInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Internal")
	bool bIsEquipped;

	// Modules
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Item|Modules")
	TArray<UItemModuleBase*> ItemModules;

	// Helper to generate IDs
	FString GenerateUniqueItemID();

	// Blueprint Events
	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnItemEquipped(AActor* Character);

	UFUNCTION(BlueprintNativeEvent, Category = "Item|Events")
	void OnItemUnequipped(AActor* Character);
};

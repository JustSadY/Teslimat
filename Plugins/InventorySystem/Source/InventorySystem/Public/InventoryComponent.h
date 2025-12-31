// InventoryComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemBase.h"
#include "Enum&&Struct/EquipSlot.h"
#include "InventoryComponent.generated.h"

// Delegate declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, UItemBase*, Item, int32, SlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, UItemBase*, Item, int32, SlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemStackChanged, UItemBase*, Item, int32, SlotIndex, int32, OldAmount,
                                              int32, NewAmount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryFull, UItemBase*, Item, int32, RequiredSlots);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquippedSignature, UItemBase*, Item, EEquipSlot, Slot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUnequippedSignature, UItemBase*, Item, EEquipSlot, Slot);

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UItemBase* Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SlotIndex;

	FInventorySlot() : Item(nullptr), SlotIndex(-1)
	{
	}

	FInventorySlot(UItemBase* InItem, int32 InSlotIndex)
		: Item(InItem), SlotIndex(InSlotIndex)
	{
	}

	bool IsValid() const { return Item != nullptr && SlotIndex >= 0; }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Constructors
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Maksimum envanter slot sayısı */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 MaxInventorySlots;

	/** Otomatik stackleme aktif mi? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	bool bAutoStackItems;

	/** Ekipman slotları aktif mi? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	bool bHasEquipmentSlots;

	/** Hangi ekipman slotları aktif (örnek: Head, Chest, Legs vs.) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment",
		meta = (EditCondition = "bHasEquipmentSlots"))
	TArray<EEquipSlot> AvailableEquipSlots;

	/** Envanter slotları */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventorySlot> InventorySlots;

	/** Ekipman slotları - EEquipSlot'a göre organize edilmiş */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipSlot, UItemBase*> EquipmentSlots;

public:
	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemStackChanged OnItemStackChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryFull OnInventoryFull;

	UPROPERTY(BlueprintAssignable, Category = "Equipment|Events")
	FOnItemEquippedSignature OnItemEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Equipment|Events")
	FOnItemUnequippedSignature OnItemUnequipped;

protected:
	/** Blueprint event when inventory changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnInventoryChanged();

public:
	// Inventory Management Functions
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemAt(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemBase* GetItemAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 FindItemSlot(UItemBase* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(UItemBase* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(UItemBase* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsInventoryFull() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetEmptySlotCount() const;

	// Stacking Functions
	UFUNCTION(BlueprintCallable, Category = "Inventory|Stacking")
	bool TryStackItem(UItemBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Stacking")
	bool SplitItemStack(int32 FromSlot, int32 ToSlot, int32 Amount);

	// Equipment Functions - UPDATED
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipItem(UItemBase* Item);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipItem(EEquipSlot EquipSlot);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UItemBase* GetEquippedItem(EEquipSlot EquipSlot) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	TMap<EEquipSlot, UItemBase*> GetAllEquippedItems() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool IsEquipSlotAvailable(EEquipSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool IsEquipSlotEmpty(EEquipSlot Slot) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool SwapEquipment(EEquipSlot SlotA, EEquipSlot SlotB);

	// Utility Functions
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SortInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<UItemBase*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetTotalItemCount() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (DisplayName = "Add Item By Class"))
	bool AddItemByClass(TSubclassOf<UItemBase> ItemClass, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (DisplayName = "Add Multiple Items By Classes"))
	bool AddItemsByClasses(const TArray<TSubclassOf<UItemBase>>& ItemClasses);

	UFUNCTION(BlueprintCallable, Category = "Equipment", meta = (DisplayName = "Equip Item By Class"))
	bool EquipItemByClass(TSubclassOf<UItemBase> ItemClass);

	// Editor/Development için utility fonksiyonlar
	UFUNCTION(BlueprintCallable, Category = "Inventory|Editor", meta = (DevelopmentOnly))
	void AddStartingItemsInEditor(const TArray<TSubclassOf<UItemBase>>& ItemsToAdd);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Debug",
		meta = (DevelopmentOnly, DisplayName = "Clear And Add Items"))
	void Debug_ClearAndAddItems(const TArray<TSubclassOf<UItemBase>>& ItemsToAdd);

protected:
	// Internal Helper Functions
	int32 FindEmptySlot() const;
	int32 FindStackableSlot(UItemBase* Item) const;
	bool InternalAddItem(UItemBase* Item, int32 SlotIndex);
	void BroadcastInventoryChange();
	UItemBase* CreateItemInstance(TSubclassOf<UItemBase> ItemClass);

public:
	void SetMaxInventorySlots(int32 Index) { MaxInventorySlots = Index; }

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetAvailableEquipSlots(const TArray<EEquipSlot>& Slots);
	
	/** İsime göre ilk eşleşen item'ı getirir (Büyük/Küçük harf duyarsız) */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemBase* GetItemByName(FString NameToSearch) const;

	/** İsime göre eşleşen TÜM item'ları getirir */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<UItemBase*> GetAllItemsByName(FString NameToSearch) const;
};

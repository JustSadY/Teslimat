#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemModuleBase.generated.h"

class UItemBase;

/**
 * Modül sistemi - Item'lara eklenti bazlı özellikler kazandırır
 * Composition over inheritance yaklaşımı kullanır
 * 
 * Örnek Modüller:
 * - DurabilityModule: Dayanıklılık sistemi
 * - RarityModule: Nadirlik seviyesi
 * - EnhancementModule: Geliştirme sistemi
 * - StatsModule: İstatistik bonusları
 * - CooldownModule: Kullanım bekleme süresi
 */
UCLASS(Blueprintable, Abstract, EditInlineNew, DefaultToInstanced)
class INVENTORYSYSTEM_API UItemModuleBase : public UObject
{
	GENERATED_BODY()

public:
	UItemModuleBase();

protected:
	/** Modülün bağlı olduğu item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module")
	UItemBase* OwnerItem;

	/** Modülün aktif olup olmadığı */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module")
	bool bIsModuleActive;

	/** Modülün açıklaması */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Module")
	FText ModuleDescription;

	/** Modülün öncelik seviyesi (düşük değer = önce işlenir) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Module")
	int32 Priority;

public:
	// ================= Initialization =================

	/**
	 * Modülü başlatır ve item'a bağlar
	 * @param InOwnerItem - Bu modülün bağlanacağı item
	 */
	UFUNCTION(BlueprintCallable, Category = "Module")
	virtual void Initialize(UItemBase* InOwnerItem);

	/**
	 * Modül verilerini sıfırlar
	 */
	UFUNCTION(BlueprintCallable, Category = "Module")
	virtual void Reset();

	// ================= Core Events =================

	/**
	 * Item envantere eklendiğinde çağrılır
	 * @param Owner - Item'ın sahibi olan actor
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemAddedToInventory(AActor* Owner);
	virtual void OnItemAddedToInventory_Implementation(AActor* Owner);

	/**
	 * Item envanterden çıkarıldığında çağrılır
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemRemovedFromInventory();
	virtual void OnItemRemovedFromInventory_Implementation();

	/**
	 * Item kuşanıldığında çağrılır
	 * @param OwningActor - Item'ı kuşanan actor
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemEquipped(AActor* OwningActor);
	virtual void OnItemEquipped_Implementation(AActor* OwningActor);

	/**
	 * Item çıkarıldığında çağrılır
	 * @param OwningActor - Item'ı çıkaran actor
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemUnequipped(AActor* OwningActor);
	virtual void OnItemUnequipped_Implementation(AActor* OwningActor);

	/**
	 * Item kullanıldığında çağrılır
	 * @param User - Item'ı kullanan actor
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemUsed(AActor* User);
	virtual void OnItemUsed_Implementation(AActor* User);

	/**
	 * Item drop edildiğinde çağrılır
	 * @param Dropper - Item'ı drop eden actor
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemDropped(AActor* Dropper);
	virtual void OnItemDropped_Implementation(AActor* Dropper);

	// ================= Stack Events =================

	/**
	 * Item merge edildiğinde çağrılır
	 * @param OtherItem - Merge edilen diğer item
	 * @param bIsSource - Bu item merge'ün kaynağı mı (true) yoksa hedefi mi (false)
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemMerged(UItemBase* OtherItem, bool bIsSource);
	virtual void OnItemMerged_Implementation(UItemBase* OtherItem, bool bIsSource);

	/**
	 * Item split edildiğinde çağrılır
	 * @param NewItem - Split sonucu oluşan yeni item
	 * @param Amount - Split edilen miktar
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Events")
	void OnItemSplit(UItemBase* NewItem, int32 Amount);
	virtual void OnItemSplit_Implementation(UItemBase* NewItem, int32 Amount);

	// ================= Update =================

	/**
	 * Her frame çağrılabilir (performans için dikkatli kullanılmalı)
	 * @param DeltaTime - Son frame'den beri geçen süre
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module")
	void TickModule(float DeltaTime);
	virtual void TickModule_Implementation(float DeltaTime);

	// ================= Data =================

	/**
	 * Modülün UI için tooltip bilgisi döndürür
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Module|UI")
	FText GetModuleTooltip() const;
	virtual FText GetModuleTooltip_Implementation() const;

	/**
	 * Modülün kayıt edilebilir verisini döndürür
	 * Save/Load için kullanılır
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Save")
	TMap<FString, FString> SerializeModuleData() const;
	virtual TMap<FString, FString> SerializeModuleData_Implementation() const;

	/**
	 * Kayıtlı veriyi modüle yükler
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Module|Save")
	void DeserializeModuleData(const TMap<FString, FString>& Data);
	virtual void DeserializeModuleData_Implementation(const TMap<FString, FString>& Data);

	/**
	 * Modülü başka bir item'a kopyalar
	 * @param TargetItem - Hedef item
	 * @return Kopyalanan yeni modül instance'ı
	 */
	UFUNCTION(BlueprintCallable, Category = "Module")
	virtual UItemModuleBase* DuplicateModule(UItemBase* TargetItem);

	// ================= Getters =================

	UFUNCTION(BlueprintPure, Category = "Module")
	UItemBase* GetOwnerItem() const { return OwnerItem; }

	UFUNCTION(BlueprintPure, Category = "Module")
	bool IsModuleActive() const { return bIsModuleActive; }

	UFUNCTION(BlueprintPure, Category = "Module")
	FText GetModuleDescription() const { return ModuleDescription; }

	UFUNCTION(BlueprintPure, Category = "Module")
	int32 GetPriority() const { return Priority; }

	// ================= Setters =================

	UFUNCTION(BlueprintCallable, Category = "Module")
	void SetModuleActive(bool bActive) { bIsModuleActive = bActive; }
};

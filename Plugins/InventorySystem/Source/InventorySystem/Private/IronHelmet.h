// IronHelmet.h - Tam Özellikli Equipment Item Örneği
#pragma once

#include "CoreMinimal.h"
#include "Items/WearableItems/WearableItem.h"
#include "IronHelmet.generated.h"

/**
 * Iron Helmet - Demir Miğfer
 * 
 * Örnek bir equipment item'ı:
 * - Head slotuna takılır
 * - Skeletal mesh kullanır
 * - Saç mesh'ini gizler
 * - Stat bonusları verir
 * - Hem inventory hem equipped passive'leri var
 * - Visual ve sound effect'leri var
 */
UCLASS(Blueprintable)
class INVENTORYSYSTEM_API UIronHelmet : public UWearableItem
{
	GENERATED_BODY()

public:
	UIronHelmet();

protected:
	// ============================================================================
	// Custom Properties (Blueprint'te override edilebilir)
	// ============================================================================

	/** Helmet seviyesi (upgrade sistemi için) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Properties")
	int32 HelmetLevel;

	/** Dayanıklılık (durability) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Properties")
	float Durability;

	/** Maksimum dayanıklılık */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Properties")
	float MaxDurability;

	/** Helmet rarity (Common, Rare, Epic, Legendary) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Properties")
	FName Rarity;

	// ============================================================================
	// Special Abilities
	// ============================================================================

	/** Helmet'in özel yeteneği var mı? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Abilities")
	bool bHasSpecialAbility;

	/** Özel yetenek açıklaması */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Abilities",
		meta = (EditCondition = "bHasSpecialAbility"))
	FText SpecialAbilityDescription;

	/** Baş vuruş hasarını azaltma oranı */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Abilities")
	float HeadshotDamageReduction;

	// ============================================================================
	// Visual Customization
	// ============================================================================

	/** Helmet renk varyantları */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Visual")
	TArray<FLinearColor> ColorVariants;

	/** Seçili renk indeksi */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Visual")
	int32 SelectedColorIndex;

	/** Glow efekti var mı? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Visual")
	bool bHasGlowEffect;

	/** Glow rengi */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Visual",
		meta = (EditCondition = "bHasGlowEffect"))
	FLinearColor GlowColor;

	/** Glow yoğunluğu */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Helmet|Visual",
		meta = (EditCondition = "bHasGlowEffect"))
	float GlowIntensity;

public:
	// ============================================================================
	// Public Functions
	// ============================================================================

	/** Dayanıklılığı azalt */
	UFUNCTION(BlueprintCallable, Category = "Helmet")
	void ReduceDurability(float Amount);

	/** Helmet'i onar */
	UFUNCTION(BlueprintCallable, Category = "Helmet")
	void RepairHelmet(float Amount);

	/** Dayanıklılık yüzdesi */
	UFUNCTION(BlueprintPure, Category = "Helmet")
	float GetDurabilityPercent() const;

	/** Helmet kırık mı? */
	UFUNCTION(BlueprintPure, Category = "Helmet")
	bool IsBroken() const;

	/** Renk varyantını değiştir */
	UFUNCTION(BlueprintCallable, Category = "Helmet")
	void ChangeColorVariant(int32 NewColorIndex);

	/** Özel yeteneği aktif et */
	UFUNCTION(BlueprintCallable, Category = "Helmet")
	void ActivateSpecialAbility(APawn* Owner);

	/** Helmet seviyesini artır */
	UFUNCTION(BlueprintCallable, Category = "Helmet")
	void UpgradeHelmet();

protected:
	// ============================================================================
	// Override Functions
	// ============================================================================

	virtual void OnEquip_Implementation(AActor* OwningPawn) override;
	virtual void OnUnequip_Implementation(AActor* OwningPawn) override;


	// WearableItem'dan override
	virtual void OnMeshAttached_Implementation(AActor* Character, UMeshComponent* MeshComponent) override;
	virtual void OnMeshDetached_Implementation(AActor* Character, UMeshComponent* MeshComponent) override;
	virtual void OnStatsApplied_Implementation(AActor* Character) override;
	virtual void OnStatsRemoved_Implementation(AActor* Character) override;

	// ============================================================================
	// Helper Functions
	// ============================================================================

	/** Material'i güncelle (renk, glow vs.) */
	void UpdateMaterialParameters();

	/** Glow efektini uygula */
	void ApplyGlowEffect();

	/** Glow efektini kaldır */
	void RemoveGlowEffect();

	/** Helmet'in stat bonuslarını seviyeye göre hesapla */
	void CalculateStatsForLevel();

	/** Particle sistem referansı */
	UPROPERTY()
	UParticleSystemComponent* GlowParticleComponent;

	/** Dynamic material instance */
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;
};

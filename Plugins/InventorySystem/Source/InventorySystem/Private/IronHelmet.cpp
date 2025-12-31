// IronHelmet.cpp
#include "IronHelmet.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"

UIronHelmet::UIronHelmet()
{
	HelmetLevel = 1;
	MaxDurability = 100.f;
	Durability = MaxDurability;
	Rarity = "Common";
	bHasSpecialAbility = false;
	HeadshotDamageReduction = 0.f;
	SelectedColorIndex = 0;
	bHasGlowEffect = false;

	GlowParticleComponent = nullptr;
	DynamicMaterial = nullptr;
}

// ============================
// Public Functions
// ============================

void UIronHelmet::ReduceDurability(float Amount)
{
	Durability = FMath::Clamp(Durability - Amount, 0.f, MaxDurability);
}

void UIronHelmet::RepairHelmet(float Amount)
{
	Durability = FMath::Clamp(Durability + Amount, 0.f, MaxDurability);
}

float UIronHelmet::GetDurabilityPercent() const
{
	return (MaxDurability > 0) ? Durability / MaxDurability : 1.f;
}

bool UIronHelmet::IsBroken() const
{
	return Durability <= 0.f;
}

void UIronHelmet::ChangeColorVariant(int32 NewColorIndex)
{
	if (ColorVariants.IsValidIndex(NewColorIndex))
	{
		SelectedColorIndex = NewColorIndex;
		UpdateMaterialParameters();
	}
}

void UIronHelmet::ActivateSpecialAbility(APawn* Owner)
{
	if (bHasSpecialAbility)
	{
		// Özel yetenek mantığı buraya
	}
}

void UIronHelmet::UpgradeHelmet()
{
	HelmetLevel++;
	CalculateStatsForLevel();
}

// ============================
// Override Functions
// ============================

void UIronHelmet::OnEquip_Implementation(AActor* OwningPawn)
{
	Super::OnEquip_Implementation(OwningPawn);
	// Mesh attach, stat apply vb.

	OnMeshAttached_Implementation(OwningPawn, nullptr); // Örnek, mesh null
	OnStatsApplied_Implementation(OwningPawn);
}

void UIronHelmet::OnUnequip_Implementation(AActor* OwningPawn)
{
	Super::OnUnequip_Implementation(OwningPawn);
	OnMeshDetached_Implementation(OwningPawn, nullptr);
	OnStatsRemoved_Implementation(OwningPawn);
}

void UIronHelmet::OnMeshAttached_Implementation(AActor* Character, UMeshComponent* MeshComponent)
{
	// Örnek: Glow particle ekle
	if (bHasGlowEffect && !GlowParticleComponent)
	{
		// Particle yarat
	}
	UpdateMaterialParameters();
}

void UIronHelmet::OnMeshDetached_Implementation(AActor* Character, UMeshComponent* MeshComponent)
{
	RemoveGlowEffect();
}

void UIronHelmet::OnStatsApplied_Implementation(AActor* Character)
{
	Super::OnStatsApplied_Implementation(Character);
	// Stat bonusları uygula
}

void UIronHelmet::OnStatsRemoved_Implementation(AActor* Character)
{
	Super::OnStatsRemoved_Implementation(Character);
	// Stat bonusları kaldır
}

// ============================
// Helper Functions
// ============================

void UIronHelmet::UpdateMaterialParameters()
{
	if (!DynamicMaterial) return;

	if (ColorVariants.IsValidIndex(SelectedColorIndex))
	{
		DynamicMaterial->SetVectorParameterValue(FName("BaseColor"), ColorVariants[SelectedColorIndex]);
	}

	if (bHasGlowEffect)
	{
		ApplyGlowEffect();
	}
}

void UIronHelmet::ApplyGlowEffect()
{
	if (GlowParticleComponent)
	{
		GlowParticleComponent->Activate(true);
	}
}

void UIronHelmet::RemoveGlowEffect()
{
	if (GlowParticleComponent)
	{
		GlowParticleComponent->Deactivate();
	}
}

void UIronHelmet::CalculateStatsForLevel()
{
	// Örnek: Headshot damage reduction seviyeye göre artar
	HeadshotDamageReduction = HelmetLevel * 0.05f;
}

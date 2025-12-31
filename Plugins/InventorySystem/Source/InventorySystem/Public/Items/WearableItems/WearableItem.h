// WearableItem.h
#pragma once

#include "CoreMinimal.h"
#include "Enum&&Struct/EquipSlot.h"
#include "Interfaces/EquipmentMeshHandler.h"
#include "Items/ItemBase.h"
#include "WearableItem.generated.h"

UCLASS(Blueprintable, Abstract)
class INVENTORYSYSTEM_API UWearableItem : public UItemBase
{
	GENERATED_BODY()

public:
	UWearableItem() { bIsEquippable = true; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	EEquipSlot EquipSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSoftObjectPtr<USkeletalMesh> ItemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FVector MeshOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FRotator MeshRotation = FRotator::ZeroRotator;


	virtual void OnUnequip_Implementation(AActor* OwningActor) override
	{
		Super::OnUnequip_Implementation(OwningActor);
		if (!OwningActor || !bIsEquippable)
			return;

		// NEW: Remove equipment mesh
		if (OwningActor->GetClass()->ImplementsInterface(UEquipmentMeshHandler::StaticClass()))
		{
			IEquipmentMeshHandler::Execute_DetachEquipmentMesh(OwningActor, EquipSlot);
		}
	}

	virtual void OnEquip_Implementation(AActor* OwningPawn) override
	{
		Super::OnEquip_Implementation(OwningPawn);
		if (!OwningPawn || !bIsEquippable)
			return;

		// Mesh yükle ve ekle
		if (!ItemMesh.IsNull())
		{
			if (USkeletalMesh* LoadedMesh = ItemMesh.LoadSynchronous())
			{
				FEquipmentMeshInfo MeshInfo;
				MeshInfo.SkeletalMesh = LoadedMesh;
				MeshInfo.SocketName = AttachSocketName;
				MeshInfo.RelativeTransform.SetLocation(MeshOffset);
				MeshInfo.RelativeTransform.SetRotation(MeshRotation.Quaternion());

				if (OwningPawn->GetClass()->ImplementsInterface(UEquipmentMeshHandler::StaticClass()))
				{
					IEquipmentMeshHandler::Execute_AttachEquipmentMesh(OwningPawn, EquipSlot, MeshInfo);
				}
			}
		}
	}

protected:
	/** Mesh karaktere eklendiğinde çalışır */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mesh")
	void OnMeshAttached(AActor* Character, UMeshComponent* MeshComponent);

	virtual void OnMeshAttached_Implementation(AActor* Character, UMeshComponent* MeshComponent)
	{
	}

	/** Mesh karakterden çıkarıldığında çalışır */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mesh")
	void OnMeshDetached(AActor* Character, UMeshComponent* MeshComponent);

	virtual void OnMeshDetached_Implementation(AActor* Character, UMeshComponent* MeshComponent)
	{
	}

	/** Karakterin istatistikleri uygulandığında çalışır */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Stats")
	void OnStatsApplied(AActor* Character);

	virtual void OnStatsApplied_Implementation(AActor* Character)
	{
	}

	/** Karakterin istatistikleri kaldırıldığında çalışır */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Stats")
	void OnStatsRemoved(AActor* Character);

	virtual void OnStatsRemoved_Implementation(AActor* Character)
	{
	}
};

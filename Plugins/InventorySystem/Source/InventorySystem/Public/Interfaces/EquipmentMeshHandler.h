// IEquipmentMeshHandler.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enum&&Struct/EquipSlot.h"
#include "EquipmentMeshHandler.generated.h"

// Equipment mesh bilgisi için struct
USTRUCT(BlueprintType)
struct FEquipmentMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FTransform RelativeTransform;

	FEquipmentMeshInfo()
		: SkeletalMesh(nullptr)
		  , SocketName(NAME_None)
		  , RelativeTransform(FTransform::Identity)
	{
	}
};

UINTERFACE(MinimalAPI, Blueprintable)
class UEquipmentMeshHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Equipment Mesh Handler Interface
 * Bu interface karakterin ekipman mesh'lerini yönetir
 */
class INVENTORYSYSTEM_API IEquipmentMeshHandler
{
	GENERATED_BODY()

public:
	/** Ekipman mesh'ini ekle/attach et */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Mesh")
	bool AttachEquipmentMesh(EEquipSlot EquipSlot, const FEquipmentMeshInfo& MeshInfo);

	/** Ekipman mesh'ini kaldır/detach et */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Mesh")
	bool DetachEquipmentMesh(EEquipSlot EquipSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Animation")
	void PlayEquipmentAnimation(UAnimationAsset* AnimationAsset, bool bFirstPerson, bool bThirdPerson);

	/** Belirli bir slot için mesh component'i al */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Mesh")
	UMeshComponent* GetEquipmentMeshComponent(EEquipSlot EquipSlot);

	/** Karakter mesh'inin belirli bölümlerini gizle/göster */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Mesh")
	void SetCharacterMeshSectionVisibility(const TArray<FName>& SectionNames, bool bVisible);

	/** Tüm ekipman mesh'lerini temizle */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Mesh")
	void ClearAllEquipmentMeshes();

	/** Ana karakter mesh'ini al */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment|Mesh")
	USkeletalMeshComponent* GetCharacterMesh();

	// Default implementations
	virtual bool AttachEquipmentMesh_Implementation(EEquipSlot EquipSlot, const FEquipmentMeshInfo& MeshInfo)
	{
		return false;
	}

	virtual bool DetachEquipmentMesh_Implementation(EEquipSlot EquipSlot) { return false; }
	virtual UMeshComponent* GetEquipmentMeshComponent_Implementation(EEquipSlot EquipSlot) { return nullptr; }

	virtual void SetCharacterMeshSectionVisibility_Implementation(const TArray<FName>& SectionNames, bool bVisible)
	{
	}

	virtual void ClearAllEquipmentMeshes_Implementation()
	{
	}

	virtual USkeletalMeshComponent* GetCharacterMesh_Implementation() { return nullptr; }
};

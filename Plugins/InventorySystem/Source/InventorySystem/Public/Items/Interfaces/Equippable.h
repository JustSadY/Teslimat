// IEquippable.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Equippable.generated.h"

UINTERFACE(Blueprintable)
class INVENTORYSYSTEM_API UEquippable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Kuşanılabilir interface
 */
class INVENTORYSYSTEM_API IEquippable
{
	GENERATED_BODY()

public:
	/** Nesne kuşanıldığında çağrılır */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void OnEquip(AActor* OwningPawn);

	virtual void OnEquip_Implementation(AActor* OwningPawn)
	{
	}

	/** Nesne çıkarıldığında çağrılır */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Equipment")
	void OnUnequip(AActor* OwningPawn);

	virtual void OnUnequip_Implementation(AActor* OwningPawn)
	{
	}
};

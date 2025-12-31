#pragma once

#include "CoreMinimal.h"
#include "Actor/InteractionActor.h"
#include "InventorySystem/Public/Items/WearableItems/WearableItem.h"
#include "APickupActor.generated.h"

UCLASS()
class TESLIMAT_API APickupActor : public AInteractionActor
{
	GENERATED_BODY()

public:
	APickupActor();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	TSubclassOf<UItemBase> ItemClass;

	virtual void Interact_Implementation(AActor* Interactor) override;
};

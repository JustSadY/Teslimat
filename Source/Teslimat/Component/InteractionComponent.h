#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AInteractionActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESLIMAT_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();
	void PrimaryInteract();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	void InteractRay();

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<AInteractionActor> CurrentViewedActor;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<AInteractionActor> InventorySlot;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float TraceDistance = 500.f;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bShowDebug = true;
};

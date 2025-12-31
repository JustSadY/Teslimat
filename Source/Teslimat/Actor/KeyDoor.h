#pragma once

#include "CoreMinimal.h"
#include "InteractionActor.h"
#include "KeyDoor.generated.h"

UCLASS()
class TESLIMAT_API AKeyDoor : public AInteractionActor
{
	GENERATED_BODY()

public:
	AKeyDoor();
	void OpenDoor();
	virtual void BeginPlay() override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	FName KeyName;
	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float WorldTargetYaw = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float InterpSpeed = 3.0f;

	FRotator InitialRotation;
	bool bIsOpen = false;
	bool bIsMoving = false;
};

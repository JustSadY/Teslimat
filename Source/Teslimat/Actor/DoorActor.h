#pragma once

#include "CoreMinimal.h"
#include "InteractionActor.h"
#include "DoorActor.generated.h"

UCLASS()
class TESLIMAT_API ADoorActor : public AInteractionActor
{
	GENERATED_BODY()

public:
	ADoorActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override; // Başlangıç rotasyonunu almak için
	virtual void Interact_Implementation(AActor* Interactor) override;

	void OpenDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

private:
	UPROPERTY(EditAnywhere, Category = "Door Settings")
	class USoundWave* ClosedDoorSoundWave;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	class USoundWave* OpenedDoorSoundWave;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float WorldTargetYaw = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
	float InterpSpeed = 3.0f;

	FRotator InitialRotation;
	bool bIsOpen = false;
	bool bIsMoving = false;
};

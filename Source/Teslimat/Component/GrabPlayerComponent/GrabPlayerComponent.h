#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabPlayerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESLIMAT_API UGrabPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabPlayerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// Skill'i başlatan fonksiyon
	UFUNCTION(BlueprintCallable, Category = "Combat Skill")
	void StartPull();

	// Skill'i durduran ve kontrolleri geri veren fonksiyon
	void StopPull();

private:
	UPROPERTY(EditAnywhere, Category = "Skill Settings")
	float PullSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Skill Settings")
	float RotationSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Skill Settings")
	float StoppingDistance = 150.f;

	bool bIsPulling = false;

	UPROPERTY()
	class ACharacter* TargetPlayer;

	UPROPERTY()
	class APlayerController* PlayerController;
};

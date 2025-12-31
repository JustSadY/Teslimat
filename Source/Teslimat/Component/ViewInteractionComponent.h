#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ViewInteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESLIMAT_API UViewInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UViewInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// Ayarlar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ViewAngleThreshold = 0.85f;

private:
	void ScanActorsInView();

	UPROPERTY()
	class UCameraComponent* PlayerCamera;

	UPROPERTY()
	TSet<AActor*> LastFrameVisibleActors;
};

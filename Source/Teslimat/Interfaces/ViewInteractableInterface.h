#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ViewInteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UViewInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class TESLIMAT_API IViewInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnEnteredCameraView();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnExitedCameraView();
};

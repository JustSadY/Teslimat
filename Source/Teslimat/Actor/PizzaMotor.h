// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionActor.h"
#include "PizzaMotor.generated.h"

UCLASS()
class TESLIMAT_API APizzaMotor : public AInteractionActor
{
	GENERATED_BODY()

	virtual void Interact_Implementation(AActor* Interactor) override;
};

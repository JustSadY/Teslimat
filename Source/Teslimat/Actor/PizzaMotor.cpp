// Fill out your copyright notice in the Description page of Project Settings.


#include "PizzaMotor.h"

#include "TeslimatCharacter.h"
#include "QuestionSystem/QuestionSubsystem.h"

class UQuestionSubsystem;

void APizzaMotor::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);

	if (!Interactor) return;

	if (ATeslimatCharacter* Char = Cast<ATeslimatCharacter>(Interactor))
	{
		Char->SetVisiblePizza(true);
	}
	if (const UGameInstance* GI = GetGameInstance())
	{
		if (UQuestionSubsystem* QuestSys = GI->GetSubsystem<UQuestionSubsystem>())
		{
			QuestSys->NextQuest();
		}
	}
}

#include "QuestionSubsystem.h"

void UQuestionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	QuestList.Add(NewObject<UTakingPizza>(this));
	QuestList.Add(NewObject<UGotoHome>(this));
	QuestList.Add(NewObject<UGotoTable>(this));
	QuestList.Add(NewObject<UFindAWayToEscape>(this));
	QuestList.Add(NewObject<UTakingKey>(this));
	QuestList.Add(NewObject<ULearnRedKey>(this));
	QuestList.Add(NewObject<ULearnBlueKey>(this));
	QuestList.Add(NewObject<UGotoUpStairs>(this));
	QuestList.Add(NewObject<UTakinGreenKey>(this));
	QuestList.Add(NewObject<UFakeHome>(this));
	NextQuest();
}

void UQuestionSubsystem::NextQuest()
{
	if (CurrentQuest)
	{
		CurrentQuest->Reset();
	}

	if (QuestList.Num() == 0) return;

	int32 NextIndex = CurrentQuestIndex + 1;

	if (QuestList.IsValidIndex(NextIndex))
	{
		CurrentQuestIndex = NextIndex;
		CurrentQuest = QuestList[CurrentQuestIndex];
		CurrentQuest->Execute();
		UE_LOG(LogTemp, Warning, TEXT("Yeni Gorev: %s"), *CurrentQuest->QuestDescription);
	}
	else
	{
		CurrentQuest = nullptr;
		UE_LOG(LogTemp, Error, TEXT("Tum gorevler bitti!"));
	}
}

FString UQuestionSubsystem::GetCurrentQuestDescription() const
{
	return CurrentQuest ? CurrentQuest->QuestDescription : TEXT("No current quest");
}

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestionSubsystem.generated.h"

UCLASS(BlueprintType)
class TESLIMAT_API UQuest : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString QuestDescription;

	virtual void Execute()
	{
	}

	virtual void Reset()
	{
	}
};

UCLASS(BlueprintType)
class TESLIMAT_API UTakingPizza : public UQuest
{
	GENERATED_BODY()

public:
	UTakingPizza() { QuestDescription = TEXT("Motordan Pizzayı al"); }
};

UCLASS(BlueprintType)
class TESLIMAT_API UGotoHome : public UQuest
{
	GENERATED_BODY()

public:
	UGotoHome() { QuestDescription = TEXT("Pizzayı Eve Götür"); }
};

UCLASS(BlueprintType)
class TESLIMAT_API UGotoTable : public UQuest
{
	GENERATED_BODY()

public:
	UGotoTable() { QuestDescription = TEXT("Pizzayı Masaya Bırak"); }
};

UCLASS(BlueprintType)
class TESLIMAT_API UFindAWayToEscape
	: public UQuest
{
	GENERATED_BODY()

public:
	UFindAWayToEscape() { QuestDescription = TEXT("Kaçmanın yolunu bul"); }
};


UCLASS(BlueprintType)
class TESLIMAT_API UTakingKey
	: public UQuest
{
	GENERATED_BODY()

public:
	UTakingKey() { QuestDescription = TEXT("Anahtarı al"); }
};


UCLASS(BlueprintType)
class TESLIMAT_API ULearnRedKey
	: public UQuest
{
	GENERATED_BODY()

public:
	ULearnRedKey() { QuestDescription = TEXT("Anahtarın nereye açtığını öğren"); }
};

UCLASS(BlueprintType)
class TESLIMAT_API ULearnBlueKey
	: public UQuest
{
	GENERATED_BODY()

public:
	ULearnBlueKey() { QuestDescription = TEXT("Üst katın anahtarını al"); }
};

UCLASS(BlueprintType)
class TESLIMAT_API UGotoUpStairs
	: public UQuest
{
	GENERATED_BODY()

public:
	UGotoUpStairs() { QuestDescription = TEXT("Üst kattaki yatak odasını aç"); }
};

UCLASS(BlueprintType)
class TESLIMAT_API UTakinGreenKey
	: public UQuest
{
	GENERATED_BODY()

public:
	UTakinGreenKey() { QuestDescription = TEXT("Dışarının anahtarını al"); }
};


UCLASS(BlueprintType)
class TESLIMAT_API UFakeHome
	: public UQuest
{
	GENERATED_BODY()

public:
	UFakeHome() { QuestDescription = TEXT("Dışarıya kaç"); }
};


UCLASS()
class TESLIMAT_API UQuestionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void NextQuest();

	UFUNCTION(BlueprintPure, Category = "Quests")
	FString GetCurrentQuestDescription() const;

	UFUNCTION(BlueprintPure, Category = "Quest System")
	int32 GetCurrentQuestIndex() const { return CurrentQuestIndex; }

protected:
	UPROPERTY()
	TArray<UQuest*> QuestList;

	UPROPERTY()
	UQuest* CurrentQuest;

	int32 CurrentQuestIndex = -1;
};

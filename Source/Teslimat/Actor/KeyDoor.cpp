#include "KeyDoor.h"
#include "InventoryComponent.h"

AKeyDoor::AKeyDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AKeyDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialRotation = GetActorRotation();
}

void AKeyDoor::Interact_Implementation(AActor* Interactor)
{
	if (!Interactor || bIsOpen) return;

	UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>();

	if (Inventory)
	{
		UE_LOG(LogTemp, Log, TEXT("Aranan Anahtar: %s"), *KeyName.ToString());

		if (UItemBase* ItemBase = Inventory->GetItemByName(KeyName.ToString()))
		{
			UE_LOG(LogTemp, Log, TEXT("Anahtar bulundu, kapı açılıyor!"));
			OpenDoor();
			Inventory->RemoveItem(ItemBase);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Anahtar eksik: %s"), *KeyName.ToString());
		}
	}
}

void AKeyDoor::OpenDoor()
{
	bIsOpen = true;
	bIsMoving = true;
	SetActorTickEnabled(true);
}

void AKeyDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		FRotator CurrentRotation = GetActorRotation();

		FRotator TargetRotation = InitialRotation;
		TargetRotation.Yaw += WorldTargetYaw;

		if (!CurrentRotation.Equals(TargetRotation, 0.1f))
		{
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
			SetActorRotation(NewRotation);
		}
		else
		{
			SetActorRotation(TargetRotation);
			bIsMoving = false;
			SetActorTickEnabled(false);
			UE_LOG(LogTemp, Log, TEXT("Kapı tamamen açıldı."));
		}
	}
}

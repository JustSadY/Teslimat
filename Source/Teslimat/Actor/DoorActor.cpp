#include "DoorActor.h"

#include "Kismet/GameplayStatics.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = true;
	// Başlangıçta Tick kapalı olabilir, hareket başlayınca açarız (Performans için)
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
	// Kapının editördeki ilk yerleşimini "Kapalı" hali olarak kaydediyoruz
	InitialRotation = GetActorRotation();
}

void ADoorActor::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);
	OpenDoor();
}

void ADoorActor::OpenDoor()
{
	bIsOpen = true;
	bIsMoving = true;
	SetActorTickEnabled(true);

	if (OpenedDoorSoundWave)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			OpenedDoorSoundWave,
			GetActorLocation()
		);
	}
}

void ADoorActor::CloseDoor()
{
	bIsOpen = false;
	bIsMoving = true;
	SetActorTickEnabled(true);

	if (ClosedDoorSoundWave)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ClosedDoorSoundWave,
			GetActorLocation()
		);
	}
}

void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		FRotator CurrentRotation = GetActorRotation();

		FRotator TargetRotation = bIsOpen ? FRotator(0.f, WorldTargetYaw, 0.f) : InitialRotation;

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
		}
	}
}

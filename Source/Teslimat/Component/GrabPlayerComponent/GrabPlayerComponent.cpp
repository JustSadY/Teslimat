#include "GrabPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UGrabPlayerComponent::UGrabPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrabPlayerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGrabPlayerComponent::StartPull()
{
	// 1. Dünyadaki 0 indexli oyuncuyu al
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	if (TargetPlayer)
	{
		PlayerController = Cast<APlayerController>(TargetPlayer->GetController());
		
		if (PlayerController)
		{
			// 2. Kontrolleri Kitle
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->SetIgnoreLookInput(true);

			// 3. Mevcut ivmeyi (hızı) sıfırla ki oyuncu kaymasın
			TargetPlayer->GetCharacterMovement()->StopMovementImmediately();

			bIsPulling = true;
		}
	}
}

void UGrabPlayerComponent::StopPull()
{
	if (PlayerController)
	{
		// Kontrolleri geri ver
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
	}

	bIsPulling = false;
	TargetPlayer = nullptr;
}

void UGrabPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPulling && TargetPlayer)
	{
		FVector CurrentLoc = TargetPlayer->GetActorLocation();
		FVector TargetLoc = GetOwner()->GetActorLocation();

		float Distance = FVector::Dist(CurrentLoc, TargetLoc);

		// Eğer oyuncu yeterince yakınsa çekmeyi bırak
		if (Distance <= StoppingDistance)
		{
			StopPull();
			return;
		}

		// --- HAREKET: Oyuncuyu düşmana doğru çek ---
		FVector NewLocation = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, PullSpeed / 100.f);
		TargetPlayer->SetActorLocation(NewLocation, true);

		// --- KAMERA: Oyuncunun bakış açısını düşmana zorla çevir ---
		FRotator CurrentRotation = PlayerController->GetControlRotation();
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLoc, TargetLoc);
		
		// Sadece Yaw ve Pitch değerlerini interp yapıyoruz (Roll genellikle sabit kalır)
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
		PlayerController->SetControlRotation(NewRotation);
	}
}
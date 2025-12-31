// Copyright Epic Games, Inc. All Rights Reserved.

#include "TeslimatCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Teslimat.h"
#include "GameFramework/CharacterMovementComponent.h"

void ATeslimatCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsSprinting && GetVelocity().Size() > 0)
	{
		CurrentStamina -= StaminaDrainRate * DeltaSeconds;

		if (CurrentStamina <= 0)
		{
			CurrentStamina = 0;
			EndStamina = true;
			StopSprint();
		}
	}
	else
	{
		if (CurrentStamina < MaxStamina)
		{
			CurrentStamina += StaminaRegenRate * DeltaSeconds;

			if (CurrentStamina >= MaxStamina)
			{
				CurrentStamina = MaxStamina;
				EndStamina = false;
				UE_LOG(LogTemp, Log, TEXT("Stamina Full! Kosma aktif."));
			}
		}
	}
}

ATeslimatCharacter::ATeslimatCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 96.0f);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh(), FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f),
	                                                           FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true; // Mouse ile bakış için kritik
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	PizzaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PizzaMesh"));
	PizzaMesh->SetupAttachment(FirstPersonCameraComponent);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));
}

void ATeslimatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATeslimatCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATeslimatCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATeslimatCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATeslimatCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this,
		                                   &ATeslimatCharacter::LookInput);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
		                                   &ATeslimatCharacter::OnInteractPressed);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ATeslimatCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		                                   &ATeslimatCharacter::StopSprint);
	}
	else
	{
		UE_LOG(LogTeslimat, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}


void ATeslimatCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATeslimatCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void ATeslimatCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATeslimatCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ATeslimatCharacter::DoJumpStart()
{
	Jump();
}

void ATeslimatCharacter::DoJumpEnd()
{
	StopJumping();
}

void ATeslimatCharacter::OnInteractPressed()
{
	if (InteractionComponent)
	{
		InteractionComponent->PrimaryInteract();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionComponent is null!"), *GetNameSafe(this))
	}
}

void ATeslimatCharacter::Sprint()
{
	if (!EndStamina)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	}
}

void ATeslimatCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void ATeslimatCharacter::SetVisiblePizza(bool IsVisible)
{
	PizzaMesh->SetVisibility(IsVisible);
}

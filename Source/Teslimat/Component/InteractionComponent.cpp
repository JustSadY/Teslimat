#include "InteractionComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Actor/InteractionActor.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	InteractRay();
}

void UInteractionComponent::InteractRay()
{
	if (!PlayerController) return;

	FVector Location;
	FRotator Rotation;
	PlayerController->GetPlayerViewPoint(Location, Rotation);

	FVector Start = Location;
	FVector End = Start + (Rotation.Vector() * TraceDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	ObjectParams.AddObjectTypesToQuery(ECC_Vehicle);
	ObjectParams.AddObjectTypesToQuery(ECC_Destructible);

	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectParams, Params);
	if (bHit)
	{
		AInteractionActor* HitInteractable = Cast<AInteractionActor>(Hit.GetActor());
		if (HitInteractable)
		{
			CurrentViewedActor = HitInteractable;
		}
		else { CurrentViewedActor = nullptr; }
	}
	else { CurrentViewedActor = nullptr; }
}

void UInteractionComponent::PrimaryInteract()
{
	if (CurrentViewedActor)
	{
		CurrentViewedActor->Interaction(GetOwner());
	}
}

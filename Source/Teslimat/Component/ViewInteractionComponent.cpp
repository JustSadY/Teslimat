#include "ViewInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Interfaces/ViewInteractableInterface.h"

UViewInteractionComponent::UViewInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UViewInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		PlayerCamera = GetOwner()->FindComponentByClass<UCameraComponent>();
	}
}

void UViewInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ScanActorsInView();
}

void UViewInteractionComponent::ScanActorsInView()
{
	if (!PlayerCamera) return;

	FVector CameraLoc = PlayerCamera->GetComponentLocation();
	FVector CameraForward = PlayerCamera->GetForwardVector();

	TSet<AActor*> CurrentlyVisibleActors;

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Scope = FCollisionShape::MakeSphere(InteractionDistance);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(Overlaps, CameraLoc, FQuat::Identity, ECC_Visibility, Scope,
	                                                      Params);

	if (bHasOverlaps)
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* PotentialActor = Overlap.GetActor();

			if (!PotentialActor || CurrentlyVisibleActors.Contains(PotentialActor)) continue;

			if (PotentialActor->Implements<UViewInteractableInterface>())
			{
				FVector ToActor = (PotentialActor->GetActorLocation() - CameraLoc).GetSafeNormal();
				float Dot = FVector::DotProduct(CameraForward, ToActor);

				if (Dot >= ViewAngleThreshold)
				{
					FHitResult Hit;
					FCollisionQueryParams TraceParams;
					TraceParams.AddIgnoredActor(GetOwner());

					if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLoc, PotentialActor->GetActorLocation(),
					                                         ECC_Visibility, TraceParams))
					{
						if (Hit.GetActor() == PotentialActor)
						{
							CurrentlyVisibleActors.Add(PotentialActor);
						}
					}
				}
			}
		}
	}

	for (AActor* Actor : CurrentlyVisibleActors)
	{
		if (!LastFrameVisibleActors.Contains(Actor))
		{
			IViewInteractableInterface::Execute_OnEnteredCameraView(Actor);
		}
	}

	for (AActor* Actor : LastFrameVisibleActors)
	{
		if (!IsValid(Actor) || !CurrentlyVisibleActors.Contains(Actor))
		{
			if (IsValid(Actor))
			{
				IViewInteractableInterface::Execute_OnExitedCameraView(Actor);
			}
		}
	}

	LastFrameVisibleActors = CurrentlyVisibleActors;
}

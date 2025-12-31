#include "APickupActor.h"

#include "InventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"

APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APickupActor::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	if (!Interactor || !ItemClass) return;
	UInventoryComponent* FoundComp = Interactor->FindComponentByClass<UInventoryComponent>();
	if (!FoundComp) return;
	FoundComp->AddItemByClass(ItemClass);
}

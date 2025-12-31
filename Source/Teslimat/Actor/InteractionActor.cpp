#include "InteractionActor.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "TeslimatCharacter.h"
#include "Widget/InteractionWidget.h"

AInteractionActor::AInteractionActor()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(StaticMeshComponent);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(StaticMeshComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	InteractionWidget->SetVisibility(false);

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(
		TEXT("/Game/Widget/W_Interaction.W_Interaction_C"));

	if (WidgetClassFinder.Succeeded())
	{
		InteractionWidget->SetWidgetClass(WidgetClassFinder.Class);
	}
	InteractionName = TEXT("Object");
}

void AInteractionActor::Interaction(AActor* Interactor)
{
	if (CanInteract(Interactor)) Interact(Interactor);
}


void AInteractionActor::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteractionActor::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AInteractionActor::OnOverlapEnd);
	if (InteractionWidget)
	{
		if (UInteractionWidget* b = Cast<UInteractionWidget>(InteractionWidget->GetWidget()))
		{
			b->InteractionActor = this;
		}
	}
}

void AInteractionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	ATeslimatCharacter* Character = Cast<ATeslimatCharacter>(OtherActor);

	if (Character && CanInteract(Character))
	{
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(true);
		}
	}
}

void AInteractionActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ATeslimatCharacter>(OtherActor))
	{
		if (InteractionWidget) InteractionWidget->SetVisibility(false);
	}
}

void AInteractionActor::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Etkileşim: %s"), *InteractionName);
}

bool AInteractionActor::CanInteract_Implementation(AActor* Interactor)
{
	return true;
}

#include "KMBreakableActor.h"
#include "Field/FieldSystemComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

AKMBreakableActor::AKMBreakableActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());

	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollectionComponent->SetupAttachment(GetRootComponent());
	
	FieldSystemComponent = CreateDefaultSubobject<UFieldSystemComponent>(TEXT("FieldSystem"));
	FieldSystemComponent->SetupAttachment(GetRootComponent());
}

void AKMBreakableActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKMBreakableActor::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void AKMBreakableActor::OnImpact(const TSharedPtr<FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	Super::OnImpact(skillEffectInstance, hitClosestPoint, hitTag);
}

void AKMBreakableActor::OnDeath()
{
	Super::OnDeath();
}
#include "KMBreakableActor.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "Field/FieldSystemComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

AKMBreakableActor::AKMBreakableActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	AttachedComponent = CreateDefaultSubobject<UKMAttachedBlendingComponent>(TEXT("AttachedComponent"));
	AttachedComponent->SetupAttachment(GetRootComponent());
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(AttachedComponent);

	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollectionComponent->SetupAttachment(AttachedComponent);
	
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

FBoxSphereBounds AKMBreakableActor::GetMasterBounds() const
{
	return GeometryCollectionComponent->Bounds;
}

UKMAttachedBlendingComponent* AKMBreakableActor::GetAttachedComponent() const
{
	return AttachedComponent;
}
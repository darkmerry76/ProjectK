#include "KMProjectileHomingActor.h"
#include "Component/KMProjectileMovementComponent.h"
#include "Core/KMDefine.h"

AKMProjectileHomingActor::AKMProjectileHomingActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	ProjectileMovementComponent = CreateDefaultSubobject<UKMProjectileMovementComponent>(FName("ProjectileMovementComponent"));
}

void AKMProjectileHomingActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKMProjectileHomingActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

UKMProjectileMovementComponent* AKMProjectileHomingActor::GetProjectileMovementComponent() const
{
	return ProjectileMovementComponent;
}

AActor* AKMProjectileHomingActor::GetTarget() const
{
	return Target;	
}

void AKMProjectileHomingActor::SetTarget(AActor* newTarget)
{
	Target = newTarget;
}

void AKMProjectileHomingActor::OnTargetArrival_Implementation()
{
	Destroy();
}

void AKMProjectileHomingActor::SetCreatedIndex(int32 index)
{
	CreatedIndex = index;
}

void AKMProjectileHomingActor::AddArrivalData(TSharedPtr<FKMProjectileArrivalData> newArrivalData)
{
	ArrivalData.Emplace(newArrivalData);
}

int32 AKMProjectileHomingActor::NumArrivalData() const
{
	return ArrivalData.Num();
}

TSharedPtr<FKMProjectileArrivalData> AKMProjectileHomingActor::GetArrivalDataByIndex(int32 index) const
{
	return ArrivalData[index];
}
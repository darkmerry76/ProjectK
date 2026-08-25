#include "KMProjectileActor.h"

#include "Component/KMProjectileMovementComponent.h"
#include "Core/KMDefine.h"

AKMProjectileActor::AKMProjectileActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	ProjectileMovementComponent = CreateDefaultSubobject<UKMProjectileMovementComponent>(FName("ProjectileMovementComponent"));
}

void AKMProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKMProjectileActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

UKMProjectileMovementComponent* AKMProjectileActor::GetProjectileMovementComponent() const
{
	return ProjectileMovementComponent;
}

AActor* AKMProjectileActor::GetTarget() const
{
	return Target;	
}

void AKMProjectileActor::SetTarget(AActor* newTarget)
{
	Target = newTarget;
}

void AKMProjectileActor::OnTargetArrival_Implementation()
{
	Destroy();
}

void AKMProjectileActor::SetCreatedIndex(int32 index)
{
	CreatedIndex = index;
}

void AKMProjectileActor::AddArrivalData(TSharedPtr<FKMProjectileArrivalData> newArrivalData)
{
	ArrivalData.Emplace(newArrivalData);
}

int32 AKMProjectileActor::NumArrivalData() const
{
	return ArrivalData.Num();
}

TSharedPtr<FKMProjectileArrivalData> AKMProjectileActor::GetArrivalDataByIndex(int32 index) const
{
	return ArrivalData[index];
}

FPrimaryAssetId AKMProjectileActor::GetPrimaryAssetId() const
{
	return Super::GetPrimaryAssetId();
/*	UClass* assetClass = GetClass();
	if (!IsValid(assetClass) || !IsValid(assetClass->ClassGeneratedBy))
	{
		return Super::GetPrimaryAssetId();
	}
	
	return FPrimaryAssetId(TEXT("Projectile"), assetClass->ClassGeneratedBy->GetFName());*/
}
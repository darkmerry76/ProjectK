#include "KMSensorEnemy.h"

#include "GameObject/KMCharacterInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UKMSensorEnemy::Init()
{
	Super::Init();
	
	if (UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOuter()))
	{
		if (IsValid(ownerCharacterInstance->GetOwnerActor()))
		{
			ActorsToIgnore.Emplace(ownerCharacterInstance->GetOwnerActor());
		}
	}
}

void UKMSensorEnemy::DoSearch()
{
	TArray<AActor*> overlapActors;
	UKismetSystemLibrary::SphereOverlapActors(this, CenterTransform.GetLocation(), Radius, ObjectTypeQuery, ActorClassFilter, ActorsToIgnore, overlapActors);

	TArray<AActor*> targetActors;
	FVector forwardVector = CenterTransform.GetRotation().GetForwardVector();
	for (auto overlapActor : overlapActors)
	{
		FVector targetToDirection = overlapActor->GetActorLocation() - CenterTransform.GetLocation();
		FVector targetToNormal = targetToDirection.GetSafeNormal();
		
		float targetToDot = FVector::DotProduct(forwardVector, targetToNormal);
		float angleDeg = FMath::RadiansToDegrees(FMath::Acos(targetToDot));
		if (angleDeg <= ViewAngle)
		{
			targetActors.Emplace(overlapActor);
		}
	}
	targetActors.Sort([&](const AActor& A, const AActor& B)
	{
		float distA = (A.GetActorLocation() - CenterTransform.GetLocation()).Size();
		float distB = (B.GetActorLocation() - CenterTransform.GetLocation()).Size();
		
		return distA < distB;
	});

	ResultDelegate.ExecuteIfBound(targetActors);
}

void UKMSensorEnemy::SetRadius(float newRadius)
{
	Radius = newRadius;
}

float UKMSensorEnemy::GetRadius() const
{
	return Radius;
}
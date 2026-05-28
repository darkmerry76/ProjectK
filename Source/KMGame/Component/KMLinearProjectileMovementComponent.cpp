#include "KMLinearProjectileMovementComponent.h"
#include "GameFramework/Actor.h"

UKMLinearProjectileMovementComponent::UKMLinearProjectileMovementComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKMLinearProjectileMovementComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	ElapsedTime += deltaTime;

	float safeDuration = FMath::Max(Duration, 0.001f);

	float alpha = ElapsedTime / safeDuration;
	alpha = FMath::Clamp(alpha, 0.f, 1.f);

	float t = Curve ? Curve->GetFloatValue(alpha) : alpha;

	FVector relativeLocation = Direction * (t * Distance);
	SetRelativeLocation(relativeLocation);

	if (alpha >= 1.0f)
	{
		bFinished = true;

		AActor* ownerActor = GetOwner();
		ownerActor->Destroy();
	}
}
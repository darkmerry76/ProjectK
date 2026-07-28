#include "KMCurveWarpingComponent.h"
#include "Animation/KMAnimInstance.h"
#include "GameFramework/Character.h"

UKMCurveWarpingComponent::UKMCurveWarpingComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

float UKMCurveWarpingComponent::GetSyncMoveRunElipsedTime() const
{
	ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return 0.f;
	}

	USkeletalMeshComponent* skeletalMeshComponent = ownerCharacter->GetMesh();
	if (!IsValid(skeletalMeshComponent))
	{
		return 0.f;
	}
	UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(skeletalMeshComponent->GetAnimInstance());
	if (!IsValid(animInstance))
	{
		return 0.f;
	}
	return animInstance->GetMovementElapsedTime();
}

void UKMCurveWarpingComponent::SetCustomMovementAnimation(UAnimSequence* animSequence)
{
	Super::SetCustomMovementAnimation(animSequence);
}

void UKMCurveWarpingComponent::EnableCustomMovement()
{
	Super::EnableCustomMovement();
}

bool UKMCurveWarpingComponent::IsCustomRun() const
{
	return Super::IsCustomRun();
}
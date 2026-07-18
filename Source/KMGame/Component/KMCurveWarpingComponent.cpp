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
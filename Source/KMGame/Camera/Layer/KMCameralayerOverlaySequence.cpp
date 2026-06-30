#include "KMCameralayerOverlaySequence.h"
#include "GameFramework/Character.h"

UKMCameralayerOverlaySequence::UKMCameralayerOverlaySequence(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCameralayerOverlaySequence::SetRelativeCameraData(const FEMCameraOutput& newRelativeCameraData)
{
	RelativeCameraData = newRelativeCameraData;
}

void UKMCameralayerOverlaySequence::Evaluate(float deltaTime, FEMCameraOutput& output)
{
	if (Alpha == 0.f)
	{
		return;
	}
	
	AActor* targetActor = GetTargetActor();
	FTransform targetTransform = FTransform::Identity;
	if (ACharacter* character = Cast<ACharacter>(targetActor))
	{
		targetTransform = character->GetMesh()->GetComponentTransform();
		targetTransform.SetRotation(targetTransform.GetRotation() * FRotator(0.f, 90.f, 0.f).Quaternion());
	}
	else
	{
		targetTransform = targetActor->GetActorTransform();
	}


	FTransform newTransform;
	newTransform.SetLocation(RelativeCameraData.Location);
	newTransform.SetRotation(RelativeCameraData.Rotation.Quaternion());

	FTransform finalTransform = newTransform * targetTransform;

	output.Location = finalTransform.GetLocation();
	output.Rotation = finalTransform.GetRotation().Rotator();
	
	output.FOV = RelativeCameraData.FOV;
}
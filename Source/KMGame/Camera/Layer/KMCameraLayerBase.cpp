#include "KMCameralayerBase.h"
#include "Camera/KMCameraActorBase.h"

UKMCameralayerBase::UKMCameralayerBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCameralayerBase::SetAlpha(float newAlpha)
{
	Alpha = newAlpha;
}

float UKMCameralayerBase::GetAlpha() const
{
	return Alpha;
}

void UKMCameralayerBase::Initalize()
{
}
	
AActor* UKMCameralayerBase::GetTargetActor() const
{
	AKMCameraActorBase* cameraActor = Cast<AKMCameraActorBase>(GetCamaeraActor());
	if (!IsValid(cameraActor))
	{
		return nullptr;
	}
	return cameraActor->GetTargetActor();
}

void UKMCameralayerBase::SetCameraActor(AActor* newOwnerActor)
{
	CamaeraActor = newOwnerActor;
}

AActor* UKMCameralayerBase::GetCamaeraActor() const
{
	if (!CamaeraActor.IsValid())
	{
		return nullptr;
	}
	return CamaeraActor.Pin().Get();
}

USceneComponent* UKMCameralayerBase::GetCameraOffsetComponent() const
{
	AKMCameraActorBase* cameraActor = Cast<AKMCameraActorBase>(GetCamaeraActor());
	if (!IsValid(cameraActor))
	{
		return nullptr;
	}
	return cameraActor->GetCameraOffset();
}

void UKMCameralayerBase::Evaluate(float deltaTime, FEMCameraOutput& output)
{
}
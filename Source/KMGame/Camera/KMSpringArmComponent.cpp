#include "KMSpringArmComponent.h"
#include "KMCameraActorBase.h"
#include "Camera/CameraComponent.h"

const FName UKMSpringArmComponent::SocketName(TEXT("SpringEndpoint"));

UKMSpringArmComponent::UKMSpringArmComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	RelativeSocketRotation = FQuat::Identity;
}

void UKMSpringArmComponent::OnRegister()
{
	Super::OnRegister();
}

FTransform UKMSpringArmComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	FTransform RelativeTransform(RelativeSocketRotation, RelativeSocketLocation);

	switch(TransformSpace)
	{
	case RTS_World:
		{
			return RelativeTransform * GetComponentTransform();
		}
	case RTS_Actor:
		{
			if( const AActor* Actor = GetOwner() )
			{
				FTransform SocketTransform = RelativeTransform * GetComponentTransform();
				return SocketTransform.GetRelativeTransform(Actor->GetTransform());
			}
			break;
		}
	case RTS_Component:
		{
			return RelativeTransform;
		}
	default:break;
	}
	return RelativeTransform;
}

bool UKMSpringArmComponent::HasAnySockets() const
{
	return true;
}

void UKMSpringArmComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const
{
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

void UKMSpringArmComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	FEMCameraOutput finalCameraOut;
	if (AKMCameraActorBase* cameraActor = Cast<AKMCameraActorBase>(GetOwner()))
	{
		for (auto cameraLayer : cameraActor->CameraLayers)
		{
			FEMCameraOutput output;
			cameraLayer.Value->Evaluate(deltaTime, output);

			finalCameraOut.Location = FMath::Lerp(finalCameraOut.Location, output.Location, cameraLayer.Value->GetAlpha());
			finalCameraOut.Rotation = FQuat::Slerp(finalCameraOut.Rotation.Quaternion(), output.Rotation.Quaternion(), cameraLayer.Value->GetAlpha()).Rotator();
			finalCameraOut.FOV = FMath::Lerp(finalCameraOut.FOV, output.FOV, cameraLayer.Value->GetAlpha());
		}
	}

	FTransform worldCamTM(finalCameraOut.Rotation, finalCameraOut.Location);
	FTransform relCamTM = worldCamTM.GetRelativeTransform(GetComponentTransform());

	RelativeSocketLocation = relCamTM.GetLocation();
	RelativeSocketRotation = relCamTM.GetRotation();

	if (IsValid(GetOwner()))
	{
		if (UCameraComponent* cameraComponent = GetOwner()->GetComponentByClass<UCameraComponent>())
		{
			cameraComponent->SetFieldOfView(finalCameraOut.FOV);
		}
	}
	
	UpdateChildTransforms();
}
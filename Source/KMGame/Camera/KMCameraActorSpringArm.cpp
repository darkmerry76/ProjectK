#include "KMCameraActorSpringArm.h"
#include "KMSpringArmComponent.h"
#include "Camera/CameraComponent.h"

AKMCameraActorSpringArm::AKMCameraActorSpringArm(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SpringArm = CreateDefaultSubobject<UKMSpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

USceneComponent* AKMCameraActorSpringArm::GetCameraOffset() const
{
	return SpringArm;
}

void AKMCameraActorSpringArm::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (TargetActor.IsValid())
	{
		SetActorLocation(TargetActor->GetActorLocation());
	}
}
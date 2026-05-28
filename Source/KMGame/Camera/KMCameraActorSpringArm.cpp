#include "KMCameraActorSpringArm.h"
#include "KMSpringArmComponent.h"
#include "Camera/CameraComponent.h"

AKMCameraActorSpringArm::AKMCameraActorSpringArm(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Scene");

	SpringArm = CreateDefaultSubobject<UKMSpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AKMCameraActorSpringArm::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (TargetActor.IsValid())
	{
		SetActorLocation(TargetActor->GetActorLocation());
	}
}
#include "KMCameraActor.h"

#include "Camera/CameraComponent.h"
#include "Components/Viewport.h"
#include "Core/KMWorldSettings.h"
#include "Engine/GameViewportClient.h"
#include "System/KMGameObjectSubsystem.h"

const FIntPoint AKMCameraActor::defaultViewportSize(1440, 3160);

AKMCameraActor::AKMCameraActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void AKMCameraActor::BeginPlay()
{
	Super::BeginPlay();

	SetViewportOnAspectRatio(GetWorld()->GetGameViewport()->Viewport);
}

void AKMCameraActor::SetViewportOnAspectRatio(FViewport* viewport)
{
	if (!viewport)
	{
		return;
	}
	const FIntPoint viewportSize = viewport->GetSizeXY();
	if (viewportSize.X <= 0 || viewportSize.Y <= 0)
	{
		return;
	}

	UCameraComponent* cameraComponent = GetCameraComponent();
	if (!IsValid(cameraComponent))
	{
		return;
	}

	const float defaultAspectRatio = static_cast<float>(defaultViewportSize.X) / static_cast<float>(defaultViewportSize.Y);

	float aspectRatio = static_cast<float>(viewportSize.X) / static_cast<float>(viewportSize.Y);

	if (aspectRatio > defaultAspectRatio)
	{
		aspectRatio = defaultAspectRatio;
	}

	cameraComponent->SetOrthoWidth(1440.f);
	cameraComponent->SetConstraintAspectRatio(true);
	cameraComponent->SetAspectRatio(aspectRatio);

	OnPostViewportOnAspectRatio();
}

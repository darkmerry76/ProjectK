#include "KMPlacementPreviewActor.h"

AKMPlacementPreviewActor::AKMPlacementPreviewActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);
}

USceneComponent* AKMPlacementPreviewActor::GetRootScene() const
{
	return RootScene;
}
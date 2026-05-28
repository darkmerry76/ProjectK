#include "KMEditorDrawDebugComponent.h"

UKMEditorDrawDebugComponent::UKMEditorDrawDebugComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
}

void UKMEditorDrawDebugComponent::SetDebugLocation(const FVector& newDebugLocation)
{
	DebugLocation = newDebugLocation;
}
void UKMEditorDrawDebugComponent::SetDebugScale(const FVector& newDebugScale)
{
	DebugScale = newDebugScale;
}

void UKMEditorDrawDebugComponent::SetDebugRotation(const FQuat& newDebugRotation)
{
	DebugRotation = newDebugRotation;
}

void UKMEditorDrawDebugComponent::SetDebugColor(const FColor& newDebugColor)
{
	DebugColor = newDebugColor;
}

void UKMEditorDrawDebugComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawDebugBox(GetWorld(), DebugLocation, DebugScale, DebugRotation, FColor::Red);
}
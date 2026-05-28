#pragma once

#include "CoreMinimal.h"
#include "KMEditorDrawDebugComponent.generated.h"

UCLASS()
class KMGAME_API UKMEditorDrawDebugComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

protected:
	FVector DebugLocation = FVector::ZeroVector;
	FVector DebugScale = FVector(1.0f, 1.0f, 1.0f);
	FQuat DebugRotation = FQuat::Identity;
	FColor DebugColor = FColor::White;
	
public:
	void SetDebugLocation(const FVector& newDebugLocation);
	void SetDebugScale(const FVector& newDebugScale);
	void SetDebugRotation(const FQuat& newDebugRotation);
	void SetDebugColor(const FColor& newDebugColor);

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};
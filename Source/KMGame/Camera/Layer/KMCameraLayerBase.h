#pragma once

#include "CoreMinimal.h"
#include "Sequencer/EMCameraCacheManager.h"
#include "KMCameraLayerBase.generated.h"

UENUM(BlueprintType)
enum class EKMCameralayerType : uint8
{
	Gameplay,
	OverlaySequence,
	FullSequencer,
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, Abstract)
class KMGAME_API UKMCameralayerBase : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	void SetAlpha(float newAlpha);
	float GetAlpha() const;
	
	virtual void Evaluate(float deltaTime, FEMCameraOutput& output);
	
protected:
	float Alpha = 0.f;
};
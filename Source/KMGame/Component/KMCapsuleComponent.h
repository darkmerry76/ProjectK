#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "KMCapsuleComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMCapsuleComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMCapsuleComponent : public UCapsuleComponent
{
	GENERATED_UCLASS_BODY()

protected:
	float OriginCapsuleRadius = 20.f;
	float OriginCapsuleHalfHeight = 40.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	
public:
	void RevertOrigin();
};
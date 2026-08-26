#pragma once

#include "CoreMinimal.h"
#include "KMPawnInterface.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class KMGAME_API UKMPawnInterface : public UInterface
{
	GENERATED_BODY()
};

class KMGAME_API IKMPawnInterface
{
	GENERATED_BODY()

public:
	virtual class UKMGameObjectInstance* GetGameObjectInstance() const = 0;
	virtual void PossessedByGameObjectInstance(class UKMGameObjectInstance* newGameObjectInstance) = 0;

	virtual class UKMCurveWarpingComponent* GetCurveWarping() const = 0;
	virtual class UKMMartialArtsComponent* GetMartialArtsComponent() const = 0;

	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag) = 0;
	virtual void OnDeath() = 0;
};
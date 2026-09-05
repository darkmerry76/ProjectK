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

	virtual class UPrimitiveComponent* GetMovementShapeComponent() const = 0;
	virtual class UKMCurveWarpingComponent* GetCurveWarpingComponent() const = 0;
	virtual class UKMMartialArtsComponent* GetMartialArtsComponent() const = 0;

	virtual class UMeshComponent* GetMasterMeshComponent() const = 0;
	virtual class UKMAttachedBlendingComponent* GetAttachedBlendingComponent() const = 0;

	virtual FTransform GetCarryOffsetTransform() const = 0;

	virtual void StartCrarry(class UKMGameObjectInstance* carriedGameObjectInstance) = 0;
	virtual void StartCrarried(class UKMGameObjectInstance* carryGameObjectInstance) = 0;

	virtual void ComplatePutdown(class UKMGameObjectInstance* putDownedGameObjectInstance) = 0;
	virtual void ComplatePutdowned(class UKMGameObjectInstance* putDownGameObjectInstance) = 0;

	virtual void PossessedByGameObjectInstance(class UKMGameObjectInstance* newGameObjectInstance) = 0;

	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag) = 0;
	virtual void OnDeath() = 0;
};
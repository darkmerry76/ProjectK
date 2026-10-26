#pragma once

#include "CoreMinimal.h"
#include "GameActor/Pawn/Character/KMPawnBase.h"
#include "KMInteractiveActorBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMInteractiveActorBase : public AKMPawnBase
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMWidgetComponent> WidgetComponent;

	ECollisionResponse PawnResponse = ECollisionResponse::ECR_Block;

public:
	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag) override;
	virtual void OnDeath() override;

	UFUNCTION(BlueprintPure)
	class UKMInteractiveInstance* GetInteractiveInstance() const;

	UFUNCTION(BlueprintPure)
	virtual FBoxSphereBounds GetMasterBounds() const;
	
	virtual FTransform GetApproachPullPoint(EKMApproachPullPointType approachPullPointType = EKMApproachPullPointType::Auto) const override;

protected:
	virtual float GetScaledRadius_Implementation() const override;
	virtual float GetScaledHalfHeight_Implementation() const override;

	virtual class UKMWidgetComponent* GetWidgetComponent() const override;
};
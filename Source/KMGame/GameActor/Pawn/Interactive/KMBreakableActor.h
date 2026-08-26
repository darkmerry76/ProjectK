#pragma once

#include "CoreMinimal.h"
#include "KMInteractiveActorBase.h"
#include "KMBreakableActor.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMBreakableActor : public AKMInteractiveActorBase
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UGeometryCollectionComponent> GeometryCollectionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UFieldSystemComponent> FieldSystemComponent;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag) override;
	virtual void OnDeath() override;
};
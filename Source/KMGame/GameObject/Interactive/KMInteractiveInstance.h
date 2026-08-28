#pragma once

#include "CoreMinimal.h"
#include "GameObject/KMGameObjectInstance.h"
#include "KMInteractiveInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMInteractiveInstance : public UKMGameObjectInstance
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<class AKMInteractiveActorBase> Interactive;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay() override;
	virtual void SetDirectionVisual(float direction, bool bForceRotate = false, class USkeletalMeshComponent* otherSkeletalMeshComp = nullptr) override;
	virtual void SetDirection(float direction, bool bForceRotate = false) override;
};
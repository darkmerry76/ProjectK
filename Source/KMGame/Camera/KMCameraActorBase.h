#pragma once

#include "CoreMinimal.h"
#include "KMCameraActorBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class AKMCameraActorBase : public AActor 
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

protected:

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetTargetActor(class AActor* newTargetActor);
	
	UFUNCTION(BlueprintPure)
	class AActor* GetTargetActor() const;

	virtual void Tick(float deltaTime) override;
};
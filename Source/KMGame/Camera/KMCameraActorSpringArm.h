#pragma once

#include "CoreMinimal.h"
#include "KMCameraActorBase.h"
#include "KMCameraActorSpringArm.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AKMCameraActorSpringArm : public AKMCameraActorBase 
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> Scene;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMSpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> Camera;

protected:
	virtual void Tick(float deltaTime) override;
};
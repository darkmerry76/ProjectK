#pragma once

#include "CoreMinimal.h"
#include "Camera/Layer/KMCameralayerBase.h"
#include "KMCameraActorBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class AKMCameraActorBase : public AActor 
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TMap<EKMCameralayerType, TObjectPtr<class UKMCameralayerBase>> CameraLayers;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> Root;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetTargetActor(class AActor* newTargetActor);
	
	UFUNCTION(BlueprintPure)
	class AActor* GetTargetActor() const;
	
	UFUNCTION(BlueprintPure)
	virtual class USceneComponent* GetCameraOffset() const;

	UFUNCTION(BlueprintPure)
	class UKMCameralayerBase* GetCameraLayer(EKMCameralayerType cameraLayerType) const;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
};
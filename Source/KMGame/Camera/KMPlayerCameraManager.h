#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "KMPlayerCameraManager.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AKMPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, TSubclassOf<class AKMCameraActorBase>> Cameras;

	UPROPERTY()
	TObjectPtr<class AKMCameraActorBase> CurrentCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TMap<EKMCameralayerType, TObjectPtr<class UKMCameralayerBase>> AdvanceLayers;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void UpdateViewTarget(FTViewTarget& outVT, float deltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual class UCameraShakeBase* StartCameraShake(TSubclassOf<class UCameraShakeBase> shakeClass, float Scale = 1.f,
		ECameraShakePlaySpace playSpace = ECameraShakePlaySpace::CameraLocal, FRotator userPlaySpaceRot = FRotator::ZeroRotator) override;
	
public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="worldContextObject"))
	static AKMPlayerCameraManager* GetActiveCameraManager(const UObject* worldContextObject);
	
	virtual void SetViewTarget(class AActor* newViewTarget, FViewTargetTransitionParams transitionParams = FViewTargetTransitionParams()) override;

	UFUNCTION(BlueprintPure)
	class UKMCameralayerBase* GetCameraLayer(EKMCameralayerType cameraLayerType) const;

	UFUNCTION(BlueprintCallable)
	void SetCamera(FName cameraName, AActor* newTargetActor);
};
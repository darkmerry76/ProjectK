#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/Layer/KMCameralayerBase.h"
#include "KMPlayerCameraManager.generated.h"

USTRUCT()
struct KMGAME_API FKMCameraLayerPlayInstance
{
	GENERATED_USTRUCT_BODY()

	virtual ~FKMCameraLayerPlayInstance();
	virtual void BeginPlay() { };
	
	float Duration = 0.f;
	float ElipsedTime = 0.f;
	float Rate = 1.f;

	float BlendInTime = 0.f;
	float BlendOutTime = 0.f;

	bool bIsImmediate = false;

	UPROPERTY()
	TObjectPtr<class UKMCameralayerBase> Cameralayer;

	bool IsEnd() const;

	virtual void Update(float deltaTime);
};

USTRUCT()
struct KMGAME_API FKMCameraGameLayerPlayInstance : public FKMCameraLayerPlayInstance
{
	GENERATED_USTRUCT_BODY()
	
	virtual ~FKMCameraGameLayerPlayInstance() override;
	
	float TargetArmLength = 1200.f;
	float Remeber_TargetArmLength = 1200.f;
	
	float Speed = 20.f;

	virtual void BeginPlay() override;
	virtual void Update(float deltaTime) override;
};

USTRUCT()
struct KMGAME_API FKMCameraSequenceLayerPlayInstance : public FKMCameraLayerPlayInstance
{
	GENERATED_USTRUCT_BODY()

	virtual ~FKMCameraSequenceLayerPlayInstance() override;
	
	TSharedPtr<struct FEMCameraCacheInstance> CameraCacheInstance;

	virtual void Update(float deltaTime) override;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API AKMPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, TSubclassOf<class AKMCameraActorBase>> Cameras;

	UPROPERTY()
	TObjectPtr<class AKMCameraActorBase> CurrentCamera;

	TArray<TSharedPtr<FKMCameraLayerPlayInstance>> CameraLayerPlayInstances; 

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

	UFUNCTION(BlueprintCallable)
	void SetCamera(FName cameraName, AActor* newTargetActor);

	UFUNCTION(BlueprintPure)
	class AKMCameraActorBase* GetCurrentCamera() const;
	
	TSharedPtr<FKMCameraLayerPlayInstance> PlayCameraLayer(EKMCameralayerType layerType,
		class UCameraAnimationSequence* cameraSequence, float duration, float blendInTime = 0.1f, float blendOutTime = 0.1f, float rate = 1.f, bool bImmediate = false);

	void RemovePlayCameraLayer(const TSharedPtr<FKMCameraLayerPlayInstance>& cameraLayerPlayInstance);
};
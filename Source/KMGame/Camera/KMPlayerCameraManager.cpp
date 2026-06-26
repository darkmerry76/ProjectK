#include "KMPlayerCameraManager.h"
#include "KMCameraActorBase.h"
#include "Kismet/GameplayStatics.h"
#include "Layer/KMCameraLayerBase.h"

AKMPlayerCameraManager::AKMPlayerCameraManager(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

AKMPlayerCameraManager* AKMPlayerCameraManager::GetActiveCameraManager(const UObject* worldContextObject)
{
	return Cast<AKMPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(worldContextObject, 0));
}

void AKMPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
}

void AKMPlayerCameraManager::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

UKMCameralayerBase* AKMPlayerCameraManager::GetCameraLayer(EKMCameralayerType cameraLayerType) const
{
	const TObjectPtr<UKMCameralayerBase>* existCameraLayer = AdvanceLayers.Find(cameraLayerType);
	if (!existCameraLayer || !*existCameraLayer)
	{
		return nullptr;
	}
	return *existCameraLayer;
}

void AKMPlayerCameraManager::SetViewTarget(AActor* newViewTarget, FViewTargetTransitionParams transitionParams)
{
	Super::SetViewTarget(newViewTarget, transitionParams);
}

UCameraShakeBase* AKMPlayerCameraManager::StartCameraShake(TSubclassOf<UCameraShakeBase> shakeClass, float scale, ECameraShakePlaySpace playSpace, FRotator userPlaySpaceRot)
{
	Super::StopAllCameraShakes();
	
	return Super::StartCameraShake(shakeClass, scale, playSpace, userPlaySpaceRot);
}

void AKMPlayerCameraManager::SetCamera(FName cameraName, AActor* newTargetActor)
{
	TSubclassOf<AKMCameraActorBase>* resultCameraClass = Cameras.Find(cameraName);
	if (resultCameraClass)
	{
		if (IsValid(CurrentCamera))
		{
			CurrentCamera->Destroy();
		}

		if (IsValid(GetWorld()))
		{
			FActorSpawnParameters spawnParameters = FActorSpawnParameters();
			spawnParameters.bNoFail = true;
			
			CurrentCamera = GetWorld()->SpawnActor<AKMCameraActorBase>(*resultCameraClass, spawnParameters);
			if (IsValid(CurrentCamera))
			{
				CurrentCamera->SetTargetActor(newTargetActor);
				SetViewTarget(CurrentCamera);
			}
		}
	}
}

void AKMPlayerCameraManager::UpdateViewTarget(FTViewTarget& outVT, float deltaTime)
{
	Super::UpdateViewTarget(outVT, deltaTime);

/*	outVT.POV.FOV = 30.f;
	outVT.POV.Location = CurrentCamera->GetCameraLocation();
	outVT.POV.Rotation = CurrentCamera->GetCameraRotation();
	outVT.POV.ProjectionMode = CurrentCamera->GetCameraProjectionMode();
	outVT.POV.OrthoWidth = CurrentCamera->GetCameraOrthoWidth();*/
}

void AKMPlayerCameraManager::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsValid(CurrentCamera))
	{
		CurrentCamera->Tick(deltaTime);
	}
}
#include "KMPlayerCameraManager.h"
#include "KMCameraActorBase.h"

AKMPlayerCameraManager::AKMPlayerCameraManager(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKMPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
}

void AKMPlayerCameraManager::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
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

/*	outVT.POV.FOV = CurrentCamera->GetFieldOfView();
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
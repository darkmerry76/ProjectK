#include "KMPlayerCameraManager.h"
#include "EMMartialArtsModule.h"
#include "KMCameraActorBase.h"
#include "Core/KMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Layer/KMCameraLayerBase.h"
#include "Layer/KMCameralayerGameplay.h"

FKMCameraLayerPlayInstance::~FKMCameraLayerPlayInstance()
{
}

bool FKMCameraLayerPlayInstance::IsEnd() const
{
	if (ElipsedTime >= Duration)
	{
		return true;
	}
	return false;
}

void FKMCameraLayerPlayInstance::Update(float deltaTime)
{
	if (ElipsedTime >= Duration)
	{
		ElipsedTime = Duration;
	}
}

FKMCameraGameLayerPlayInstance::~FKMCameraGameLayerPlayInstance()
{
	UKMCameralayerGameplay* cameralayerGameplay = Cast<UKMCameralayerGameplay>(Cameralayer);
	if (IsValid(cameralayerGameplay))
	{
		cameralayerGameplay->TargetArmLength = Remeber_TargetArmLength;
	}
}

void FKMCameraGameLayerPlayInstance::BeginPlay()
{
	UKMCameralayerGameplay* cameralayerGameplay = Cast<UKMCameralayerGameplay>(Cameralayer);
	if (IsValid(cameralayerGameplay))
	{
		if (!bIsImmediate)
		{
			Remeber_TargetArmLength = cameralayerGameplay->TargetArmLength;
		}
	}
}

void FKMCameraGameLayerPlayInstance::Update(float deltaTime)
{
	Super::Update(deltaTime);

	UKMCameralayerGameplay* cameralayerGameplay = Cast<UKMCameralayerGameplay>(Cameralayer);
	if (IsValid(cameralayerGameplay))
	{
		cameralayerGameplay->TargetArmLength = FMath::FInterpTo(cameralayerGameplay->TargetArmLength, TargetArmLength, deltaTime, Speed); 
	}

	ElipsedTime += deltaTime * Rate;
}

FKMCameraSequenceLayerPlayInstance::~FKMCameraSequenceLayerPlayInstance()
{
	if (IsValid(Cameralayer))
	{
		if (bIsImmediate)
		{
			Cameralayer->SetAlpha(1.f);
		}
		else
		{
			Cameralayer->SetAlpha(0.f);
		}
	}
}

void FKMCameraSequenceLayerPlayInstance::Update(float deltaTime)
{
	Super::Update(deltaTime);
	
	float blendInAlpha = 1.0f;
	if (BlendInTime > 0.0f)
	{
		blendInAlpha = FMath::Clamp(ElipsedTime / BlendInTime, 0.0f, 1.0f);
	}

	float blendOutAlpha = 1.0f;
	if (BlendOutTime > 0.0f && !bIsImmediate)
	{
		blendOutAlpha = FMath::Clamp((Duration - ElipsedTime) / BlendOutTime, 0.0f, 1.0f);
	}

	float alpha = FMath::Min(blendInAlpha, blendOutAlpha);
				
	Cameralayer->SetAlpha(alpha);
				
	FEMCameraOutput cameraOutput;
	CameraCacheInstance->Evaluate(ElipsedTime, cameraOutput);

	Cameralayer->SetRelativeCameraData(cameraOutput);

	ElipsedTime += deltaTime * Rate;
}

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
	check(IsValid(GetWorld()));
	TSubclassOf<AKMCameraActorBase>* resultCameraClass = Cameras.Find(cameraName);
	if (resultCameraClass)
	{
		if (IsValid(CurrentCamera))
		{
			CurrentCamera->Destroy();
		}

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

AKMCameraActorBase* AKMPlayerCameraManager::GetCurrentCamera() const
{
	return CurrentCamera;
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

TSharedPtr<FKMCameraLayerPlayInstance> AKMPlayerCameraManager::PlayCameraLayer(EKMCameralayerType layerType,
	UCameraAnimationSequence* cameraSequence, float duration, float blendInTime, float blendOutTime, float rate, bool bImmediate)
{
	AKMCameraActorBase* currentCamera = GetCurrentCamera();
	if (!IsValid(currentCamera))
	{
		return nullptr;
	}
		
	TWeakPtr<FEMCameraCacheManager> cameraCacheManager = nullptr;
	if (GetWorld()->IsGameWorld())
	{
		if (UKMGameInstance* gameInstance = UKMGameInstance::GetGameInstance(this))
		{
			cameraCacheManager = gameInstance->GetCameraCacheManager();
		}
	}
	else
	{
		if (FEMMartialArtsModule* martialArtsModule = FModuleManager::LoadModulePtr<FEMMartialArtsModule>("EMMartialArts"))
		{
			cameraCacheManager = martialArtsModule->GetCameraCacheManager();
		}
	}
	if (!cameraCacheManager.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<FKMCameraLayerPlayInstance> newCameraLayerPlayInstance = nullptr;

	if (layerType == EKMCameralayerType::OverlaySequence)
	{
		TSharedPtr<FKMCameraSequenceLayerPlayInstance> newCameraSequenceLayerPlayInstance = MakeShared<FKMCameraSequenceLayerPlayInstance>();
		newCameraLayerPlayInstance = newCameraSequenceLayerPlayInstance;
		
		newCameraSequenceLayerPlayInstance->CameraCacheInstance = cameraCacheManager.Pin()->CreateCameraCacheInstance(cameraSequence);
		if (!newCameraSequenceLayerPlayInstance->CameraCacheInstance.IsValid())
		{
			return nullptr;
		}
	}
	else if (layerType == EKMCameralayerType::Gameplay)
	{
		TSharedPtr<FKMCameraGameLayerPlayInstance> newCameraGameLayerPlayInstance = MakeShared<FKMCameraGameLayerPlayInstance>();
		newCameraLayerPlayInstance = newCameraGameLayerPlayInstance;
	}

	if (!newCameraLayerPlayInstance.IsValid())
	{
		return nullptr;	
	}
	
	newCameraLayerPlayInstance->Cameralayer = currentCamera->GetCameraLayer(layerType);
	if (!IsValid(newCameraLayerPlayInstance->Cameralayer))
	{
		return nullptr;
	}
	newCameraLayerPlayInstance->Cameralayer->SetAlpha(1.f);
	newCameraLayerPlayInstance->BlendInTime = blendInTime;
	newCameraLayerPlayInstance->BlendOutTime = blendOutTime;
	newCameraLayerPlayInstance->Rate = rate;
	newCameraLayerPlayInstance->Duration = duration;
	newCameraLayerPlayInstance->bIsImmediate = bImmediate;
	newCameraLayerPlayInstance->BeginPlay();
	CameraLayerPlayInstances.Emplace(newCameraLayerPlayInstance);

	return newCameraLayerPlayInstance;
}

void AKMPlayerCameraManager::RemovePlayCameraLayer(const TSharedPtr<FKMCameraLayerPlayInstance>& cameraLayerPlayInstance)
{
	CameraLayerPlayInstances.Remove(cameraLayerPlayInstance);
}

void AKMPlayerCameraManager::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsValid(CurrentCamera))
	{
		CurrentCamera->Tick(deltaTime);
	}
	
	for (auto playInstanceItr = CameraLayerPlayInstances.CreateIterator(); playInstanceItr; ++playInstanceItr)
	{
		if ((*playInstanceItr)->IsEnd())
		{
			playInstanceItr.RemoveCurrent();
		}
		else
		{
			(*playInstanceItr)->Update(deltaTime);
		}
	}
}
#include "KMAnimNotifyState_Camera.h"
#include "CameraAnimationSequence.h"
#include "EMMartialArtsModule.h"
#include "Camera/KMCameraActorBase.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Camera/Layer/KMCameralayerOverlaySequence.h"
#include "Sequencer/EMCameraCacheManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_Camera
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMAnimNotifyState_Camera::UKMAnimNotifyState_Camera(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	SetGroupType(EEMNotifyGroupType::Animation);
#endif
}

FString UKMAnimNotifyState_Camera::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (IsValid(CameraSequence))
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *CameraSequence->GetName());		
	}
	
	return notifyName;
}

void UKMAnimNotifyState_Camera::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	if (FEMMartialArtsModule* martialArtsModule = FModuleManager::LoadModulePtr<FEMMartialArtsModule>("EMMartialArts"))
	{
		CameraCacheManager = martialArtsModule->GetCameraCacheManager();
	}
	if (CameraCacheManager.IsValid())
	{
		CameraCache = CameraCacheManager.Pin()->GetCameraCacheData(CameraSequence);
	}
	AnimationTimes.Emplace(meshComp, 0.f);

	AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp);
	if(IsValid(playerCameraManager))
	{
		if (AKMCameraActorBase* currentCamera = playerCameraManager->GetCurrentCamera())
		{
			CameraOverlayLayer = Cast<UKMCameralayerOverlaySequence>(currentCamera->GetCameraLayer(EKMCameralayerType::OverlaySequence));
			CameraOverlayLayer->SetAlpha(1.f);
		}
	}
}

void UKMAnimNotifyState_Camera::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (CameraCache.IsValid())
	{
		if (CameraOverlayLayer.IsValid())
		{
			if (float* alphaTime = AnimationTimes.Find(meshComp))
			{
				float blendInAlpha = 1.0f;
				if (BlendInTime > 0.0f)
				{
					blendInAlpha = FMath::Clamp((*alphaTime) / BlendInTime, 0.0f, 1.0f);
				}

				float blendOutAlpha = 1.0f;
				if (BlendOutTime > 0.0f)
				{
					blendOutAlpha = FMath::Clamp((eventReference.GetNotify()->Duration - (*alphaTime)) / BlendOutTime, 0.0f, 1.0f);
				}

				float alpha = FMath::Min(blendInAlpha, blendOutAlpha);
				
				//CameraOverlayLayer->SetAlpha(1.f);
				
				FEMCameraOutput cameraOutput;
				CameraCache->Evaluate(*alphaTime, cameraOutput);

				CameraOverlayLayer->SetRelativeCameraData(cameraOutput);
				(*alphaTime) += frameDeltaTime;
			}
		}
	}
}

void UKMAnimNotifyState_Camera::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	AnimationTimes.Remove(meshComp);

	if (CameraOverlayLayer.IsValid())
	{
		CameraOverlayLayer->SetAlpha(0.f);
	}

	AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp);
	if(IsValid(playerCameraManager))
	{
		playerCameraManager->StartCameraFade(1.f, 0.f, 2.f, FLinearColor::Black, false, false);
	}
}

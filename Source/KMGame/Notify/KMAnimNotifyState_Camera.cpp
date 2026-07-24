#include "KMAnimNotifyState_Camera.h"
#include "CameraAnimationSequence.h"
#include "EMMartialArtsModule.h"
#include "Camera/KMCameraActorBase.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Camera/Layer/KMCameralayerOverlaySequence.h"
#include "Core/KMGameInstance.h"
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
	AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp);
	if(IsValid(playerCameraManager))
	{
		CameraLayerPlayInstance = playerCameraManager->PlayCameraLayer(EKMCameralayerType::OverlaySequence,
			CameraSequence, eventReference.GetNotify()->Duration, BlendInTime, BlendOutTime, PlayRate, bIsImmadiate);
	}
}

void UKMAnimNotifyState_Camera::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_Camera::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp);
	if(IsValid(playerCameraManager))
	{
		if (CameraLayerPlayInstance.IsValid())
		{
			playerCameraManager->RemovePlayCameraLayer(CameraLayerPlayInstance.Pin());
		}
	}
}

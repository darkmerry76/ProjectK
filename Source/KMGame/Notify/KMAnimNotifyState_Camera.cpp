#include "KMAnimNotifyState_Camera.h"

#include "CameraAnimationSequence.h"
#include "EMMartialArtsModule.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sequencer/EMCameraCacheManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_Camera
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMAnimNotifyState_Camera::UKMAnimNotifyState_Camera(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	SetGroupType(EEMNotifyGroupType::Animation);
#endif

	if (FEMMartialArtsModule* martialArtsModule = FModuleManager::LoadModulePtr<FEMMartialArtsModule>("EMMartialArts"))
	{
		CameraCacheManager = martialArtsModule->GetCameraCacheManager();
	}
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
	if (CameraCacheManager.IsValid())
	{
		CameraCache = CameraCacheManager.Pin()->GetCameraCacheData(CameraSequence);
	}
	AnimationTimes.Emplace(meshComp, 0.f);
}

void UKMAnimNotifyState_Camera::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (CameraCache.IsValid())
	{
		AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp);
		if(IsValid(playerCameraManager))
		{
		if (float* alphaTime = AnimationTimes.Find(meshComp))
		{
			FEMCameraOutput cameraOutput;
			CameraCache->Evaluate(*alphaTime, cameraOutput);

			(*alphaTime) += frameDeltaTime;
		}
	}
}

void UKMAnimNotifyState_Camera::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	AnimationTimes.Remove(meshComp);
}

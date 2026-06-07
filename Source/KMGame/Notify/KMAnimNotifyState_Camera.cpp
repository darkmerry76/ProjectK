#include "KMAnimNotifyState_Camera.h"
#include "EMMartialArtsModule.h"
#include "Sequencer/EMCameraCacheManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_Camera
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMAnimNotifyState_Camera::UKMAnimNotifyState_Camera(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	GroupType = EEMNotifyGroupType::Animation;

	if (FEMMartialArtsModule* martialArtsModule = FModuleManager::LoadModulePtr<FEMMartialArtsModule>("EMMartialArts"))
	{
		CameraCacheManager = martialArtsModule->GetCameraCacheManager();
	}
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
		if (float* alphaTime = AnimationTimes.Find(meshComp))
		{
			CameraCache->Evaluate(*alphaTime);

			(*alphaTime) += frameDeltaTime;
		}
	}
}

void UKMAnimNotifyState_Camera::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	AnimationTimes.Remove(meshComp);
}

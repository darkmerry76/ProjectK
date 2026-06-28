#include "KMAnimNotifyState_FadeInout.h"

#include "Camera/KMPlayerCameraManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_FadeInout
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMAnimNotifyState_FadeInout::UKMAnimNotifyState_FadeInout(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	SetGroupType(EEMNotifyGroupType::Animation);
#endif
}

FString UKMAnimNotifyState_FadeInout::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	return notifyName;
}

void UKMAnimNotifyState_FadeInout::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	AnimationTimes.Emplace(meshComp, 0.f);
}

void UKMAnimNotifyState_FadeInout::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp);
	if(IsValid(playerCameraManager))
	{
		if (float* alphaTime = AnimationTimes.Find(meshComp))
		{
			float fadeInAlpha = FadeInTime > 0.f ? FMath::Clamp((*alphaTime) / FadeInTime, 0.0f, 1.0f) : 1.f;
			float fadeOutAlpha = FadeOutTime > 0.f ? FMath::Clamp((eventReference.GetNotify()->Duration - (*alphaTime)) / FadeOutTime, 0.0f, 1.0f) : 1.f;

			float alpha = FMath::Min(fadeInAlpha, fadeOutAlpha);
			playerCameraManager->SetManualCameraFade(1.f - alpha, FLinearColor::Black, false);
			(*alphaTime) += frameDeltaTime;
		}
	}
}

void UKMAnimNotifyState_FadeInout::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	AnimationTimes.Remove(meshComp);
}

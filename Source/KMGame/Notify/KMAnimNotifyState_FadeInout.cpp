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

void UKMAnimNotifyState_FadeInout::SetFadeValue(USkeletalMeshComponent* meshComp, float elipsedTime, const FAnimNotifyEventReference& eventReference)
{
	AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp);
	if(!IsValid(playerCameraManager))
	{
		return;
	}
	float fadeInAlpha = 1.0f;
	if (FadeInTime > 0.0f)
	{
		fadeInAlpha = FMath::Clamp(elipsedTime / FadeInTime, 0.0f, 1.0f);
	}

	float fadeOutAlpha = 1.0f;
	if (FadeOutTime > 0.0f)
	{
		fadeOutAlpha = FMath::Clamp((eventReference.GetNotify()->Duration - elipsedTime) / FadeOutTime, 0.0f, 1.0f);
	}
			
	float alpha = FMath::Min(fadeInAlpha, fadeOutAlpha);
	playerCameraManager->SetManualCameraFade(1.f - alpha, FLinearColor::Black, false);
}

void UKMAnimNotifyState_FadeInout::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	AnimationTimes.Emplace(meshComp, 0.f);
}

void UKMAnimNotifyState_FadeInout::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (float* alphaTime = AnimationTimes.Find(meshComp))
	{
		SetFadeValue(meshComp, *alphaTime, eventReference);
		(*alphaTime) += frameDeltaTime;
	}
}

void UKMAnimNotifyState_FadeInout::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	SetFadeValue(meshComp, eventReference.GetNotify()->Duration, eventReference);
	AnimationTimes.Remove(meshComp);
}

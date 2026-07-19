#include "KMAnimNotify_CameraShake.h"
#include "Camera/KMPlayerCameraManager.h"

UKMAnimNotify_CameraShake::UKMAnimNotify_CameraShake() : Super()
{
	GroupType = EEMNotifyGroupType::Animation; 
}

FString UKMAnimNotify_CameraShake::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (IsValid(CameraShakeClass))
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *CameraShakeClass->GetName());
	}
	
	return notifyName;
}

void UKMAnimNotify_CameraShake::Notify(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	Super::Notify(meshComp, animation, eventReference);

	if (AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(meshComp))
	{
		playerCameraManager->PlayWorldCameraShake(meshComp->GetWorld(), CameraShakeClass, meshComp->GetComponentLocation(), 1500.f, 1500.f, false);
	}
}
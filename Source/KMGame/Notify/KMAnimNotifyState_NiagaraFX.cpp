#include "KMAnimNotifyState_NiagaraFX.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

UKMAnimNotifyState_NiagaraFX::UKMAnimNotifyState_NiagaraFX(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::FX);
}

void UKMAnimNotifyState_NiagaraFX::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	SpawnedEffect = SpawnEffect(meshComp, animation);
}

void UKMAnimNotifyState_NiagaraFX::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_NiagaraFX::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(SpawnedEffect))
	{
		SpawnedEffect->Deactivate();
	}
}

UNiagaraComponent* UKMAnimNotifyState_NiagaraFX::SpawnEffect(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation)
{
	UNiagaraComponent* returnComp = nullptr;
	if (IsValid(Template))
	{
		if (Template->IsLooping())
		{
			return returnComp;
		}

		if (bIsAttached)
		{
			returnComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, meshComp, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
		}
		else
		{
			const FTransform meshTransform = meshComp->GetSocketTransform(SocketName);
			
			returnComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(meshComp->GetWorld(), Template,
				meshTransform.TransformPosition(LocationOffset), (meshTransform.GetRotation() * RotationOffsetQuat).Rotator(), FVector(1.0f),true);
		}

		if (IsValid(returnComp))
		{
			returnComp->SetUsingAbsoluteScale(bAbsoluteScale);
			returnComp->SetCustomTimeDilation(TimeDilation);
			returnComp->SetRelativeScale3D_Direct(Scale);
			returnComp->SetAutoDestroy(true);
		}
	}
	

	return returnComp;
}

UNiagaraComponent* UKMAnimNotifyState_NiagaraFX::GetSpawnedEffect() const 
{
	return SpawnedEffect;
}

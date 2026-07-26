#include "KMAnimNotifyState_NiagaraFX.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "System/EMIsolationSubsystem.h"

UKMAnimNotifyState_NiagaraFX::UKMAnimNotifyState_NiagaraFX(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::FX);
}

FString UKMAnimNotifyState_NiagaraFX::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (IsValid(Template))
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *Template->GetName());		
	}
	
	return notifyName;
}

void UKMAnimNotifyState_NiagaraFX::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	FKMAnimNotifyState_NiagaraFXData* niagaraFXData = SpawnedEffects.Find(targetMeshComp);
	if (niagaraFXData && !niagaraFXData->NiagaraComponents.IsEmpty())
	{
		int32 lastIndex = niagaraFXData->NiagaraComponents.Num() - 1;
		niagaraFXData->NiagaraComponents.Emplace(niagaraFXData->NiagaraComponents[lastIndex]);
		niagaraFXData->NiagaraComponents[lastIndex] = nullptr;
	}
	else
	{
		UNiagaraComponent* spawnedEffect = SpawnEffect(targetMeshComp, animation);
		if (IsValid(spawnedEffect))
		{
			SpawnedEffects.FindOrAdd(targetMeshComp).NiagaraComponents.Emplace(spawnedEffect);
		}
	}
}

void UKMAnimNotifyState_NiagaraFX::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_NiagaraFX::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	FKMAnimNotifyState_NiagaraFXData* niagaraFXData = SpawnedEffects.Find(targetMeshComp);
	
	if (!bIsContinue && niagaraFXData && !niagaraFXData->NiagaraComponents.IsEmpty())
	{
		if (IsValid(niagaraFXData->NiagaraComponents[0]))
		{
			niagaraFXData->NiagaraComponents[0]->Deactivate();
		}
		niagaraFXData->NiagaraComponents.RemoveAt(0);
	}
	if (niagaraFXData && niagaraFXData->NiagaraComponents.IsEmpty())
	{
		SpawnedEffects.Remove(targetMeshComp);
	}
}

UNiagaraComponent* UKMAnimNotifyState_NiagaraFX::SpawnEffect(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	
	UNiagaraComponent* returnComp = nullptr;
	if (IsValid(Template))
	{
		if (Template->IsLooping())
		{
			return returnComp;
		}
		if (bIsAttached)
		{
			returnComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, targetMeshComp, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
		}
		else
		{
			const FTransform meshTransform = targetMeshComp->GetSocketTransform(SocketName);
			
			returnComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(targetMeshComp->GetWorld(), Template,
				meshTransform.TransformPosition(LocationOffset), (meshTransform.GetRotation() * RotationOffset.Quaternion()).Rotator(), FVector(1.0f),true);
		}
		
		if (IsValid(returnComp))
		{
			returnComp->SetUsingAbsoluteScale(bAbsoluteScale);
			returnComp->SetCustomTimeDilation(TimeDilation);
			returnComp->SetRelativeScale3D_Direct(Scale);
			returnComp->ComponentTags = Tags;
			returnComp->SetAutoDestroy(true);

			if (UEMIsolationSubsystem* isolationSubsystem = UEMIsolationSubsystem::GetIsolationSubsystem(meshComp))
			{
				isolationSubsystem->AddExtraComponent(meshComp->GetOwner(), returnComp);
			}
		}
	}
	return returnComp;
}

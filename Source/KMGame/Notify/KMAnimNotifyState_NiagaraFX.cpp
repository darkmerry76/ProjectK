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
	USkeletalMeshComponent* targetMeshComp = Cast<USkeletalMeshComponent>(GetTargetSceneComponent(meshComp));
	if (IsValid(targetMeshComp))
	{
		SpawnEffect(targetMeshComp);
	}
}

void UKMAnimNotifyState_NiagaraFX::NotifyBeginEx(AActor* actor, UEMMartialArts* martialArts, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(actor) && IsValid(actor->GetRootComponent()))
	{
		SpawnEffect(actor->GetRootComponent());
	}
}

void UKMAnimNotifyState_NiagaraFX::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_NiagaraFX::NotifyTickEx(AActor* actor, UEMMartialArts* martialArts, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	
}

void UKMAnimNotifyState_NiagaraFX::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	if (USkeletalMeshComponent* targetMeshComp = Cast<USkeletalMeshComponent>(GetTargetSceneComponent(meshComp)))
	{
		EndEffect(targetMeshComp);
	}
}

void UKMAnimNotifyState_NiagaraFX::NotifyEndEx(class AActor* actor, class UEMMartialArts* martialArts, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(actor) && IsValid(actor->GetRootComponent()))
	{
		EndEffect(actor->GetRootComponent());
	}
}

void UKMAnimNotifyState_NiagaraFX::EndEffect(const USceneComponent* ownerComponent)
{
	FKMAnimNotifyState_NiagaraFXData* niagaraFXData = SpawnedEffects.Find(ownerComponent);
		
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
		SpawnedEffects.Remove(ownerComponent);
	}
}

UNiagaraComponent* UKMAnimNotifyState_NiagaraFX::SpawnEffect(USceneComponent* ownerComponent)
{
	if (!IsValid(ownerComponent))
	{
		return nullptr;;
	}
	
	UNiagaraComponent* returnComp = nullptr;
	if (IsValid(Template))
	{
		if (Template->IsLooping())
		{
			return returnComp;
		}
		if (bIsAttached)
		{
			returnComp = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, ownerComponent, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
		}
		else
		{
			const FTransform meshTransform = ownerComponent->GetSocketTransform(SocketName);
			
			returnComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(ownerComponent->GetWorld(), Template,
				meshTransform.TransformPosition(LocationOffset), (meshTransform.GetRotation() * RotationOffset.Quaternion()).Rotator(), FVector(1.0f),true);
		}
		
		if (IsValid(returnComp))
		{
			returnComp->SetUsingAbsoluteScale(bAbsoluteScale);
			returnComp->SetCustomTimeDilation(TimeDilation);
			returnComp->SetRelativeScale3D_Direct(Scale);
			returnComp->ComponentTags = Tags;
			returnComp->SetAutoDestroy(true);

			if (UEMIsolationSubsystem* isolationSubsystem = UEMIsolationSubsystem::GetIsolationSubsystem(ownerComponent))
			{
				isolationSubsystem->AddExtraComponent(ownerComponent->GetOwner(), returnComp);
			}
		}
	}

	FKMAnimNotifyState_NiagaraFXData* niagaraFXData = SpawnedEffects.Find(ownerComponent);
	if (niagaraFXData && !niagaraFXData->NiagaraComponents.IsEmpty())
	{
		int32 lastIndex = niagaraFXData->NiagaraComponents.Num() - 1;
		niagaraFXData->NiagaraComponents.Emplace(niagaraFXData->NiagaraComponents[lastIndex]);
		niagaraFXData->NiagaraComponents[lastIndex] = nullptr;
	}
	else
	{
		SpawnedEffects.FindOrAdd(ownerComponent).NiagaraComponents.Emplace(returnComp);
	}

	return returnComp;
}

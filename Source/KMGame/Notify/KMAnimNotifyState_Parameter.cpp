#include "KMAnimNotifyState_Parameter.h"
#include "Core/KMParameterLayerSystem.h"
#include "Core/KMParameterValue.h"

UKMAnimNotifyState_Parameter::UKMAnimNotifyState_Parameter(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::FX);
}

FString UKMAnimNotifyState_Parameter::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	return notifyName;
}

FName UKMAnimNotifyState_Parameter::GetLayerName() const
{
	return FName(*LexToString(GetUniqueID()));
}

void UKMAnimNotifyState_Parameter::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	
	if (UKMParameterLayerSubsystem* parameterLayerSubsystem = UKMParameterLayerSubsystem::GetParameterLayerSubsystem(targetMeshComp))
	{
		TSharedPtr<FKMSkeletalMeshComponentParameterLayer> skeletalMeshComponentParameterLayer = parameterLayerSubsystem->GetSkeletalMeshComponentLayer(targetMeshComp);
		if (skeletalMeshComponentParameterLayer.IsValid())
		{
			for (auto paramItr : Parameters)
			{
				if (!paramItr.IsValid())
				{
					continue;
				}
				skeletalMeshComponentParameterLayer->AddMaterialParameterLayer(GetLayerName(), paramItr.Get<FKMParameterBase>().ParamName, paramItr, paramItr.Get<FKMParameterBase>().BlendMode, paramItr.Get<FKMParameterBase>().Priority);
			}
		}
	}
}

void UKMAnimNotifyState_Parameter::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	
	if (UKMParameterLayerSubsystem* parameterLayerSubsystem = UKMParameterLayerSubsystem::GetParameterLayerSubsystem(targetMeshComp))
	{
		TSharedPtr<FKMSkeletalMeshComponentParameterLayer> skeletalMeshComponentParameterLayer = parameterLayerSubsystem->GetSkeletalMeshComponentLayer(targetMeshComp);
		if (skeletalMeshComponentParameterLayer.IsValid() && !FMath::IsNearlyZero(eventReference.GetNotify()->Duration))
		{
			skeletalMeshComponentParameterLayer->AddMaterialParameterAlpha(GetLayerName(), frameDeltaTime / eventReference.GetNotify()->Duration);
		}
	}
}

void UKMAnimNotifyState_Parameter::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	USkeletalMeshComponent* targetMeshComp = GetTargetMeshComp(meshComp);
	
	if (UKMParameterLayerSubsystem* parameterLayerSubsystem = UKMParameterLayerSubsystem::GetParameterLayerSubsystem(targetMeshComp))
	{
		TSharedPtr<FKMSkeletalMeshComponentParameterLayer> skeletalMeshComponentParameterLayer = parameterLayerSubsystem->GetSkeletalMeshComponentLayer(targetMeshComp);
		if (skeletalMeshComponentParameterLayer.IsValid())
		{
			skeletalMeshComponentParameterLayer->RemoveMaterialParameterLayer(GetLayerName());
		}
	}
}
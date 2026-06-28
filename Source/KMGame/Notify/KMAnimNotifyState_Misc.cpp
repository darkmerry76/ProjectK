#include "KMAnimNotifyState_Misc.h"

#include "EMMartialArts.h"
#include "EngineUtils.h"
#include "Actor/KMItemAppearanceActor.h"
#include "Character/KMCharacter.h"
#include "Component/KMMartialArtsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Skill/Ability/KMAbility.h"
#include "System/EMIsolationSubsystem.h"

UKMAnimNotifyState_Misc::UKMAnimNotifyState_Misc(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::Plan);
}

void UKMAnimNotifyState_Misc::SetTimeDilation(USkeletalMeshComponent* meshComp, float newTimeDilation)
{
	if (!IsValid(meshComp) || !IsValid(meshComp->GetWorld()))
	{
		return;
	}
	
	float finalTimeDilation = newTimeDilation;
	
	if (IsValid(GlobalTimeDilationCurve) && !FMath::IsNearlyEqual(TotalDuration, 0.f))
	{
		float minValue = 0.f;
		float maxValue = 1.f;
		
		GlobalTimeDilationCurve->GetTimeRange(minValue, maxValue);

		float time = FMath::Lerp(minValue, maxValue, ElapsedTime / TotalDuration);
		
		finalTimeDilation = GlobalTimeDilationCurve->GetFloatValue(time) * newTimeDilation;
	}

	UGameplayStatics::SetGlobalTimeDilation(meshComp, finalTimeDilation);
}

void UKMAnimNotifyState_Misc::CollectionShowActor(AActor* newActor, TArray<AActor*>& showActors)
{
	if (!IsValid(newActor))
	{
		return;
	}
	
	showActors.Emplace(newActor);

	if (AKMCharacter* character = Cast<AKMCharacter>(newActor))
	{
		if (IsValid(character->WeaponInstance))
		{
			showActors.Emplace(character->WeaponInstance->GetSpawnedActor());
		}
	}
}

void UKMAnimNotifyState_Misc::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyBegin(meshComp, animation, totalDuration, eventReference);

	ElapsedTime = 0.f;
	TotalDuration = totalDuration;

	if (bIsOverride_GlobalTimedilation)
	{
		SetTimeDilation(meshComp, GlobalTimeDilationScale);
	}

	if (bIsOverride_ShowOwnerCharacter || bIsOverride_ShowTargetCharacter)
	{
		if (UEMIsolationSubsystem* isolationSubsystem = UEMIsolationSubsystem::GetIsolationSubsystem(meshComp))
		{
			TArray<AActor*> showActors;
			if (bIsOverride_ShowOwnerCharacter)
			{
				CollectionShowActor(meshComp->GetOwner(), showActors);
			}
			if (bIsOverride_ShowTargetCharacter)
			{
				if (const FKMMartialArtsSkillContextData* skillContextData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
				{
					if (UKMAbility* ability = Cast<UKMAbility>(skillContextData->GetAbility()))
					{
						CollectionShowActor(ability->GetTargetCharacter(), showActors);
					}
				}
			}
			
			isolationSubsystem->EnterIsolation(showActors);
		}
	}
}

void UKMAnimNotifyState_Misc::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyTick(meshComp, animation, frameDeltaTime, eventReference);

	if (bIsOverride_GlobalTimedilation)
	{
		SetTimeDilation(meshComp, GlobalTimeDilationScale);
	}
	ElapsedTime += frameDeltaTime;
}

void UKMAnimNotifyState_Misc::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	Super::NotifyEnd(meshComp, animation, eventReference);

	if (bIsOverride_GlobalTimedilation)
	{
		UGameplayStatics::SetGlobalTimeDilation(meshComp, 1.f);
	}

	if (bIsOverride_ShowOwnerCharacter || bIsOverride_ShowTargetCharacter)
	{
		if (UEMIsolationSubsystem* isolationSubsystem = UEMIsolationSubsystem::GetIsolationSubsystem(meshComp))
		{
			isolationSubsystem->LeaveIsolation();
		}
	}
}
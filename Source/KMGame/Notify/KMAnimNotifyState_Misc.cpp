#include "KMAnimNotifyState_Misc.h"
#include "EMMartialArts.h"
#include "Actor/KMItemAppearanceActor.h"
#include "Character/KMCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "System/EMIsolationSubsystem.h"
#include "System/KMGameObjectSubsystem.h"

UKMAnimNotifyState_Misc::UKMAnimNotifyState_Misc(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::Plan);
}

FString UKMAnimNotifyState_Misc::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	return notifyName;
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
		if (UKMItemAppearanceInstance* weaponInstance = character->GetWeaponInstance())
		{
			showActors.Emplace(weaponInstance->GetSpawnedActor());
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

	if (bIsOverride_ShowOwnerCharacter || bIsOverride_ShowOtherCharacter)
	{
		if (UEMIsolationSubsystem* isolationSubsystem = UEMIsolationSubsystem::GetIsolationSubsystem(meshComp))
		{
			TArray<AActor*> showActors;
			if (bIsOverride_ShowOwnerCharacter)
			{
				CollectionShowActor(meshComp->GetOwner(), showActors);
			}
			if (bIsOverride_ShowOtherCharacter && meshComp->GetWorld()->IsGameWorld())
			{
				if (UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(meshComp))
				{
					for (auto objectItr : gameObjectSubsystem->GetGameObjectMap())
					{
						if (UKMCharacterInstance* characterInstance = Cast<UKMCharacterInstance>(objectItr.Value))
						{
							CollectionShowActor(characterInstance->GetCharacter(), showActors);
						}
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

	if (bIsOverride_ShowOwnerCharacter || bIsOverride_ShowOtherCharacter)
	{
		if (UEMIsolationSubsystem* isolationSubsystem = UEMIsolationSubsystem::GetIsolationSubsystem(meshComp))
		{
			isolationSubsystem->LeaveIsolation();
		}
	}
}
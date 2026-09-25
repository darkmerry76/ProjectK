#include "KMAbilitySkill.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMTargetSubsystem.h"
#include "Util/KMUtil.h"

UKMAbilitySkill::UKMAbilitySkill(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilitySkill::Activate()
{
	PlayMartialArts(nullptr, Rate, bIsLoop);
	PostActivated();

	Super::Activate();
}

void UKMAbilitySkill::PostActivated()
{
	if (bIsDirectionFallow)
	{
		if (UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance())
		{
			if (IsValid(GetTargetActor()))
			{
				FVector targetToDirection = GetTargetActor()->GetActorLocation() - GetOwnerActor()->GetActorLocation();
				targetToDirection.Z = 0.0f;
				targetToDirection.Normalize();

				ownerGameObjectInstance->SetDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection) * DirectionWeight), bIsForceRotation);
			}
		}
	}
}

void UKMAbilitySkill::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);

	UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance();
	if(!IsValid(ownerGameObjectInstance))
	{
		return;
	}

	UKMSkillHandler* skillHandler = ownerGameObjectInstance->GetSkillHandler();
	check(IsValid(skillHandler));

	if (SkillInstance.IsValid() && !bCancel && EndingTag.IsValid())
	{
		if (bUsedHitResult && SkillInstance.IsValid())
		{
			if (SkillInstance.Pin()->Target.IsValid())
			{
				SkillInstance.Pin()->Target->Targets.Empty();
			}
			
			if (!HitResults.IsEmpty() && SkillInstance.Pin()->Target.IsValid())
			{
				if (AKMCharacter* targetCharacter = Cast<AKMCharacter>(HitResults[0].GetActor()))
				{
					if (UKMGameObjectInstance* targetGameObjectInstance = targetCharacter->GetGameObjectInstance())
					{
						SkillInstance.Pin()->Target->Targets.Emplace(targetGameObjectInstance->GetId());
					}
				}
			}
		}
		
		skillHandler->TransitionTechniqueSkill(SkillInstance.Pin(), EndingTag);
	}

	if (bIsDirectionFallow)
	{
/*		UKMCharacterInstance* ownerCharacterInstance = GetOwnerCharacterInstance();
		check(IsValid(ownerCharacterInstance));
		
		FVector ownerForwardDirection = character->GetActorForwardVector();
		ownerForwardDirection.Z = 0.0f;
		ownerForwardDirection.Normalize();
	
		ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D8Way(FVector2D(ownerForwardDirection)));*/
	}
}

void UKMAbilitySkill::SetSkillInstance(const TSharedPtr<FKMSkillInstance>& newSkillInstance)
{
	SkillInstance = newSkillInstance;
}

TWeakPtr<FKMSkillInstance> UKMAbilitySkill::GetSkillInstance() const
{
	return SkillInstance;
}

void UKMAbilitySkill::ForceComplate()
{
	if (SkillInstance.IsValid())
	{
		SkillInstance.Pin()->SetForceComplete(true);
	}
}

void UKMAbilitySkill::AppendHitResult(const TArray<FHitResult>& hitResults)
{
	HitResults.Append(hitResults);
}

void UKMAbilitySkill::ClearHitResults()
{
	HitResults.Empty();
}
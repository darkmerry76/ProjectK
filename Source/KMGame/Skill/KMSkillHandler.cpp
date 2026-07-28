#include "KMSkillHandler.h"
#include <Tables/Generated/KMTable_SkillEffectTransition.h>
#include <Tables/Generated/KMTable_SkillSet_Hero.h>
#include <Tables/Generated/KMTable_Skill_Normal.h>
#include <Tables/Generated/KMTable_Skill_Projectile.h>
#include "Ability/KMAbility.h"
#include "Character/KMCharacter.h"
#include "DataAsset/KMAssetManager.h"
#include "GameObject/KMCharacterInstance.h"
#include "GameObject/KMGameObjectInstance.h"
#include "Stat/KMStatModifierBase.h"
#include "System/KMGameObjectSubsystem.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_Skill.h"
#include "Tables/Generated/KMTable_SkillEffect.h"
#include "Animation/AnimSequence.h"
#include "Component/KMCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tables/Generated/KMTable_Character.h"
#include "Tables/Generated/KMTable_SkillCondition.h"
#include "Animation/AnimSequence.h"

UKMSkillHandler::UKMSkillHandler(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

UKMGameObjectInstance* UKMSkillHandler::GetOwner() const
{
	return GetTypedOuter<UKMGameObjectInstance>();
}

const TArray<FKMSkillKey>& UKMSkillHandler::GetOwnedSkills() const
{
	return OwnedSkills;
}

void UKMSkillHandler::SetAbilityFlag(EKMAbilityFlag newFlag)
{
	AbilityMask |= static_cast<int8>(newFlag);
}

void UKMSkillHandler::ClearAbilityFlag(EKMAbilityFlag flag)
{
	AbilityMask = AbilityMask & ~static_cast<int32>(flag);
}

bool UKMSkillHandler::HasAbilityFlag(EKMAbilityFlag flag) const
{
	return AbilityMask & static_cast<int32>(flag);
}

void UKMSkillHandler::ClearResisterSkillSet()
{
	OwnedSkills.Empty();
	OwnenSkillSets.Empty();
	CooltimeInstances.Empty();
}

void UKMSkillHandler::ClearActiveSkills()
{
	ComboData.Reset();
	
	for (auto skillInstanceItr = SkillInstances.CreateIterator(); skillInstanceItr; ++skillInstanceItr)
	{
		TSharedPtr<FKMSkillInstance> skillInstance = skillInstanceItr->Value;
		if (!skillInstance.IsValid())
		{
			continue;
		}

		if (skillInstance->SkillKey.TableRecord->Type != EKMSkillType::Active)
		{
			continue;
		}
		if (TSharedPtr<FKMAbilityInstanceCooltime>* timerInstance = CooltimeInstances.Find(skillInstance->SkillKey))
		{
			(*timerInstance)->ForceReady();
		}
		
		skillInstance->Leave();
		OnRemoveAbilityInstance(skillInstance);
		skillInstanceItr.RemoveCurrent();
	}
}

void UKMSkillHandler::ClearPassiveSkills()
{
	for (auto skillInstanceItr = SkillInstances.CreateIterator(); skillInstanceItr; ++skillInstanceItr)
	{
		TSharedPtr<FKMSkillInstance> skillInstance = skillInstanceItr->Value;
		if (!skillInstance.IsValid())
		{
			continue;
		}

		if (skillInstance->SkillKey.TableRecord->Type != EKMSkillType::Passive)
		{
			continue;
		}
		if (TSharedPtr<FKMAbilityInstanceCooltime>* timerInstance = CooltimeInstances.Find(skillInstance->SkillKey))
		{
			(*timerInstance)->ForceReady();
		}
		
		skillInstance->Leave();
		OnRemoveAbilityInstance(skillInstance);
		skillInstanceItr.RemoveCurrent();
	}
}

void UKMSkillHandler::ClearAllSkills()
{
	for (auto skillInstanceItr = SkillInstances.CreateIterator(); skillInstanceItr; ++skillInstanceItr)
	{
		TSharedPtr<FKMSkillInstance> skillInstance = skillInstanceItr->Value;
		if (!skillInstance.IsValid())
		{
			continue;
		}
		if (TSharedPtr<FKMAbilityInstanceCooltime>* timerInstance = CooltimeInstances.Find(skillInstance->SkillKey))
		{
			(*timerInstance)->ForceReady();
		}
		
		skillInstance->Leave();
		OnRemoveAbilityInstance(skillInstance);
		skillInstanceItr.RemoveCurrent();
	}
	ComboData.Reset();
}

void UKMSkillHandler::RegisterSkillSets(const FKMTable_SkillSetRow* newSkillSet)
{
	check(newSkillSet);
	OwnenSkillSets.Emplace(newSkillSet);
	RegisterSkills(newSkillSet->Skills);
}

void UKMSkillHandler::RegisterSkills(const TArray<FKMSkillKey>& skillKeys)
{
	for (FKMSkillKey skillKey : skillKeys)
	{
		if (!skillKey.IsValid())
		{
			continue;
		}
		RegisterSkill(FKMSkillKey::CreateKey(skillKey.TableId, skillKey.Level));
	}
}

void UKMSkillHandler::RegisterSkills(const TArray<FName>& skillIds)
{
	for (FName skillId : skillIds)
	{
		if (skillId == NAME_None)
		{
			continue;
		}
		RegisterSkill(FKMSkillKey::CreateKey(skillId, 0));
	}
}

void UKMSkillHandler::RegisterSkill(const FKMSkillKey& newSkillKey)
{
	check(newSkillKey.IsValid());
	if (OwnedSkills.Contains(newSkillKey))
	{
		return;
	}
	
	OwnedSkills.Emplace(newSkillKey);
	TSharedPtr<FKMAbilityInstanceCooltime> newCooltime = MakeShared<FKMAbilityInstanceCooltime>(this, newSkillKey);
	newCooltime->ForceReady();
	CooltimeInstances.Emplace(newSkillKey, newCooltime);

	if (newSkillKey.TableRecord->Type == EKMSkillType::Passive)
	{
		UseSkill(newSkillKey, MakeShared<FKMLockOnCluster>(this));
	}
}

bool UKMSkillHandler::HasOwnedSkill(const FKMSkillKey& skillKey) const
{
	return OwnedSkills.Contains(skillKey);
}

bool UKMSkillHandler::IsReadyCooltime(const FKMSkillKey& skillKey) const
{
	const TSharedPtr<FKMAbilityInstanceCooltime>* timerInstance = CooltimeInstances.Find(skillKey);
	check(timerInstance);

	check((*timerInstance)->GetType() == FKMAbilityInstanceCooltime::TypeName());
	
	return (*timerInstance)->IsReady();
}

bool UKMSkillHandler::ResetCooltime(const FKMSkillKey& skillKey)
{
	TSharedPtr<FKMAbilityInstanceCooltime>* timerInstance = CooltimeInstances.Find(skillKey);
	if(timerInstance == nullptr)
	{
		return false;
	}

	check((*timerInstance)->GetType() == FKMAbilityInstanceCooltime::TypeName());
	
	(*timerInstance)->Reset();
	return true;
}

bool UKMSkillHandler::IsSkillAvailable(const FKMSkillKey& skillKey) const
{
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));

	if (ownerCharacterInstance->IsDead())
	{
		return false;
	}
	if (!skillKey.IsValid())
	{
		return false;
	}
	
	if (!HasOwnedSkill(skillKey))
	{
		return false;
	}

	if (!IsReadyCooltime(skillKey))
	{
		return false;	
	}
	
	const FKMTable_SkillRow* newSkillTable = FKMTable_SkillRow::FindRowPtr(skillKey.TableId, skillKey.Level);
	check(newSkillTable);

	int32 overlapCount = 0;
	for (auto activatedskillItr : SkillInstances)
	{
		TSharedPtr<FKMSkillInstance> activatedskill = activatedskillItr.Value;
		const FKMTable_SkillRow* activatedskillTable = FKMTable_SkillRow::FindRowPtr(activatedskill->SkillKey.TableId, activatedskill->SkillKey.Level);
		check(activatedskillTable);

		if (activatedskill->SkillKey.TableId == skillKey.TableId && !activatedskill->IsComplete())
		{
			overlapCount++;
		}

		if (activatedskillTable->Type == EKMSkillType::Active && newSkillTable->Type == EKMSkillType::Active)
		{
			//return false;
		}
	}

	if (newSkillTable->OverlapCount > 0 && overlapCount >= newSkillTable->OverlapCount)
	{
		return false;
	}
	
	return true;
}

bool UKMSkillHandler::CanUseSkill(const FKMSkillKey& skillKey, const TSharedPtr<FKMLockOnCluster>& lockOnCluster) const
{
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));

	if (!IsSkillAvailable(skillKey))
	{
		return false;
	}
	
/*	if(lockOnCluster.IsValid() == false)
	{
		return false;
	}

	if (lockOnCluster->IsBestTargetAvailable() == false)
	{
		return false;
	}*/

	UKMStatModifierBase* statModifier = ownerCharacterInstance->GetStatModifier();
	check(IsValid(statModifier));

	if (skillKey.TableRecord->CostHp > 0 &&
		statModifier->GetEffectiveStat().GetHpCurr() < static_cast<double>(skillKey.TableRecord->CostHp))
	{
		return false;
	}
	
	if (skillKey.TableRecord->CostMp > 0 &&
		statModifier->GetEffectiveStat().GetMpCurr() < static_cast<double>(skillKey.TableRecord->CostMp))
	{
		return false;
	}

	if (skillKey.TableRecord->CostEmp > 0 &&
		statModifier->GetEffectiveStat().GetEmpCurr() < static_cast<double>(skillKey.TableRecord->CostEmp))
	{
		return false;
	}

	if (skillKey.TableRecord->CostTempo > 0 &&
		statModifier->GetEffectiveStat().GetTempoCurr() < static_cast<double>(skillKey.TableRecord->CostTempo))
	{
		return false;
	}

/*	FVector offsetLocation = ownerCharacterInstance->GetTransform().GetLocation() - lockOnCluster->GetTargetTransform().GetLocation();
	offsetLocation.X = 0.0f;

	float skillDistance = offsetLocation.Size();
	if(skillDistance > skillKey.TableRecord->Range && skillKey.TableRecord->Range > 0.001f)
	{
		return false;
	}*/
	
	return true;
}

bool UKMSkillHandler::IsUsedSkill() const
{
	int32 numActiveSkill = 0;
	for (auto activatedSkillItr : SkillInstances)
	{
		if (activatedSkillItr.Value->SkillKey.TableRecord->Type == EKMSkillType::Active)
		{
			++numActiveSkill;
		}
	}
	
	return numActiveSkill != 0;
}

float UKMSkillHandler::GetConditionScore(const FName& skillConditionName, TSharedPtr<FKMLockOnCluster> lockOnCluster) const
{
	UKMCharacterInstance* targetCharacter = lockOnCluster->GetBestTarget();
	if (!IsValid(targetCharacter))
	{
		
		return -1.f;
	}
	return GetConditionScore(skillConditionName, targetCharacter);
}

float UKMSkillHandler::GetConditionScore(const FName& skillConditionName, const UKMCharacterInstance* targetCharacter) const
{
	const FKMTable_SkillConditionRow* skillConditionRow = FKMTable_SkillConditionRow::FindRowPtr(skillConditionName);
	if (!skillConditionRow)
	{
		return -1.f;
	}
	
	UKMCharacterInstance* ownerCharacter = Cast<UKMCharacterInstance>(GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return -1.f;
	}

	UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacter()->GetCharacterMovement());

	FVector ownerForward = ownerCharacter->GetCharacter()->GetActorForwardVector();
	if (skillConditionRow->LocomotionState == EKMLocomotionState::Land && !characterMovement->IsOnGround())
	{
		return -1.f;
	}
	else if (skillConditionRow->LocomotionState == EKMLocomotionState::Air && !characterMovement->IsAir())
	{
		if (!characterMovement->IsCustomMovementMode(EKMCustomMovementMode::CMODE_Jump))
		{
			return -1.f;
		}
	}

	FVector targetToDirection = targetCharacter->GetCharacter()->GetActorLocation() - ownerCharacter->GetCharacter()->GetActorLocation();
	float targetToDistance = targetToDirection.Size();
		
	if (skillConditionRow->TargetRangeMin > targetToDistance || skillConditionRow->TargetRange < targetToDistance)
	{
		return -1.f;
	}

	float center = (skillConditionRow->TargetRangeMin + skillConditionRow->TargetRange) * 0.5f;
	float halfRange = (skillConditionRow->TargetRange - skillConditionRow->TargetRangeMin) * 0.5f;

	float distanceScore = 1.f - (FMath::Abs(targetToDistance - center) / halfRange);
	distanceScore = FMath::Clamp(distanceScore, 0.f, 1.f);

	FVector targetToNormal = targetToDirection.GetSafeNormal();

	float dot = FVector::DotProduct(ownerForward, targetToNormal);
	float angleDeg = FMath::RadiansToDegrees(FMath::Acos(dot));
	if (angleDeg > skillConditionRow->TargetDir)
	{
		return -1.f;
	}

	float angleScore = FVector::DotProduct(ownerForward, targetToNormal);
	float resultScore = angleScore * 2.f + distanceScore * 1.5f;
	return resultScore; 
}

void UKMSkillHandler::ActivatedNextComboSkill()
{
	if (!ComboData.IsValid())
	{
		return;
	}

	if (ComboData.CurrentCombo == ComboData.NextCombo)
	{
		return;
	}

	TSharedPtr<FKMSkillInstance> newSkillInstance = UseSkill(FKMSkillKey(ComboData.skillSet->Skills[ComboData.NextCombo], 0), ComboData.LockOnCluster);
	if (!newSkillInstance.IsValid())
	{
		return;
	}

	if (ComboData.SkillInstance.IsValid())
	{
		ComboData.SkillInstance.Pin()->SetForceComplete(true);
	}

	ComboData.CurrentCombo = ComboData.NextCombo;
	ComboData.SkillInstance = newSkillInstance;
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::UseUltimateSkill()
{
	if (ComboData.SkillInstance.IsValid())
	{
		return nullptr;
	}

	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));
	
	if (ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag) &&
	!ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::Event_Cancel_Tag) &&
	!ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::State_Cancel_Tag))
	{
		return nullptr;
	}
	float bestScore = -9999.f;
	FName bestSkillId = NAME_None;

	for (auto skillsetItr = OwnenSkillSets.CreateConstIterator(); skillsetItr; ++skillsetItr)
	{
		const FKMTable_SkillSetRow* skillSetRow = *skillsetItr;

		if (!skillSetRow->Ultimate)
		{
			continue;
		}

		for (int32 skillIndex = 0; skillIndex < skillSetRow->Skills.Num(); ++skillIndex)
		{
			float currentSkillScore = GetConditionScore(skillSetRow->Skills[skillIndex], nullptr);
			if (currentSkillScore > bestScore)
			{
				bestScore = currentSkillScore;
				bestSkillId = skillSetRow->Skills[skillIndex];
			}
		}
	}

	TSharedPtr<FKMSkillInstance> newSkillInstance = UseSkill(FKMSkillKey(bestSkillId, 0), nullptr);
	if (!newSkillInstance.IsValid())
	{
		return nullptr;
	}
	
	return newSkillInstance;
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::UseCombatSkill(const TSharedPtr<FKMLockOnCluster>& lockOnCluster)
{
	if (!lockOnCluster.IsValid() || !lockOnCluster->IsBestTargetAvailable())
	{
		return nullptr;
	}
	
	const FKMTable_SkillSetRow* skillSet = ComboData.skillSet;;
	float comboSkillScore = -1.f;

	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));

	if (ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag) &&
		!ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::Event_Cancel_Tag) &&
		!ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::State_Cancel_Tag))
	{
		return nullptr;
	}

	if (ComboData.IsValid())
	{
		if (ownerCharacterInstance->HasGameplayTag(FKMGameplayTagName::Event_Cancel_Combo_Available_Tag))
		{
			if (ComboData.SkillInstance.IsValid())
			{
				if (ComboData.CurrentCombo == -1 || ComboData.CurrentCombo + 1 < ComboData.skillSet->Skills.Num())
				{
					comboSkillScore = GetConditionScore(ComboData.skillSet->Skills[ComboData.CurrentCombo + 1], lockOnCluster);
					if (comboSkillScore > 0.f)
					{
						ComboData.NextCombo = ComboData.CurrentCombo + 1;
					}
				}
			}
		}
	}
	if (comboSkillScore < 0.f)
	{
		if (ComboData.SkillInstance.IsValid())
		{
			return nullptr;
		}
		ComboData.Reset();
	}
	
	if (!ComboData.IsValid())
	{
		skillSet = EvalurateSkillSet(lockOnCluster);
		ComboData.skillSet = skillSet;
		ComboData.CurrentCombo = -1;
		ComboData.NextCombo = 0;
	}

	if (!skillSet || ComboData.NextCombo >= skillSet->Skills.Num())
	{
		return nullptr;
	}

	if (ComboData.NextCombo != 0)
	{
		return nullptr;
	}
	if (ComboData.CurrentCombo != -1)
	{
		return nullptr;
	}

	ComboData.LockOnCluster = MakeShared<FKMLockOnCluster>(*lockOnCluster.Get());
	
	ActivatedNextComboSkill();
	
	if (!ComboData.SkillInstance.IsValid())
	{
		ComboData.Reset();
		return nullptr;;
	}

	return ComboData.SkillInstance.Pin();
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::UseTechniqueSkill(const TSharedPtr<FKMLockOnCluster>& lockOnCluster)
{
	if (ComboData.SkillInstance.IsValid())
	{
		return nullptr;
	}

	float bestScore = 0.f;
	FName bestSkillId = NAME_None;

	TSharedPtr<FKMLockOnCluster> newLockOnCluster = MakeShared<FKMLockOnCluster>(*lockOnCluster.Get());
	for (auto skillsetItr = OwnenSkillSets.CreateConstIterator(); skillsetItr; ++skillsetItr)
	{
		const FKMTable_SkillSetRow* skillSetRow = *skillsetItr;

		if (!skillSetRow->Technique)
		{
			continue;
		}

		for (int32 skillIndex = 0; skillIndex < skillSetRow->Skills.Num(); ++skillIndex)
		{
			float currentSkillScore = GetConditionScore(skillSetRow->Skills[skillIndex], newLockOnCluster);
			if (currentSkillScore > bestScore)
			{
				bestScore = currentSkillScore;
				bestSkillId = skillSetRow->Skills[skillIndex];
			}
		}
	}
	if (bestSkillId == NAME_None)
	{
		return nullptr;
	}

	TSharedPtr<FKMSkillInstance> newSkillInstance = UseSkill(FKMSkillKey(bestSkillId, 0), newLockOnCluster);
	if (!newSkillInstance.IsValid())
	{
		return nullptr;
	}
	ApplyEffects(newSkillInstance, FKMGameplayTagName::Event_Grab_Tag);
	return nullptr;
}

const FKMTable_SkillSetRow* UKMSkillHandler::EvalurateSkillSet(const TSharedPtr<FKMLockOnCluster>& lockOnCluster) const
{
	return EvalurateSkillSet(lockOnCluster->GetBestTarget());
}

const FKMTable_SkillSetRow* UKMSkillHandler::EvalurateSkillSet(const UKMCharacterInstance* targetCharacterInstance) const
{
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));
	
	float bestScore = 0.f;
	const FKMTable_SkillSetRow* bestSkillset = nullptr;
	for (auto skillsetItr = OwnenSkillSets.CreateConstIterator(); skillsetItr; ++skillsetItr)
	{
		const FKMTable_SkillSetRow* skillSetTableRow = *skillsetItr;
		if (skillSetTableRow->Technique)
		{
			continue;
		}
		if (!skillSetTableRow->OwnerCharacter.IsEmpty())
		{
			if (!skillSetTableRow->OwnerCharacter.ContainsByPredicate([&](const FName& characterId)
			{
				return ownerCharacterInstance->GetCharacterId() == characterId;
			})){
				continue;
			}
		}
		check(skillSetTableRow && !skillSetTableRow->Skills.IsEmpty());

		float currentScore = GetConditionScore(skillSetTableRow->Skills[0], targetCharacterInstance);
		if (currentScore > bestScore)
		{
			bestSkillset = skillSetTableRow;
			bestScore = currentScore;
		}
	}

	return bestSkillset;
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::UseSkill(const FKMSkillKey& skillKey, TSharedPtr<FKMLockOnCluster> lockOnCluster)
{
	TSharedPtr<FKMSkillInstance> newSkillInstance = MakeShared<FKMSkillInstance>(this, skillKey);
	newSkillInstance->Caster = GetOwner()->GetId();
	newSkillInstance->Target = lockOnCluster;
	;	
	return UseSkillInternal(newSkillInstance);
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::UseAssistSkill(const FKMSkillKey& skillKey)
{
	TSharedPtr<FKMAssistSkillInstance> newSkillInstance = MakeShared<FKMAssistSkillInstance>(this, skillKey);
	newSkillInstance->Caster = GetOwner()->GetId();
	;	
	return UseSkillInternal(newSkillInstance);
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::UseSkillInternal(const TSharedPtr<FKMSkillInstance>& newSkillInstance)
{
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	if (IsValid(ownerCharacterInstance) == true)
	{
		return UseSkillInternal(ownerCharacterInstance, newSkillInstance);
	}
	return nullptr;
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::UseSkillInternal(UKMCharacterInstance* ownerCharacterInstance, const TSharedPtr<FKMSkillInstance>& newSkillInstance)
{
	if (!CanUseSkill(newSkillInstance->SkillKey, newSkillInstance->Target))
	{
		return nullptr;
	}
	
	check(IsValid(ownerCharacterInstance));
	
	const FKMTable_SkillRow* skillTable = newSkillInstance->SkillKey.TableRecord;
	check(skillTable);

	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(assetManager));

	if (const FKMTable_Skill_NormalRow* normalSkillTable = CastRow<FKMTable_Skill_NormalRow>(skillTable))
	{
		AKMCharacter* ownerCharacter = ownerCharacterInstance->GetCharacter();
		check(IsValid(ownerCharacter));

		UKMAbility* newAbility = nullptr;
		UObject* assetObject = assetManager->GetAsset(normalSkillTable->Ability);
		if (UEMMartialArts* martialArts = Cast<UEMMartialArts>(assetObject))
		{
			if (IsValid(martialArts->GetAbilityBP()) && IsValid(martialArts->GetAbilityBP()->GeneratedClass))
			{
				UClass* abilityGeneratedClass = martialArts->GetAbilityBP()->GeneratedClass;
				newAbility = NewObject<UKMAbility>(this, abilityGeneratedClass);
				newAbility->SetMartialArts(martialArts);
			}
		}
		else
		{
			if (UClass* abilityClass = Cast<UClass>(assetObject))
			{
				newAbility = NewObject<UKMAbility>(this, abilityClass);
			}
		}
		if (IsValid(newAbility))
		{
			newAbility->SetLockOnCluster(newSkillInstance->Target);
			newAbility->Activate();
			newAbility->SetSkillInstance(newSkillInstance);
		}
	}
	
	if (skillTable->Type == EKMSkillType::Active)
	{
		LatestActiveSkillInstance = newSkillInstance;
	}

	newSkillInstance->SkillTriggerDelegate.AddUObject(this, &ThisClass::OnSkillTrigger);
	newSkillInstance->SkillEffectTriggerDelegate.AddUObject(this, &ThisClass::OnSkillEffectTrigger);
	
	UKMStatModifierBase* statModifier = ownerCharacterInstance->GetStatModifier();
	check(IsValid(statModifier));
	
	statModifier->GetEffectiveStat().SetHpCurr(
		statModifier->GetEffectiveStat().GetHpCurr() - static_cast<int32>(skillTable->CostHp), true);

	statModifier->GetEffectiveStat().SetMpCurr(
		statModifier->GetEffectiveStat().GetMpCurr() - static_cast<int32>(skillTable->CostMp), true);

	statModifier->GetEffectiveStat().SetEmpCurr(
		statModifier->GetEffectiveStat().GetEmpCurr() - static_cast<int32>(skillTable->CostEmp), true);
	
	statModifier->GetEffectiveStat().SetTempoCurr(
		statModifier->GetEffectiveStat().GetTempoCurr() - static_cast<int32>(skillTable->CostTempo), true);
	
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	AbilityEvents.FindOrAdd(newSkillInstance);
	OnAddAbilityInstance(newSkillInstance);
	newSkillInstance->Enter();
	
	SkillInstances.Emplace(LastAbilityUniqueId++, newSkillInstance);
	ResetCooltime(newSkillInstance->SkillKey);

	ApplyEffects(newSkillInstance, FKMGameplayTagName::Event_Skill_Start_Tag);

	return newSkillInstance;
}

void UKMSkillHandler::OnSkillEffectTrigger(const FGameplayTag& eventTag, const TSharedPtr<FKMSkillInstance>& skillInstance)
{
	check (skillInstance.IsValid());

	ApplyEffects(skillInstance, eventTag);
}

void UKMSkillHandler::OnSkillTrigger(const FGameplayTag& eventTag, const TSharedPtr<FKMSkillInstance>& skillInstance)
{
	check (skillInstance.IsValid());
	
	if (const FKMTable_Skill_ProjectileRow* projectileSkillTable = CastRow<FKMTable_Skill_ProjectileRow>(skillInstance->SkillKey.TableRecord))
	{
		if (eventTag.GetTagName() == projectileSkillTable->TriggerEvent)
		{
			GetProjectileTriggerDelegate().Broadcast(skillInstance);
		}
	}
}

TArray<TSharedPtr<FKMSkillEffectInstance>> UKMSkillHandler::ApplyEffects(const TSharedPtr<FKMSkillInstance>& skillInstance, const FGameplayTag& eventTag)
{
	check(skillInstance.IsValid());

	UKMCharacterInstance* ownerCharacter = Cast<UKMCharacterInstance>(GetOwner());
	check(ownerCharacter);

	TArray<TSharedPtr<FKMSkillEffectInstance>> outSkillEffectInstances;
	if (ownerCharacter->HasGameplayTag(FKMGameplayTagName::State_Parry_Tag))
	{
		return outSkillEffectInstances;
	}

	UKMCharacterInstance* casterCharacter = Cast<UKMCharacterInstance>(UKMGameObjectSubsystem::GetGameObjectSubsystem(this)->GetGameObject(skillInstance->Caster));
	check(casterCharacter);
	
	TSet<UKMCharacterInstance*> targetInstances;
	if (skillInstance->Target.IsValid())
	{
		for (int32 targetIndex = 0; targetIndex < skillInstance->Target->NumTarget(); ++targetIndex)
		{
			UKMCharacterInstance* targetCharacterInstance = Cast<UKMCharacterInstance>(skillInstance->Target->GetTargetByIndex(targetIndex));
			if (!IsValid(targetCharacterInstance))
			{
				continue;
			}
			targetInstances.FindOrAdd(targetCharacterInstance);
		}
	}
	
	for (auto skillEffectItr : skillInstance->SkillKey.TableRecord->Effects)
	{
		const FKMTable_SkillEffectRow* skillEffectRow = FKMTable_SkillEffectRow::FindRowPtr(skillEffectItr);
		check(skillEffectRow);

		TSet<UKMCharacterInstance*> fianltargetInstances;
		
		if (skillEffectRow->TargetType == EKMSkillEffectTargetType::Instigator)
		{
			fianltargetInstances.Emplace(casterCharacter);
		}
		else if (skillEffectRow->TargetType == EKMSkillEffectTargetType::Target)
		{
			fianltargetInstances.Append(targetInstances);
		}

		if (eventTag.IsValid() && (skillEffectRow->ActivatedEvent == NAME_None ||
			(skillEffectRow->ActivatedEvent != NAME_None &&
			FGameplayTag::RequestGameplayTag(skillEffectRow->ActivatedEvent) != eventTag)))
		{
			continue;
		}

		for (auto targetItr : fianltargetInstances)
		{
			UKMCharacterInstance* targetCharacterInstance = targetItr;
			check(IsValid(targetCharacterInstance));
			
			UKMSkillHandler* targetSkillHandler = targetCharacterInstance->GetSkillHandler();
			check(IsValid(targetSkillHandler));
	
			TSharedPtr<FKMSkillEffectInstance> newSkillInstance = targetSkillHandler->ApplyEffectInternal(skillInstance, skillEffectItr);
			if (newSkillInstance.IsValid())
			{
				outSkillEffectInstances.Emplace(newSkillInstance);
			}
		}
	}

	return outSkillEffectInstances;
}

TSharedPtr<FKMSkillEffectInstance> UKMSkillHandler::ApplyEffectInternal(const TSharedPtr<FKMSkillInstance>& skillInstance, const FName& effectName)
{
	if (!skillInstance.IsValid())
	{
		return nullptr;
	}
	check(skillInstance->SkillKey.TableRecord);

	TSharedPtr<FKMSkillInstance> dupSkillInstance = MakeShared<FKMSkillInstance>(*skillInstance.Get());

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	const FKMTable_SkillEffectRow* skillEffectTable = FKMTable_SkillEffectRow::FindRowPtr(effectName);
	check(skillEffectTable);

	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));

	for (auto readTag: skillEffectTable->ReadGameplaytag)
	{
		bool isNot = false;
		FString tagToString = readTag.ToString();
		if (tagToString.StartsWith(TEXT("!")))
		{
			isNot = true;
			tagToString = tagToString.RightChop(1);
		}
		if (ownerCharacterInstance->HasGameplayTag(FGameplayTag::RequestGameplayTag(*tagToString)) == isNot)
		{
			return nullptr;
		}
	}

	TSharedPtr<FKMSkillEffectInstance> newSkillEffectInstance;
	switch (skillEffectTable->Type)
	{
	case EKMSkillEffectType::Damage:
		newSkillEffectInstance = MakeShared<FKMSkillEffectDamageInstance>(this, skillEffectTable, dupSkillInstance);
		break;
	case EKMSkillEffectType::Abnormal:
		newSkillEffectInstance = MakeShared<FKMSkillEffectAbnormalInstance>(this, skillEffectTable, dupSkillInstance);
		break;
	case EKMSkillEffectType::Buff:
		newSkillEffectInstance = MakeShared<FKMSkillEffectBuffInstance>(this, skillEffectTable, dupSkillInstance);
		break;
	default: return nullptr;	
	}
	check(newSkillEffectInstance.IsValid());

	EffectInstances.Emplace(LastAbilityUniqueId++, newSkillEffectInstance);
	OnAddAbilityInstance(newSkillEffectInstance);
		//check(newSkillEffectInstance->GetOwnerSkillInstance()->Target->GetBestTarget() == GetTypedOuter<UKMCharacterInstance>());
	newSkillEffectInstance->Enter();

	return newSkillEffectInstance;
}

int32 UKMSkillHandler::GetScoreSkill(const FKMSkillKey& skillKey) const
{
	if (!IsSkillAvailable(skillKey))
	{
		return -1;
	}

	if (skillKey.TableRecord->Type != EKMSkillType::Active)
	{
		return -1;
	}
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));

	UKMStatModifierBase* statModifier = ownerCharacterInstance->GetStatModifier();
	check(IsValid(statModifier));

	if (skillKey.TableRecord->CostHp > 0)
	{
		if (statModifier->GetEffectiveStat().GetHpCurr() > skillKey.TableRecord->CostHp)
		{
			return 100;
		}
		else
		{
			return -1;
		}
	}

	if (skillKey.TableRecord->CostMp > 0)
	{
		if (statModifier->GetEffectiveStat().GetMpCurr() > skillKey.TableRecord->CostMp)
		{
			return 100;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}

template<typename _TLKey, typename _TLValue>
void UKMSkillHandler::UpdateAbilities(TMap<_TLKey, TSharedPtr<_TLValue>>& abilityInstances, float deltaSeconds)
{
	for (auto skillInstanceItr = abilityInstances.CreateIterator(); skillInstanceItr; ++skillInstanceItr)
	{
		TSharedPtr<FKMAbilityInstanceBase> abilityInstance = skillInstanceItr->Value;
		if (!abilityInstance.IsValid())
		{
			continue;
		}
		
		abilityInstance->Tick(deltaSeconds);
		abilityInstance->PostTick(deltaSeconds);
		if (abilityInstance->IsComplete())
		{
			abilityInstance->Leave();
			OnRemoveAbilityInstance(abilityInstance);
			skillInstanceItr.RemoveCurrent();
		}
	}
}

void UKMSkillHandler::Tick(float deltaSeconds)
{
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	check(IsValid(ownerCharacterInstance));

	UpdateAbilities<FKMSkillKey, FKMAbilityInstanceCooltime>(CooltimeInstances, deltaSeconds);
	
	UpdateAbilities<int32, FKMSkillInstance>(SkillInstances, deltaSeconds);
	UpdateAbilities<int32, FKMSkillEffectInstance>(EffectInstances, deltaSeconds);
}

void UKMSkillHandler::OnAddAbilityInstance(TSharedPtr<FKMAbilityInstanceBase> abilityInstance)
{
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	if (abilityInstance->IsA(FKMSkillInstance::TypeName()))
	{
		TSharedPtr<FKMSkillInstance> skillInstance = StaticCastSharedPtr<FKMSkillInstance>(abilityInstance);
		for (auto tag : skillInstance->SkillKey.TableRecord->GameplayTag)
		{
			ownerCharacterInstance->AddGameplayTag(FGameplayTag::RequestGameplayTag(tag));
		}
	}
	else if (abilityInstance->IsA(FKMSkillEffectInstance::TypeName()))
	{
		TSharedPtr<FKMSkillEffectInstance> skillEffectInstance = StaticCastSharedPtr<FKMSkillEffectInstance>(abilityInstance);
		for (auto tag : skillEffectInstance->GetEffectTableRecord()->WriteGameplayTag)
		{
			ownerCharacterInstance->AddGameplayTag(FGameplayTag::RequestGameplayTag(tag));
		}
	}
}

void UKMSkillHandler::OnRemoveAbilityInstance(TSharedPtr<FKMAbilityInstanceBase> abilityInstance)
{
	AbilityEvents.Remove(abilityInstance);
	
	UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(GetOwner());
	if (abilityInstance->IsA(FKMSkillInstance::TypeName()))
	{
		TSharedPtr<FKMSkillInstance> skillInstance = StaticCastSharedPtr<FKMSkillInstance>(abilityInstance);
		for (auto tag : skillInstance->SkillKey.TableRecord->GameplayTag)
		{
			ownerCharacterInstance->RemoveGameplayTag(FGameplayTag::RequestGameplayTag(tag));
		}
	}
	else if (abilityInstance->IsA(FKMSkillEffectInstance::TypeName()))
	{
		TSharedPtr<FKMSkillEffectInstance> skillEffectInstance = StaticCastSharedPtr<FKMSkillEffectInstance>(abilityInstance);
		for (auto tag : skillEffectInstance->GetEffectTableRecord()->WriteGameplayTag)
		{
			ownerCharacterInstance->RemoveGameplayTag(FGameplayTag::RequestGameplayTag(tag));
		}
	}
}

void UKMSkillHandler::TriggerEvent(const FGameplayTag& eventTag)
{
	for (auto& abilityItr : AbilityEvents)
	{
		if (abilityItr->GetType() == FKMSkillInstance::TypeName())
		{
			abilityItr->OnTriggerEvent(eventTag);	
		}
	}
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::GetLatestActiveSkillInstance() const
{
	if (!LatestActiveSkillInstance.IsValid())
	{
		return nullptr;
	}

	return LatestActiveSkillInstance.Pin();
}

TSharedPtr<FKMSkillInstance> UKMSkillHandler::GetSkillInstance(const FKMSkillKey& skillKey) const
{
	for (auto skillItr = SkillInstances.CreateConstIterator(); skillItr; ++skillItr)
	{
		const TSharedPtr<FKMSkillInstance> skillInstance = skillItr->Value;  
		if (skillInstance->SkillKey == skillKey)
		{
			return skillInstance;
		}
	}
	return nullptr;
}

bool UKMSkillHandler::IsSkillActivated(const FKMSkillKey& skillKey) const
{
	for (auto skillItr = SkillInstances.CreateConstIterator(); skillItr; ++skillItr)
	{
		const TSharedPtr<FKMSkillInstance> skillInstance = skillItr->Value;  
		if (skillInstance->SkillKey == skillKey)
		{
			return true;
		}
	}
	return false;
}

void UKMSkillHandler::TriggerTransitionSkillEffect(const FGameplayTag& effectTag)
{
	UKMCharacterInstance* characterInstance =  GetTypedOuter<UKMCharacterInstance>();
	
	for (auto effectItr = EffectInstances.CreateIterator(); effectItr; ++effectItr)
	{
		TSharedPtr<FKMSkillEffectInstance> skillEffectInstance = effectItr->Value;  
		if (skillEffectInstance->GetEffectTableRecord()->TransitionId == NAME_None)
		{
			continue;
		}

		const FKMTable_SkillEffectTransitionRow* effectTransitionRow =
			FKMTable_SkillEffectTransitionRow::FindRowPtr(skillEffectInstance->GetEffectTableRecord()->TransitionId, effectTag.GetTagName());
		if (!effectTransitionRow)
		{
			continue;
		}
		check(skillEffectInstance->GetOwnerSkillInstance()->Target->GetBestTarget() == characterInstance); 

		ApplyEffectInternal(skillEffectInstance->GetOwnerSkillInstance(), effectTransitionRow->BranchEffectId);
		skillEffectInstance->SetForceComplete(true);
	}
}
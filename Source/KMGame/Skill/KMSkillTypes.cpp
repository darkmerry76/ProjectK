#include "KMSkillTypes.h"
#include <Tables/Generated/KMTable_SkillEffect.h>
#include <Tables/Generated/KMTable_SkillEffect_Grab.h>
#include <Tables/Generated/KMTable_SkillEffect_Normal.h>
#include <Tables/Generated/KMTable_Skill_Normal.h>
#include "GameplayTagContainer.h"
#include "Ability/KMAbility.h"
#include "Ability/KMAbilityEffect.h"
#include "Ability/KMAbilitySkill.h"
#include "DataAsset/KMAssetManager.h"
#include "GameObject/KMCharacterInstance.h"
#include "Stat/KMStatModifierBase.h"
#include "System/KMGameObjectSubsystem.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_Skill.h"

FKMSkillKey FKMSkillKey::CreateKey(FName tableId, int32 level)
{
	return FKMSkillKey(tableId, level);
}

FKMAbilityInstanceBase::FKMAbilityInstanceBase() : OwnerObject(nullptr)
{
}

FKMAbilityInstanceBase::FKMAbilityInstanceBase(const FKMAbilityInstanceBase& sourceInstanceBase) : OwnerObject(sourceInstanceBase.OwnerObject)
	, ElipsedTime(sourceInstanceBase.ElipsedTime)
	, TimeScale(sourceInstanceBase.TimeScale)
	, Copied(true)
{
}

FKMAbilityInstanceBase::FKMAbilityInstanceBase(UObject* ownerObject) : OwnerObject(ownerObject)
{
}

FKMAbilityInstanceBase::~FKMAbilityInstanceBase()
{
}

void FKMAbilityInstanceBase::Enter()
{
	ActivatedAbility();
}

void FKMAbilityInstanceBase::Leave(bool bCancel)
{
	DeactivatedAbility(bCancel);
}

void FKMAbilityInstanceBase::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(OwnerObject);
	Collector.AddReferencedObjects(AbilitieAssets);
}

float FKMAbilityInstanceBase::GetElipsedTime() const
{
	return ElipsedTime;
}

void FKMAbilityInstanceBase::ResetElipsedTime()
{
	ElipsedTime = 0.f;
}

void FKMAbilityInstanceBase::PostTick(float deltaSeconds)
{
	ElipsedTime += deltaSeconds * TimeScale;
}

void FKMAbilityInstanceBase::OnTriggerEvent(const FGameplayTag& eventTag)
{
	for (auto& ability : AbilitieAssets)
	{
		ability->OnTriggerEvent(eventTag);
	}
}

void FKMAbilityInstanceBase::ActivatedAbility()
{
}

void FKMAbilityInstanceBase::DeactivatedAbility(bool bCancel)
{
	RemoveAllAbilityAsset(true, bCancel);
}

UKMAbility* FKMAbilityInstanceBase::AddAbillityAsset(const FName& pDAKey)
{
	if (!OwnerObject.IsValid())
	{
		return nullptr;
	}
	
	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(assetManager) == true);

	UObject* assetObject = assetManager->GetAsset(pDAKey);

	UKMAbility* newAbility = nullptr;
	if (UEMMartialArts* martialArts = Cast<UEMMartialArts>(assetObject))
	{
		if (IsValid(martialArts->GetAbilityBP()->GeneratedClass))
		{
			newAbility = NewObject<UKMAbility>(OwnerObject.Pin().Get(), martialArts->GetAbilityBP()->GeneratedClass);
			newAbility->SetMartialArts(martialArts);
		}
	}
	else
	{
		UClass* abilityClass = Cast<UClass>(assetObject);
		if (!abilityClass)
		{
			return nullptr;
		}
		newAbility = NewObject<UKMAbility>(OwnerObject.Pin().Get(), abilityClass);
	}
	if (!IsValid(newAbility))
	{
		return nullptr;
	}
	
	AbilitieAssets.Emplace(newAbility);
	return newAbility;
}

void FKMAbilityInstanceBase::RemoveAbilityAsset(UKMAbility* ability)
{
	AbilitieAssets.Remove(ability);
}

void FKMAbilityInstanceBase::RemoveAllAbilityAsset(bool bDeactivateEvent, bool bCancel)
{
	if (bDeactivateEvent)
	{
		for (auto ability : AbilitieAssets)
		{
			ability->Deactivate(bCancel);
		}
	}
	AbilitieAssets.Empty();
}

const TArray<TObjectPtr<class UKMAbility>> FKMAbilityInstanceBase::GetAbilitieAssets() const
{
	return AbilitieAssets;
};

FKMAbilityInstanceCooltime::FKMAbilityInstanceCooltime(UObject* ownerObject, const FKMSkillKey& skillKey) : FKMAbilityInstanceBase(ownerObject)
	, SkillKey(skillKey)
{
}

float FKMAbilityInstanceCooltime::GetElipsedTime() const
{
	UKMCharacterInstance* characterInstance = OwnerObject->GetTypedOuter<UKMCharacterInstance>();
	check(IsValid(characterInstance))
	
	return FKMAbilityInstanceBase::GetElipsedTime() * characterInstance->GetStatModifier()->GetEffectiveStat().GetCooltimeWeights();
}

void FKMAbilityInstanceCooltime::ForceReady()
{
	if (const FKMTable_Skill_NormalRow* skillNormal = CastRow<FKMTable_Skill_NormalRow>(SkillKey.TableRecord))
	{
		PostTick(skillNormal->CoolTime); 
	}
}

bool FKMAbilityInstanceCooltime::IsReady() const
{
	UKMCharacterInstance* characterInstance = OwnerObject->GetTypedOuter<UKMCharacterInstance>();
	check(IsValid(characterInstance))
	
	if (const FKMTable_Skill_NormalRow* skillNormal = CastRow<FKMTable_Skill_NormalRow>(SkillKey.TableRecord))
	{
		return GetElipsedTime() >= skillNormal->CoolTime;
	}
	return true;
}

void FKMAbilityInstanceCooltime::Reset()
{
	ResetElipsedTime();
}

FKMSkillInstance::FKMSkillInstance(UObject* ownerObject, const FKMSkillKey& skillKey) : FKMAbilityInstanceBase(ownerObject)
	, SkillKey(skillKey)
{
	State = EKMSkillState::Start;
}

FKMSkillInstance::FKMSkillInstance(const FKMSkillInstance& sourceSkillInstance) : FKMAbilityInstanceBase(sourceSkillInstance)
	, SkillKey(sourceSkillInstance.SkillKey)
	, Caster(sourceSkillInstance.Caster)
	, Target(nullptr)
	, Tags(sourceSkillInstance.Tags)
{
	if(sourceSkillInstance.Target.IsValid())
	{
		Target = MakeShared<FKMLockOnCluster>(*sourceSkillInstance.Target.Get());
	}
}

void FKMSkillInstance::Enter()
{
	FKMAbilityInstanceBase::Enter();
}

void FKMSkillInstance::ActivatedAbility()
{
	FKMAbilityInstanceBase::ActivatedAbility();
	if (const FKMTable_Skill_NormalRow* normalSkillTable = CastRow<FKMTable_Skill_NormalRow>(SkillKey.TableRecord))
	{
		if (UKMAbilitySkill* newAbilitySkill = Cast<UKMAbilitySkill>(AddAbillityAsset(normalSkillTable->Ability.PdaKey)))
		{
			newAbilitySkill->SetLockOnCluster(Target);
			newAbilitySkill->SetSkillInstance(SharedThis(this));
			newAbilitySkill->Activate();
		}
	}
}

void FKMSkillInstance::DeactivatedAbility(bool bCancel)
{
	FKMAbilityInstanceBase::DeactivatedAbility(bCancel);
}

bool FKMSkillInstance::IsComplete() const
{
	check(OwnerObject.IsValid());

	if (bIsForceComplete)
	{
		return true;
	}

	if (const FKMTable_Skill_NormalRow* skillNormal = CastRow<FKMTable_Skill_NormalRow>(SkillKey.TableRecord))
	{
		if (skillNormal->Duration < 0.0001f)
		{
			return false;
		}
		if (skillNormal->ActiveType == EKMSkillActiveType::Cahnneling)
		{
			if (State != EKMSkillState::End)
			{
				return false;
			}

			if (skillNormal->EndTime <= GetElipsedTime())
			{
				return true;
			}
		}
		
		if (skillNormal->CoolTime < 0.0001f)
		{
			return false;
		}

		if (skillNormal->Duration <= GetElipsedTime())
		{
			return true;
		}
	}
	return false;
}

void FKMSkillInstance::AddAssistSkill(TSharedPtr<class FKMSkillInstance> assistSkillInstance)
{
	AssistSkills.Emplace(assistSkillInstance);	
}

void FKMSkillInstance::OnTriggerEvent(const FGameplayTag& eventTag)
{
	FKMAbilityInstanceBase::OnTriggerEvent(eventTag);
	for (int32 assistSkillIndex = 0; assistSkillIndex < AssistSkills.Num(); ++assistSkillIndex)
	{
		TSharedPtr<FKMSkillInstance> assistSkillInstance = AssistSkills[assistSkillIndex];
		if (!assistSkillInstance.IsValid())
		{
			continue;
		}
		assistSkillInstance->OnTriggerEvent(eventTag);
	}
}

bool FKMSkillInstance::CanTransitionTo(EKMSkillState newState) const
{
	if (ResevedState == newState)
	{
		return false;
	}
	
	switch (State)
	{
	case EKMSkillState::Start:
	case EKMSkillState::Loop:
		return (newState == EKMSkillState::End);

	default:
		return false;
	}
}

void FKMSkillInstance::TransitionTo(EKMSkillState newState)
{
	State = newState;
	OnStateEnter(newState);
}

void FKMSkillInstance::TransitionTo_Reseved(EKMSkillState newState)
{
	ResevedState = newState;
}

void FKMSkillInstance::OnStateEnter(EKMSkillState newState)
{
	ResetElipsedTime();
	
	if (newState == EKMSkillState::End)
	{
		for (auto& ability : AbilitieAssets)
		{
			ability->OnRequestEnd();
		}
	}
}

void FKMSkillInstance::RequestEnd()
{
	if (!CanTransitionTo(EKMSkillState::End))
	{
		return;
	}
	
	TransitionTo_Reseved(EKMSkillState::End);
}

void FKMSkillInstance::Tick(float deltaSeconds)
{
	if (bIsSuspend)
	{
		return;
	}
	deltaSeconds = deltaSeconds * TimeScale;

	if (const FKMTable_Skill_NormalRow* skillNormal = CastRow<FKMTable_Skill_NormalRow>(SkillKey.TableRecord))
	{
		if (skillNormal->ActiveType == EKMSkillActiveType::Cahnneling)
		{
			if (ResevedState == EKMSkillState::End)
			{
				if (skillNormal->Duration <= GetElipsedTime())
				{
					TransitionTo(EKMSkillState::End);
				}
			}			
		}
	}
}


FKMAssistSkillInstance::FKMAssistSkillInstance(UObject* ownerObject, const FKMSkillKey& skillKey) : FKMSkillInstance(ownerObject, skillKey)
{
}

void FKMAssistSkillInstance::Enter()
{
	FKMSkillInstance::Enter();
}

bool FKMAssistSkillInstance::IsComplete() const
{
	return bIsForceComplete;
}

void FKMAssistSkillInstance::OnTriggerEvent(const FGameplayTag& eventTag)
{
	FKMSkillInstance::OnTriggerEvent(eventTag);
}

void FKMAssistSkillInstance::Tick(float deltaSeconds)
{
}

FKMSkillEffectInstance::FKMSkillEffectInstance(UObject* ownerObject,
	const FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance) : FKMAbilityInstanceBase(ownerObject)
	, OwnerSkillInstance(ownerSkillInstance)
	, EffectTableRecord(effectTableRecord)
{
	check(EffectTableRecord != nullptr);
}

void FKMSkillEffectInstance::Reset()
{
	DeactivatedAbility(!IsComplete());
	ActivatedAbility();

	ResetElipsedTime();
}

bool FKMSkillEffectInstance::IsComplete() const
{
	check(OwnerObject.IsValid());

	if (bIsForceComplete)
	{
		return true;
	}

	if (EffectTableRecord->Duration < 0.f)
	{
		return false;
	}
	
	if (GetElipsedTime() >= EffectTableRecord->Duration)
	{
		return true;
	}
	return false;
}

void FKMSkillEffectInstance::ActivatedAbility()
{
	FKMAbilityInstanceBase::ActivatedAbility();

	if (!EffectTableRecord)
	{
		return;
	}
	
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(OwnerObject.Get());
	check(IsValid(gameObjectSubsystem));

	if (!OwnerObject.IsValid())
	{
		return;
	}

	UKMCharacterInstance* targetCharacterInstance = Cast<UKMCharacterInstance>(OwnerObject->GetTypedOuter<UKMCharacterInstance>());
	check(IsValid(targetCharacterInstance));

	UKMStatModifierBase* targetStat = targetCharacterInstance->GetStatModifier();
	check(IsValid(targetStat));

	check(targetCharacterInstance == targetStat->GetTypedOuter<UKMCharacterInstance>());

	if (UKMAbilityEffect* newEffectAbility = Cast<UKMAbilityEffect>(AddAbillityAsset(EffectTableRecord->Ability.PdaKey)))
	{
		newEffectAbility->SetSkillEffectInstance(SharedThis(this));
		newEffectAbility->SetLockOnCluster(OwnerSkillInstance->Target);
		newEffectAbility->SetCastObjectKey(OwnerSkillInstance->Caster);
		newEffectAbility->Activate();
	}
}

void FKMSkillEffectInstance::DeactivatedAbility(bool bCancel)
{
	FKMAbilityInstanceBase::DeactivatedAbility(bCancel);
}

void FKMSkillEffectInstance::OnTriggerEvent(const FGameplayTag& eventTag)
{
	FKMAbilityInstanceBase::OnTriggerEvent(eventTag);
}

void FKMSkillEffectInstance::Enter()
{
	FKMAbilityInstanceBase::Enter();	
}

void FKMSkillEffectInstance::Leave(bool bCancel)
{
	FKMAbilityInstanceBase::Leave(bCancel);
}

const FKMTable_SkillEffectRow* FKMSkillEffectInstance::GetEffectTableRecord() const
{
	return EffectTableRecord;
}

FKMSkillEffectDamageInstance::FKMSkillEffectDamageInstance(UObject* ownerObject,
	const FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance) : FKMSkillEffectInstance(ownerObject, effectTableRecord, ownerSkillInstance)
{
}

void FKMSkillEffectDamageInstance::Classification()
{
	
}

void FKMSkillEffectDamageInstance::Enter()
{
	FKMSkillEffectInstance::Enter();
}

void FKMSkillEffectDamageInstance::Leave(bool bCancel)
{
	FKMSkillEffectInstance::Leave(bCancel);
}

void FKMSkillEffectDamageInstance::Tick(float deltaSeconds)
{
	if (!OwnerObject.IsValid())
	{
		return;
	}

	if (!OwnerSkillInstance->Target.IsValid())
	{
		return;
	}

	if(!OwnerObject.IsValid())
	{
		return;
	}

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(OwnerObject.Get());
	check(IsValid(gameObjectSubsystem));

	UKMCharacterInstance* casterCharacterInstance = Cast<UKMCharacterInstance>(gameObjectSubsystem->GetGameObject(OwnerSkillInstance->Caster));
	if (!IsValid(casterCharacterInstance))
	{
		return;
	}

	UKMStatModifierBase* casterStat = casterCharacterInstance->GetStatModifier();
	check(IsValid(casterStat));

	double basePhysicsDamage = casterStat->GetEffectiveStat().GetAtk() * EffectTableRecord->BaseValue;

	UKMCharacterInstance* targetCharacterInstance = Cast<UKMCharacterInstance>(OwnerObject->GetTypedOuter<UKMCharacterInstance>());
	check(IsValid(targetCharacterInstance));

	UKMStatModifierBase* targetStat = targetCharacterInstance->GetStatModifier();
	check(IsValid(targetStat));

	double finalPhysicsDamage = basePhysicsDamage * FMath::RandRange(0.9f, 1.1f);
	bool bCriChange = FMath::RandRange(0, static_cast<int32>(casterStat->GetEffectiveStat().GetCriChange())) == 0;
	if (bCriChange)
	{
		double criDamage = casterStat->GetEffectiveStat().GetCri() * FMath::RandRange(0.75f, 1.f);
		finalPhysicsDamage += criDamage;
	}

	float physicsDefence = targetStat->GetEffectiveStat().GetDef();
	finalPhysicsDamage -= physicsDefence;
	if (finalPhysicsDamage <= 0.f)
	{
		finalPhysicsDamage = 0.01f;
	}

	targetCharacterInstance->AddAggroTarget(casterCharacterInstance);

	if (bCriChange)
	{
		targetStat->GetEffectiveStat().BroadcastCriDamage(finalPhysicsDamage);
	}
	else
	{
		targetStat->GetEffectiveStat().BroadcastDamage(finalPhysicsDamage);
	}

	targetStat->GetEffectiveStat().ApplyDamage(finalPhysicsDamage, true);
	
	FKMDamageEvent newDamageEvent;
	newDamageEvent.Attacker =  casterCharacterInstance->GetId();
	newDamageEvent.Target =  targetCharacterInstance->GetId();
	newDamageEvent.Damage = basePhysicsDamage;
	newDamageEvent.bIsCritical = bCriChange;

	newDamageEvent.Context = EKMDamageEventContext::Attacker;
	casterCharacterInstance->BroadCastDamageEvent(newDamageEvent);
	
	newDamageEvent.Context = EKMDamageEventContext::Target;
	targetCharacterInstance->BroadCastDamageEvent(newDamageEvent);
}

FKMSkillEffectAbnormalInstance::FKMSkillEffectAbnormalInstance(UObject* ownerObject,
	const FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance) : FKMSkillEffectInstance(ownerObject, effectTableRecord, ownerSkillInstance)
{
}

void FKMSkillEffectAbnormalInstance::Classification()
{
	
}

void FKMSkillEffectAbnormalInstance::Enter()
{
	FKMSkillEffectInstance::Enter();
}

void FKMSkillEffectAbnormalInstance::Leave(bool bCancel)
{
	FKMSkillEffectInstance::Leave(bCancel);
}

void FKMSkillEffectAbnormalInstance::Tick(float deltaSeconds)
{
	if (!OwnerObject.IsValid())
	{
		return;
	}
}

FKMSkillEffectBuffInstance::FKMSkillEffectBuffInstance(UObject* ownerObject,
	const FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance) : FKMSkillEffectInstance(ownerObject, effectTableRecord, ownerSkillInstance)
{
}

void FKMSkillEffectBuffInstance::Classification()
{
}

void FKMSkillEffectBuffInstance::Enter()
{
	FKMSkillEffectInstance::Enter();

	Apply(0.f);
}

void FKMSkillEffectBuffInstance::Leave(bool bCancel)
{
	FKMSkillEffectInstance::Leave(bCancel);
}

double FKMSkillEffectBuffInstance::CalculateParameter(UKMCharacterInstance* target, double statValue, float deltaSeconds) const
{
	const FKMSecondaryStat& secondaryStat = target->GetStatModifier()->GetEffectiveStat();
	double baseValue = EffectTableRecord->BaseValue;
	switch(EffectTableRecord->ScaleType)
	{
	case EKMStatParameterType::Hp: baseValue = (1.f * baseValue) * (secondaryStat.GetHp() * deltaSeconds); break;
	case EKMStatParameterType::HpRps: baseValue = (1.f * baseValue) * (secondaryStat.GetHpRps() * deltaSeconds); break;
	case EKMStatParameterType::HpCurr: baseValue = (1.f * baseValue) * (secondaryStat.GetHpCurr() * deltaSeconds); break;

	case EKMStatParameterType::Sp: baseValue = (1.f * baseValue) * (secondaryStat.GetSp() * deltaSeconds); break;
	case EKMStatParameterType::SpRps: baseValue = (1.f * baseValue) * (secondaryStat.GetSpRps() * deltaSeconds); break;
	case EKMStatParameterType::SpCurr: baseValue = (1.f * baseValue) * (secondaryStat.GetSpCurr() * deltaSeconds); break;

	case EKMStatParameterType::Mp: baseValue = (1.f * baseValue) * (secondaryStat.GetMp() * deltaSeconds); break;
	case EKMStatParameterType::MpRps: baseValue = (1.f * baseValue) * (secondaryStat.GetMpRps() * deltaSeconds); break;
	case EKMStatParameterType::MpCurr: baseValue = (1.f * baseValue) * (secondaryStat.GetMpCurr() * deltaSeconds); break;

	case EKMStatParameterType::Atk: baseValue = (1.f * baseValue) * (secondaryStat.GetAtk() * deltaSeconds); break;
	case EKMStatParameterType::AtkSpd: baseValue = (1.f * baseValue) * (secondaryStat.GetAtkSpd() * deltaSeconds); break;
	case EKMStatParameterType::Dex: baseValue = (1.f * baseValue) * (secondaryStat.GetDex() * deltaSeconds); break;

	case EKMStatParameterType::Def: baseValue = (1.f * baseValue) * (secondaryStat.GetDef() * deltaSeconds); break;
	case EKMStatParameterType::Cri: baseValue = (1.f * baseValue) * (secondaryStat.GetCri() * deltaSeconds); break;
	case EKMStatParameterType::CriChance: baseValue = (1.f * baseValue) * (secondaryStat.GetCriChange() * deltaSeconds); break;
	case EKMStatParameterType::Mov: baseValue = (1.f * baseValue) * (secondaryStat.GetMov() * deltaSeconds); break;

	case EKMStatParameterType::Emp: baseValue = (1.f * baseValue) * (secondaryStat.GetEmp() * deltaSeconds); break;
	case EKMStatParameterType::EmpCurr: baseValue = (1.f * baseValue) * (secondaryStat.GetEmpCurr() * deltaSeconds); break;

	case EKMStatParameterType::Tempo: baseValue = (1.f * baseValue) * (secondaryStat.GetTempo() * deltaSeconds); break;
	case EKMStatParameterType::TempoRps: baseValue = (1.f * baseValue) * (secondaryStat.GetTempoRps() * deltaSeconds); break;
	case EKMStatParameterType::TempoCurr: baseValue = (1.f * baseValue) * (secondaryStat.GetTempoCurr() * deltaSeconds); break;
	default:break;
	}
	
	switch(EffectTableRecord->Operator)
	{
	case EKMOperatorType::Add		: return statValue + baseValue; break;
	case EKMOperatorType::Subtract	: return statValue - baseValue; break;
	case EKMOperatorType::Multiply	: return statValue * baseValue; break;
	case EKMOperatorType::Divide	: return statValue / baseValue; break;
	default: break;
	}
	return statValue;
}

void FKMSkillEffectBuffInstance::CalculateParameter(UKMCharacterInstance* target, float deltaSeconds)
{
	FKMSecondaryStat& secondaryStat = target->GetStatModifier()->GetEffectiveStat();
	switch (EffectTableRecord->Parameter)
	{
		case EKMStatParameterType::Hp :
			secondaryStat.SetHp(CalculateParameter(target, secondaryStat.GetHp(), deltaSeconds), true); break;
		case EKMStatParameterType::HpRps :
			secondaryStat.SetHpRps(CalculateParameter(target, secondaryStat.GetHpRps(), deltaSeconds), true); break;
		case EKMStatParameterType::HpCurr :
			secondaryStat.SetHpCurr(CalculateParameter(target, secondaryStat.GetHpCurr(), deltaSeconds), true); break;
		case EKMStatParameterType::Sp :
			secondaryStat.SetSp(CalculateParameter(target, secondaryStat.GetSp(), deltaSeconds), true); break;
		case EKMStatParameterType::SpRps :
			secondaryStat.SetSpRps(CalculateParameter(target, secondaryStat.GetSpRps(), deltaSeconds), true); break;
		case EKMStatParameterType::SpCurr :
			secondaryStat.SetSpCurr(CalculateParameter(target, secondaryStat.GetSpCurr(), deltaSeconds), true); break;
		case EKMStatParameterType::Mp :
			secondaryStat.SetMp(CalculateParameter(target, secondaryStat.GetMp(), deltaSeconds), true); break;
		case EKMStatParameterType::MpRps :
			secondaryStat.SetMpRps(CalculateParameter(target, secondaryStat.GetMpRps(), deltaSeconds), true); break;
		case EKMStatParameterType::MpCurr :
			secondaryStat.SetMpCurr(CalculateParameter(target, secondaryStat.GetMpCurr(), deltaSeconds), true); break;
		case EKMStatParameterType::Atk :
			secondaryStat.SetAtk(CalculateParameter(target, secondaryStat.GetAtk(), deltaSeconds), true); break;
		case EKMStatParameterType::AtkSpd :
			secondaryStat.SetAtkSpd(CalculateParameter(target, secondaryStat.GetAtkSpd(), deltaSeconds), true); break;
		case EKMStatParameterType::Dex :
			secondaryStat.SetDex(CalculateParameter(target, secondaryStat.GetDex(), deltaSeconds), true); break;
		case EKMStatParameterType::Def :
			secondaryStat.SetDef(CalculateParameter(target, secondaryStat.GetDef(), deltaSeconds), true); break;
		case EKMStatParameterType::CriChance :
			secondaryStat.SetCriChange(CalculateParameter(target, secondaryStat.GetCriChange(), deltaSeconds), true); break;
		case EKMStatParameterType::Cri :
			secondaryStat.SetCri(CalculateParameter(target, secondaryStat.GetCri(), deltaSeconds), true); break;
		case EKMStatParameterType::Mov :
			secondaryStat.SetMov(CalculateParameter(target, secondaryStat.GetMov(), deltaSeconds), true); break;
		case EKMStatParameterType::Run :
			secondaryStat.SetRun(CalculateParameter(target, secondaryStat.GetRun(), deltaSeconds), true); break;
		case EKMStatParameterType::Emp :
			secondaryStat.SetEmp(CalculateParameter(target, secondaryStat.GetEmp(), deltaSeconds), true); break;
		case EKMStatParameterType::EmpCurr :
			secondaryStat.SetEmpCurr(CalculateParameter(target, secondaryStat.GetEmpCurr(), deltaSeconds), true); break;
		case EKMStatParameterType::Tempo :
			secondaryStat.SetTempo(CalculateParameter(target, secondaryStat.GetTempo(), deltaSeconds), true); break;
		case EKMStatParameterType::TempoCurr :
			secondaryStat.SetTempoCurr(CalculateParameter(target, secondaryStat.GetTempoCurr(), deltaSeconds), true); break;
		case EKMStatParameterType::TempoRps :
			secondaryStat.SetTempoRps(CalculateParameter(target, secondaryStat.GetTempoRps(), deltaSeconds), true); break;
		default: check(0);
	}
}

void FKMSkillEffectBuffInstance::Apply(float deltaSeconds)
{
	if(!OwnerObject.IsValid())
	{
		return;
	}
	
	if (ApplyTime < EffectTableRecord->Interval)
	{
		return;
	}

	UKMCharacterInstance* targetCharacterInstance = Cast<UKMCharacterInstance>(OwnerObject->GetTypedOuter<UKMCharacterInstance>());
	check(IsValid(targetCharacterInstance));

	CalculateParameter(targetCharacterInstance, deltaSeconds);

	ApplyTime = 0.f;
}

void FKMSkillEffectBuffInstance::Tick(float deltaSeconds)
{
	if (!OwnerObject.IsValid())
	{
		return;
	}
	
	Apply(deltaSeconds);

	ApplyTime += deltaSeconds;
}
#pragma once

#include "CoreMinimal.h"
#include "KMSkillTypes.h"
#include "GameplayTagContainer.h"
#include "KMSkillHandler.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FKMAbilityEventDelegate, const class UKMAbility*, ability);

USTRUCT(Blueprintable)
struct FKMAbilityEvent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Handle;
	
	FKMAbilityEventDelegate Delegate;
	TSharedPtr<class FKMSkillInstance> SkillInstance;
};

struct FKMComboData
{
	const struct FKMTable_SkillSetRow* skillSet = nullptr;
	int32 CurrentCombo = -1;
	int32 NextCombo = 0;
	bool Pressed = false;
	
	TWeakPtr<class FKMSkillInstance> SkillInstance;
	TSharedPtr<FKMLockOnCluster> LockOnCluster;

	bool IsValid() const
	{
		return skillSet != nullptr;
	}

	void Reset()
	{
		skillSet = nullptr;
		CurrentCombo = -1;
		NextCombo = 0;
		SkillInstance = nullptr;
		LockOnCluster = nullptr;
		Pressed = false;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FKMSkillProjectileTriggerDelegate, const TSharedPtr<class FKMSkillInstance>& skillInstance);
UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMSkillHandler : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	class UKMGameObjectInstance* GetOwner() const;

	void RegisterSkillSets(const FKMTable_SkillSetRow* newSkillSet);
	
	void RegisterSkills(const TArray<FKMSkillKey>& skillKeys);
	void RegisterSkills(const TArray<FName>& skillIds);
	
	void RegisterSkill(const FKMSkillKey& newSkillKey);
	bool HasOwnedSkill(const FKMSkillKey& skillKey) const;

	bool IsReadyCooltime(const FKMSkillKey& skillKey) const;
	bool ResetCooltime(const FKMSkillKey& skillKey);

	void ClearActiveSkills();
	void ClearPassiveSkills();
	void ClearAllSkills();
	
	void ClearResisterSkillSet();

	TSharedPtr<FKMSkillInstance> UseAssistSkill(const FKMSkillKey& skillKey);
	TSharedPtr<FKMSkillInstance> UseUltimateSkill();
	TSharedPtr<FKMSkillInstance> UseCombatSkill(const TSharedPtr<class FKMLockOnCluster>& lockOnCluster);
	TSharedPtr<FKMSkillInstance> UseTechniqueSkill(const TSharedPtr<FKMLockOnCluster>& lockOnCluster);
	
	TSharedPtr<FKMSkillInstance> UseSkill(const FKMSkillKey& skillKey, TSharedPtr<class FKMLockOnCluster> lockOnCluster);

	UFUNCTION(BlueprintCallable)
	void TriggerEvent(const FGameplayTag& eventTag);
	
	bool IsUsedSkill() const;
	
	bool IsSkillAvailable(const FKMSkillKey& skillKey) const;

	const TArray<FKMSkillKey>& GetOwnedSkills() const;

	bool CanUseSkill(const FKMSkillKey& skillKey, const TSharedPtr<class FKMLockOnCluster>& lockOnCluster) const;
	void ResolveSkillCondition(const struct FKMTable_SkillRow* skillTable);

	int32 GetScoreSkill(const FKMSkillKey& skillKey) const;

	TSharedPtr<class FKMSkillInstance> GetSkillInstance(const FKMSkillKey& skillKey) const;
	bool IsSkillActivated(const FKMSkillKey& skillKey) const;
	
	TSharedPtr<class FKMSkillInstance> GetLatestActiveSkillInstance() const;
	
	float GetConditionScore(const FName& skillConditionName, TSharedPtr<class FKMLockOnCluster> lockOnCluster) const;
	float GetConditionScore(const FName& skillConditionName, const UKMCharacterInstance* targetCharacter) const;
	const struct FKMTable_SkillSetRow* EvalurateSkillSet(const TSharedPtr<class FKMLockOnCluster>& lockOnCluster) const;
	const struct FKMTable_SkillSetRow* EvalurateSkillSet(const UKMCharacterInstance* targetCharacterInstance) const;

	TArray<TSharedPtr<FKMSkillEffectInstance>> ApplyEffects(const TSharedPtr<FKMSkillInstance>& skillInstance, const FGameplayTag& eventTag, const FName& hitTag = NAME_None);

	void Tick(float deltaSeconds);

	static FKMSkillProjectileTriggerDelegate& GetProjectileTriggerDelegate()
	{
		static FKMSkillProjectileTriggerDelegate ProjectileTriggerDelegate;
		return ProjectileTriggerDelegate;
	}

	void TriggerTransitionSkillEffect(const FGameplayTag& effectTag);
	void ActivatedNextComboSkill();

	UFUNCTION(BlueprintPure)
	int32 GetSkillOverlapCount(const FName& skillId) const;
	
	int32 GetSkillEffectOverlapCount(const FName& skillEffectGroup, const FName& skillEffectId, TArray<TSharedPtr<FKMSkillEffectInstance>>* outSkillEffects = nullptr) const;
	
protected:
	void OnAddAbilityInstance(TSharedPtr<class FKMAbilityInstanceBase> abilityInstance);
	void OnRemoveAbilityInstance(TSharedPtr<class FKMAbilityInstanceBase> abilityInstance);
	
	TSharedPtr<FKMSkillInstance> UseSkillInternal(const TSharedPtr<class FKMSkillInstance>& newSkillInstance);

	void ResetSkillEffect(const FName& skillEffectId);
	
	void RemoveSkillEffect(const FName& skillEffectId);
	
	template<typename _TL>
	void RemoveForceAbility(const TArray<TSharedPtr<_TL>>& abilityInstances, bool bCancel = false);
	
	TSharedPtr<FKMSkillEffectInstance> ApplyEffectInternal(const TSharedPtr<class FKMSkillInstance>& skillInstance, const FName& effectName);
	
	TSharedPtr<FKMSkillInstance> UseSkillInternal(class UKMCharacterInstance* ownerCharacterInstance, const TSharedPtr<class FKMSkillInstance>& newSkillInstance);

	bool UpdateAbilitiy(const TSharedPtr<FKMAbilityInstanceBase>& abilityInstance, float deltaSeconds);
	
	template<typename _TLKey, typename _TLValue>
	void UpdateAbilities(TMap<_TLKey, TSharedPtr<_TLValue>>& abilityInstances, float deltaSeconds);

protected:
	TSet<TSharedPtr<FKMAbilityInstanceBase>> AbilityEvents;
	
	int32 AbilityNextHandle = 0;
	
	TArray<FKMSkillKey> OwnedSkills;
	TArray<const FKMTable_SkillSetRow*> OwnenSkillSets;
	
	TMap<int32, TSharedPtr<class FKMSkillInstance>> SkillInstances;
	TMap<int32, TSharedPtr<class FKMSkillEffectInstance>> EffectInstances;
	TMap<FKMSkillKey, TSharedPtr<class FKMAbilityInstanceCooltime>> CooltimeInstances;
	TArray<TSharedPtr<FKMAbilityInstanceBase>> PendingNewAbilities;
		
	int32 LastAbilityUniqueId = 0;

	uint8 AbilityMask = 0;

	FKMComboData ComboData;

	TWeakPtr<class FKMSkillInstance> LatestActiveSkillInstance;
};
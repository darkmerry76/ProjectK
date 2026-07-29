#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/KMDefine.h"
#include "Stat/KMSecondaryBaseStat.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMSkillTypes.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct KMGAME_API FKMAssistSkillData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FName> Tags;
};

struct KMGAME_API FKMSkillProjectileArrivalData : public FKMProjectileArrivalData
{
	FKMSkillProjectileArrivalData(TSharedPtr<class FKMSkillInstance> skillInstance) : SkillInstance(skillInstance) { };
	
	virtual FName GetType() const { return TypeName(); }
	static FName TypeName() { return TEXT("FKMSkillProjectileArrivalData"); }

	TSharedPtr<class FKMSkillInstance> SkillInstance;
};

class KMGAME_API FKMAbilityInstanceBase : public FGCObject, public TSharedFromThis<FKMAbilityInstanceBase>
{
public:
	virtual FName GetType() const { return FKMAbilityInstanceBase::TypeName(); }
	static FName TypeName() { return TEXT("FKMAbilityInstanceBase"); }
	
	FKMAbilityInstanceBase(UObject* ownerObject);
	FKMAbilityInstanceBase(const FKMAbilityInstanceBase& sourceInstanceBase);
	FKMAbilityInstanceBase();

	virtual bool IsA(const FName& typeName) const
	{
		if (typeName == FKMAbilityInstanceBase::TypeName())
		{
			return true;
		}
		return false;
	}

	virtual ~FKMAbilityInstanceBase() override;

	virtual FString GetReferencerName() const override { return GetType().ToString(); }

	virtual void Enter() { };
	virtual void Leave() { };
	
	virtual float GetElipsedTime() const;
	virtual void Tick(float deltaSeconds) { };
	virtual bool IsComplete() const { return false; };

	virtual void PostTick(float deltaSeconds);

	virtual void OnTriggerEvent(const FGameplayTag& eventTag) { };

	void SetAbility(class UKMAbility* newAbility) { Ability = newAbility; }
	class UKMAbility* GetAbility() const { return Ability; };

	void Suspend() { bIsSuspend = true; }
	void Resume() { bIsEnabled = false; }

	void SetEnable(bool newIsEnable) { bIsEnabled = newIsEnable; };
	bool IsEnabled() const { return bIsEnabled; };
	
protected:
	virtual void ResetElipsedTime();
	
protected:
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
	TWeakObjectPtr<UObject> OwnerObject = nullptr;

	TObjectPtr<class UKMAbility> Ability;

private:
	float ElipsedTime = 0.f;
	
protected:
	float TimeScale = 1.f;
	bool Copied = false;

	bool bIsSuspend = false;
	bool bIsEnabled = true;
};

class KMGAME_API FKMAbilityInstanceCooltime : public FKMAbilityInstanceBase 
{
public:
	FKMAbilityInstanceCooltime(UObject* ownerObject, const FKMSkillKey& skillKey);

	virtual FName GetType() const override { return FKMAbilityInstanceCooltime::TypeName(); }
	static FName TypeName() { return TEXT("FKMAbilityInstanceCooltime"); }

	virtual bool IsReady() const;
	virtual void Reset();

	virtual float GetElipsedTime() const override;

	void ForceReady();
	
	FKMSkillKey SkillKey;
};

class KMGAME_API FKMAbilityPassiveSkillInstigator : public FKMAbilityInstanceBase 
{
public:
	FKMAbilityPassiveSkillInstigator(UObject* ownerObject, const FKMSkillKey& skillKey);

	virtual FName GetType() const override { return FKMAbilityPassiveSkillInstigator::TypeName(); }
	static FName TypeName() { return TEXT("FKMAbilityPassiveSkillInstigator"); }
};

enum class EKMSkillState
{
	Start,
	Loop,
	End,
	Cancel,
	Interrupted
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FKMSkillEffectTriggerDelegate, const FGameplayTag& eventTag, const TSharedPtr<class FKMSkillInstance>& skillInstance);
DECLARE_MULTICAST_DELEGATE_TwoParams(FKMSkillTriggerDelegate, const FGameplayTag& eventTag, const TSharedPtr<class FKMSkillInstance>& skillInstance);

class KMGAME_API FKMSkillInstance : public FKMAbilityInstanceBase
{
public:
	FKMSkillInstance(UObject* ownerObject, const FKMSkillKey& skillKey);
	FKMSkillInstance(const FKMSkillInstance& sourceSkillInstance);

	virtual FName GetType()  const override { return FKMSkillInstance::TypeName(); }
	static FName TypeName() { return TEXT("FKMSkillInstance"); }

	virtual bool IsA(const FName& typeName) const override
	{
		if (typeName == FKMSkillInstance::TypeName())
		{
			return true;
		}
		return FKMAbilityInstanceBase::IsA(typeName);
	}

	virtual bool IsComplete() const override;
	virtual void Tick(float deltaSeconds) override;
	virtual void Enter() override;

	void AddAssistSkill(TSharedPtr<class FKMSkillInstance> assistSkillInstance);

	virtual void OnTriggerEvent(const FGameplayTag& eventTag) override;
	void SetEffectTriggers(TArray<TSharedPtr<FKMSkillEffectTriggerData>>* newEffectTriggers);

	void RequestEnd();

	void SetForceComplete(bool bForceComplete) { bIsForceComplete = bForceComplete; }
	bool GetForceComplete() const { return bIsForceComplete; }

protected:
	bool CanTransitionTo(EKMSkillState newState) const;
	void TransitionTo(EKMSkillState newState);
	void TransitionTo_Reseved(EKMSkillState newState);
	void OnStateEnter(EKMSkillState newState);
	
public:
	FKMSkillKey SkillKey;
	int32 Caster = INDEX_NONE;
	TSharedPtr<class FKMLockOnCluster> Target;

	TMap<FName, FName> Tags;

	FKMSkillEffectTriggerDelegate SkillEffectTriggerDelegate;
	FKMSkillTriggerDelegate SkillTriggerDelegate;

protected:
	void NotifyEffectTrigger(float prevTime, float nextTime);

	TArray<TSharedPtr<FKMSkillEffectTriggerData>>* EffectTriggers;
	TArray<int32> EffectTriggerIndices;
	TArray<TSharedPtr<FKMSkillInstance>> AssistSkills;

	bool bIsForceComplete = false;

	EKMSkillState State = EKMSkillState::Start;
	EKMSkillState ResevedState = EKMSkillState::Start;
};

class KMGAME_API FKMAssistSkillInstance : public FKMSkillInstance
{
public:
	FKMAssistSkillInstance(UObject* ownerObject, const FKMSkillKey& skillKey);

	virtual FName GetType()  const override { return FKMAssistSkillInstance::TypeName(); }
	static FName TypeName() { return TEXT("FKMAssistSkillInstance"); }

	virtual bool IsA(const FName& typeName) const override
	{
		if (typeName == FKMAssistSkillInstance::TypeName())
		{
			return true;
		}
		return FKMSkillInstance::IsA(typeName);
	}

	virtual bool IsComplete() const override;
	virtual void Tick(float deltaSeconds) override;
	virtual void Enter() override;

	virtual void OnTriggerEvent(const FGameplayTag& eventTag) override;

	bool ForceComplete = false;

protected:
	void NotifyEffectTrigger(float prevTime, float nextTime);
};

class KMGAME_API FKMSkillEffectInstance : public FKMAbilityInstanceBase
{
public:
	FKMSkillEffectInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	virtual FName GetType() const override { return FKMSkillEffectInstance::TypeName(); }
	virtual bool IsA(const FName& typeName) const override
	{
		if (typeName == FKMSkillEffectInstance::TypeName())
		{
			return true;
		}
		return FKMAbilityInstanceBase::IsA(typeName);
	}
	static FName TypeName() { return TEXT("FKMSkillEffectInstance"); }

	virtual bool IsComplete() const override;
	virtual void Enter() override;
	virtual void Leave() override;

	const struct FKMTable_SkillEffectRow* GetEffectTableRecord() const;

	void SetForceComplete(bool bForceComplete) { bIsForceComplete = bForceComplete; };

	TSharedPtr<FKMSkillInstance> GetOwnerSkillInstance() const { return OwnerSkillInstance; };

	TMap<TWeakObjectPtr<class UKMStatModifierBase>, TWeakObjectPtr<UObject>> GetUsedEffectAbilities() { return UsedEffectAbilities; }; 

protected:
	float ApplyTime = 0.f;
	bool bIsForceComplete = false;

	TSharedPtr<FKMSkillInstance> OwnerSkillInstance;
	const struct FKMTable_SkillEffectRow* EffectTableRecord;
	
	TMap<TWeakObjectPtr<class UKMStatModifierBase>, TWeakObjectPtr<UObject>> UsedEffectAbilities; 
};

class KMGAME_API FKMSkillEffectDamageInstance : public FKMSkillEffectInstance
{
public:
	FKMSkillEffectDamageInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	void Classification();

	virtual void Enter() override;
	virtual void Leave() override;
	
	virtual FName GetType() const override { return FKMSkillEffectDamageInstance::TypeName(); }
	static FName TypeName() { return TEXT("FKMSkillEffectDamageInstance"); }
	virtual bool IsA(const FName& typeName) const override
	{
		if (typeName == FKMSkillEffectDamageInstance::TypeName())
		{
			return true;
		}
		return FKMSkillEffectInstance::IsA(typeName);
	}

	virtual void Tick(float deltaSeconds) override;
};

class KMGAME_API FKMSkillEffectAbnormalInstance : public FKMSkillEffectInstance
{
public:
	FKMSkillEffectAbnormalInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	void Classification();

	virtual void Enter() override;
	virtual void Leave() override;
	
	virtual FName GetType() const override { return FKMSkillEffectAbnormalInstance::TypeName(); }
	static FName TypeName() { return TEXT("FKMSkillEffectAbnormalInstance"); }
	virtual bool IsA(const FName& typeName) const override
	{
		if (typeName == FKMSkillEffectAbnormalInstance::TypeName())
		{
			return true;
		}
		return FKMSkillEffectInstance::IsA(typeName);
	}

	virtual void Tick(float deltaSeconds) override;
};

class KMGAME_API FKMSkillEffectBuffInstance : public FKMSkillEffectInstance
{
public:
	FKMSkillEffectBuffInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	void Classification();

	virtual void Enter() override;
	virtual void Leave() override;
	
	virtual FName GetType() const override { return FKMSkillEffectBuffInstance::TypeName(); }
	static FName TypeName() { return TEXT("FKMSkillEffectBuffInstance"); }
	virtual bool IsA(const FName& typeName) const override
	{
		if (typeName == FKMSkillEffectBuffInstance::TypeName())
		{
			return true;
		}
		return FKMSkillEffectInstance::IsA(typeName);
	}

	virtual void Tick(float deltaSeconds) override;
	void Apply(float deltaSeconds);

	double CalculateParameter(class UKMCharacterInstance* target, double statValue, float deltaSeconds) const;
	
	void CalculateParameter(class UKMCharacterInstance* target, float deltaSeconds);
};
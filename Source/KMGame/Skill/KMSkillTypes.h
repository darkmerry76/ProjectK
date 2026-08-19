#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/KMDefine.h"
#include "KMSkillTypes.generated.h"

#define IMPLEMENT_ABILITY_TYPEINFO(ClassName, ParentClass)			\
	public:															\
	virtual FName GetType() const override { return TypeName(); }	\
	static FName TypeName() { return TEXT(#ClassName); }			\
	virtual bool IsA_Internal(const FName& otherTypeName) const override \
	{                                                               \
		if (ClassName::GetType() == otherTypeName)                  \
		{                                                           \
			return true;                                            \
		}                                                           \
		return ParentClass::IsA_Internal(otherTypeName);            \
	}

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

	virtual ~FKMAbilityInstanceBase() override;

	template<typename _TL>
	bool IsA() const
	{
		return IsA_Internal(_TL::TypeName());
	}

	virtual FString GetReferencerName() const override { return GetType().ToString(); }

	virtual void Init();
	virtual void Reset() { };
	virtual void Enter();
	virtual void Leave();
	
	virtual float GetElipsedTime() const;
	virtual void Tick(float deltaSeconds) { };
	virtual bool IsComplete() const { return false; };

	virtual void PostTick(float deltaSeconds);

	virtual void OnTriggerEvent(const FGameplayTag& eventTag);
	virtual void InitAbilityAsset(bool bAutoActivate = false);
	virtual void DeactivatedAbility(bool bCancel = false);
	
	void Suspend() { bIsSuspend = true; }
	void Resume() { bIsEnabled = false; }

	void SetEnable(bool newIsEnable) { bIsEnabled = newIsEnable; };
	bool IsEnabled() const { return bIsEnabled; };

	bool IsEnter() const { return bIsEnter; };

	class UKMAbility* AddAbillityAsset(const FName& pDAKey);
	void RemoveAbilityAsset(class UKMAbility* ability);
	void RemoveAllAbilityAsset(bool bDeactivateEvent = true, bool bCancel = false);
	
	const TArray<TObjectPtr<class UKMAbility>> GetAbilitieAssets() const;

	virtual void SetForceComplete(bool bForceComplete) { bIsForceComplete = bForceComplete; }
	bool GetForceComplete() const { return bIsForceComplete; }

	void Cancel() { bIsCancel = true; };

	int32 UniqueId = INDEX_NONE;
	
protected:
	virtual void ResetElipsedTime();
	virtual bool IsA_Internal(const FName& otherTypeName) const
	{
		if (GetType() != otherTypeName)
		{
			return false;
		}
		return true;
	}
	
protected:
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
	TWeakObjectPtr<UObject> OwnerObject = nullptr;
	
	TArray<TObjectPtr<class UKMAbility>> AbilitieAssets;

private:
	float ElipsedTime = 0.f;
	
protected:
	float TimeScale = 1.f;
	bool Copied = false;

	bool bIsSuspend = false;
	bool bIsEnabled = true;
	bool bIsEnter = false;
	bool bIsForceComplete = false;
	bool bIsCancel = false;
};

class KMGAME_API FKMAbilityInstanceCooltime : public FKMAbilityInstanceBase 
{
	IMPLEMENT_ABILITY_TYPEINFO(FKMAbilityInstanceCooltime, FKMAbilityInstanceBase)
public:
	FKMAbilityInstanceCooltime(UObject* ownerObject, const FKMSkillKey& skillKey);

	virtual bool IsReady() const;
	virtual void Reset();

	virtual float GetElipsedTime() const override;

	void ForceReady();
	
	FKMSkillKey SkillKey;
};

class KMGAME_API FKMAbilityPassiveSkillInstigator : public FKMAbilityInstanceBase 
{
	IMPLEMENT_ABILITY_TYPEINFO(FKMAbilityPassiveSkillInstigator, FKMAbilityInstanceBase)
public:
	FKMAbilityPassiveSkillInstigator(UObject* ownerObject, const FKMSkillKey& skillKey);
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
	IMPLEMENT_ABILITY_TYPEINFO(FKMSkillInstance, FKMAbilityInstanceBase)
public:
	FKMSkillInstance(UObject* ownerObject, const FKMSkillKey& skillKey);
	FKMSkillInstance(const FKMSkillInstance& sourceSkillInstance);

	virtual bool IsComplete() const override;
	virtual void Tick(float deltaSeconds) override;
	virtual void Enter() override;

	void AddAssistSkill(TSharedPtr<class FKMSkillInstance> assistSkillInstance);

	virtual void OnTriggerEvent(const FGameplayTag& eventTag) override;
	void RequestEnd();

protected:
	bool CanTransitionTo(EKMSkillState newState) const;
	void TransitionTo(EKMSkillState newState);
	void TransitionTo_Reseved(EKMSkillState newState);
	void OnStateEnter(EKMSkillState newState);

	virtual void InitAbilityAsset(bool bAutoActivate = false) override;
	virtual void DeactivatedAbility(bool bCancel = false) override; 
	
public:
	FKMSkillKey SkillKey;
	int32 Caster = INDEX_NONE;
	TSharedPtr<class FKMLockOnCluster> Target;
	TMap<FName, FName> Tags;

protected:
	TArray<TSharedPtr<FKMSkillInstance>> AssistSkills;

	EKMSkillState State = EKMSkillState::Start;
	EKMSkillState ResevedState = EKMSkillState::Start;
};

class KMGAME_API FKMAssistSkillInstance : public FKMSkillInstance
{
	IMPLEMENT_ABILITY_TYPEINFO(FKMAssistSkillInstance, FKMSkillInstance)
public:
	FKMAssistSkillInstance(UObject* ownerObject, const FKMSkillKey& skillKey);

	virtual bool IsComplete() const override;
	virtual void Tick(float deltaSeconds) override;
	virtual void Enter() override;

	virtual void OnTriggerEvent(const FGameplayTag& eventTag) override;
};

class KMGAME_API FKMSkillEffectInstance : public FKMAbilityInstanceBase
{
	IMPLEMENT_ABILITY_TYPEINFO(FKMSkillEffectInstance, FKMAbilityInstanceBase)
public:
	FKMSkillEffectInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	virtual void Reset() override;
	virtual bool IsComplete() const override;
	virtual void Enter() override;
	virtual void Leave() override;
	
	const struct FKMTable_SkillEffectRow* GetEffectTableRecord() const;
	TSharedPtr<FKMSkillInstance> GetOwnerSkillInstance() const { return OwnerSkillInstance; };

	virtual void SetForceComplete(bool bForceComplete) override;

protected:
	virtual void InitAbilityAsset(bool bAutoActivate = false) override;
	virtual void DeactivatedAbility(bool bCancel = false) override;
	virtual void OnTriggerEvent(const FGameplayTag& eventTag) override;

protected:
	float ApplyTime = 0.f;

	TSharedPtr<FKMSkillInstance> OwnerSkillInstance;
	const struct FKMTable_SkillEffectRow* EffectTableRecord;
};

class KMGAME_API FKMSkillEffectDamageInstance : public FKMSkillEffectInstance
{
	IMPLEMENT_ABILITY_TYPEINFO(FKMSkillEffectDamageInstance, FKMSkillEffectInstance)
public:
	FKMSkillEffectDamageInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	void Classification();

	virtual void Enter() override;
	virtual void Leave() override;
	virtual void Tick(float deltaSeconds) override;
};

class KMGAME_API FKMSkillEffectAbnormalInstance : public FKMSkillEffectInstance
{
	IMPLEMENT_ABILITY_TYPEINFO(FKMSkillEffectAbnormalInstance, FKMSkillEffectInstance)
public:
	FKMSkillEffectAbnormalInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	void Classification();

	virtual void Enter() override;
	virtual void Leave() override;
	virtual void Tick(float deltaSeconds) override;
};

class KMGAME_API FKMSkillEffectBuffInstance : public FKMSkillEffectInstance
{
	IMPLEMENT_ABILITY_TYPEINFO(FKMSkillEffectBuffInstance, FKMSkillEffectInstance)
public:
	FKMSkillEffectBuffInstance(UObject* ownerObject, const struct FKMTable_SkillEffectRow* effectTableRecord, const TSharedPtr<FKMSkillInstance> ownerSkillInstance);

	void Classification();

	virtual void Enter() override;
	virtual void Leave() override;

	virtual void Tick(float deltaSeconds) override;
	void Apply(float deltaSeconds);

	double CalculateParameter(class UKMCharacterInstance* target, double statValue, float deltaSeconds) const;
	
	void CalculateParameter(class UKMCharacterInstance* target, float deltaSeconds);
};
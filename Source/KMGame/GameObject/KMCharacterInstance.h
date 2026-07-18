#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "Stat/KMSecondaryBaseStat.h"
#include "KMGameObjectInstance.h"
#include "Notify/KMAnimNotifyState_Hit.h"
#include "Skill/KMSkillTypes.h"
#include "Stat/KMStatModifierBase.h"
#include "System/EMTickerSubsystem.h"
#include "Templates/SubclassOf.h"
#include "KMCharacterInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMCharacterDeathDelegate, class UKMCharacterInstance* character);
DECLARE_MULTICAST_DELEGATE_FourParams(FKMCharacterStatChangeDelegate, class UKMCharacterInstance* character, EKMStatFactorType factorType, float prevValue, float newValue);
DECLARE_MULTICAST_DELEGATE_TwoParams(FKMCharacterInflictDelegate, int32 comboCount, class UKMCharacterInstance* victimCharacter);
DECLARE_MULTICAST_DELEGATE_OneParam(FKMCharacterDamageDelegate, const FKMDamageEvent& newDamageEvent);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FKMCharacterCommbatMessageDelegate, const class UKMCharacterInstance* character, EKMCommbatMessageType messageType, const FString& newMessage);

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMCharacterInstance : public UKMGameObjectInstance
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual FName GetRecordKey() const;
	virtual FName GetRecordStatKey() const;
	
	void SetCharacter(class AKMCharacter* newCharacter);

	UFUNCTION(BlueprintPure)
	class AKMCharacter* GetCharacter() const;

	UFUNCTION(BlueprintCallable)
	void SetBeastTableId(FName beatId);

	UFUNCTION(BlueprintPure)
	FName GetBeatId() const;

	UFUNCTION(BlueprintCallable)
	void ToggleBeast();
	
	UFUNCTION(BlueprintCallable)
	void RevertFromBest();

	UFUNCTION(BlueprintNativeEvent)
	void OnRevertFromBest();
	
	UFUNCTION(BlueprintCallable)
	void TransformToBeast();

	UFUNCTION(BlueprintNativeEvent)
	void OnTransformToBeast();

	UFUNCTION(BlueprintCallable)
	void SpawnBeast();

	virtual void BeginPlay() override;
	virtual void EndPlay() override;

	virtual void UpdateTransform();
	virtual void Tick(float deltaSeconds) override;

	void SetTable(const struct FKMTable_CharacterRow* newTable);
	const struct FKMTable_CharacterRow* GetTable() const;

	void SetDepthSort(float newDepthSort);
	float GetDepthSort() const;

	void SetLocation(const FVector& newLocation);
	void SetScale3D(const FVector& newScale3D);
	void SetRotation(const FRotator& newRotation);

	void SetTransform(const FTransform& newTransform);
	const FTransform& GetTransform() const;
	
	UFUNCTION(BlueprintCallable)
	bool UseSkill(const FName skillName, int32 skillLevel);

	UFUNCTION(BlueprintCallable)
	void UseSkillDash(float dashDirection);

	UFUNCTION(BlueprintCallable)
	void SetCharacterDirectionVisual(float direction, bool bForceRotate = false);
	
	UFUNCTION(BlueprintCallable)
	void SetCharacterDirection(float direction, bool bForceRotate = false);

	UFUNCTION(BlueprintPure)
	float GetCharacterDirection() const;

	UFUNCTION(BlueprintPure)
	bool IsBeast() const;
	
	UFUNCTION(Blueprintable)
	void Run();

	UFUNCTION(BlueprintPure)
	bool IsRun() const;

	UFUNCTION(Blueprintable)
	void Walk();

	UFUNCTION(BlueprintPure)
	bool IsWalk() const;
	
	UFUNCTION(BlueprintCallable)
	bool UseSkillParam(const FName skillName, int32 skillLevel, const TArray<FKMAssistSkillData> assistSkillData);

	void UseCombatSkill();
	void UseUltimateSkill();
	void UseTechniqueSkill();
	bool UseParrySkill();

	bool UseGuardSkill();
	bool UseGuardSkill_Release();

	UFUNCTION(BlueprintCallable)
	void AddAggroTarget(class UKMCharacterInstance* attacker);

	UFUNCTION(BlueprintPure)
	const class UKMCharacterInstance* GetBestAggroTarget() const;

	void OnStatChange(EKMStatFactorType factorType, float prevValue, float newValue);
	void BroadCastDamageEvent(const FKMDamageEvent& newDamageEvent);
	
	virtual void OnDeath();

	UFUNCTION(BlueprintPure)
	virtual bool IsDead() const;

	virtual bool CanLockOn() const { return false; };
	virtual bool CanBeTargeted() const { return true; };
	virtual bool CanReceiveReward() const { return true; }

	class UKMSkillHandler* GetSkillHandler() const;
	class UKMStatModifierBase* GetStatModifier() const;

	void StartForceMove(const float& newDirection);

	UFUNCTION(BlueprintCallable)
	void Inflict(class UKMCharacterInstance* victimCharacter);

	UFUNCTION(BlueprintCallable)
	void Stiff(float duration = 0.2f);

	void Hit(UKMCharacterInstance* attackerCharacterInstance, TSharedPtr<class FKMSkillInstance> latestSkillInstance, const FVector& hitClosestPoint);
	
	void OnStiffRelease();

	UFUNCTION(BlueprintCallable)
	float GetMoveAccelate() const;
	
	void OnSensorResult(const TArray<AActor*>& resultActors);
	
	static TArray<FKMSkillKey> DefaultPassiveSkills;
	
	virtual void RemoveGameplayTag(FGameplayTag Tag) override;

	UFUNCTION(BlueprintCallable)
	void HitCheckClear();

	UFUNCTION(BlueprintCallable)
	void BoxHitImpact(const FTransform& orientationTransform, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter);

protected:
	void HitCollection(class AActor* hitActor,const FTransform& orientationTransform);
	
	virtual void OnAddGameplayTag_Implementation(const FGameplayTag& newTag) override;
	virtual void OnRemoveGameplayTag_Implementation(const FGameplayTag& removedTag) override;

	UFUNCTION()
	void OnUpdatePawnThrowOverlapResults(const TArray<FHitResult>& hitResults);
	
private:
	void ShowDamage(EKMStatFactorType factorType, int32 damage);

public:
	static FKMCharacterDeathDelegate& GetCharacterDeathDelegate()
	{
		static FKMCharacterDeathDelegate deathDelegate;
		return deathDelegate;
	}

	static FKMCharacterStatChangeDelegate& GetGlobalStatChangeEvent()
	{
		static FKMCharacterStatChangeDelegate statChangeEvent;
		return statChangeEvent;
	}

	FKMCharacterDamageDelegate& GetDamageDelegate()
	{
		return DamageDelegate;
	}

	FKMCharacterStatChangeDelegate& GetStatChangeEvent()
	{
		return StatChangeEvent;
	}

	FKMCharacterInflictDelegate& GetInflictDelegate()
	{
		return InflictDelegate;
	}

	FKMCharacterCommbatMessageDelegate& GetCombatMessageDelegate()
	{
		return CombatMessageDelegate;
	}
	
protected:
	FKMCharacterInflictDelegate InflictDelegate;
	FKMCharacterStatChangeDelegate StatChangeEvent;
	FKMCharacterDamageDelegate DamageDelegate;
	FKMCharacterCommbatMessageDelegate CombatMessageDelegate;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<class AKMCharacter> Character = nullptr;

	UPROPERTY(Transient)
	TWeakObjectPtr<class AKMCharacterBeast> Beast = nullptr;

	const struct FKMTable_CharacterRow* Table = nullptr;

	TSharedPtr<class FKMTimingParry> TimingParry;
	TSharedPtr<class FKMTimingCancel> TimingCancel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UKMSensor> SensorClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<class UKMSensor> SensorInstance;

	FTimerHandle SensorHandle;

	TSharedPtr<FKMLockOnCluster> LockonTarget;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "CharacterInstance", meta=(AllowPrivateAccess=true))
	TSubclassOf<class UKMSkillHandler> SkillHandlerClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "CharacterInstance", meta=(AllowPrivateAccess=true))
	TObjectPtr<class UKMSkillHandler> SkillHandler = nullptr;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "CharacterInstance", meta=(AllowPrivateAccess=true))
	TSubclassOf<class UKMStatModifierBase> StatModifierClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "CharacterInstance", meta=(AllowPrivateAccess=true))
	TObjectPtr<class UKMStatModifierBase> StatModifier;

	UPROPERTY()
	TSet<TWeakObjectPtr<UKMCharacterInstance>> AggroTarget;

	UPROPERTY(EditAnywhere, Transient)
	TSet<AActor*> ThrowOverlapActors;
	
	FTransform Transform;

	FName BeastId = NAME_None;
	
	float DepthSort = 0.f;
	bool bIsRun = false;

	float MoveAccelate = 0.f;
	float CharacterDirection = 1.f;
	
	FTimerHandle StiffTimerHandle;
	
	FEMTickerHandle GoodCancelTimerHandle;
	FEMTickerHandle GreatCancelTimerHandle;
	FEMTickerHandle PerfectCancelTimerHandle;
	int32 ComboCount = 0;

	FKMHitCheckData HitCheckData;

	bool bIsBeast = false;
};
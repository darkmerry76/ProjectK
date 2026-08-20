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
DECLARE_MULTICAST_DELEGATE_ThreeParams(FKMCharacterSkillMessageDelegate, const class UKMCharacterInstance* character, TSharedPtr<FKMAbilityInstanceBase> abilityInstance, const FString& prefixMessage);

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

	FName GetCharacterId() const;

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
	void SetCharacterDirectionVisual(float direction, bool bForceRotate = false, class USkeletalMeshComponent* otherSkeletalMeshComp = nullptr);
	
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

	UFUNCTION(BlueprintPure)
	TSubclassOf<class UCameraShakeBase> GetCameraShakeByPowerType(EKMDamagePowerType powerType) const;
	
	UFUNCTION(BlueprintCallable)
	void Inflict(class UKMCharacterInstance* victimCharacter);

	UFUNCTION(BlueprintCallable)
	void Stiff(float duration = 0.2f, bool bReset = false);

	UFUNCTION(BlueprintCallable)
	void ShakeRoot(float newDistance, float newFrequency, float newDuration = 0.2f);

	void Hit(UKMCharacterInstance* attackerCharacterInstance, TSharedPtr<class FKMSkillInstance> latestSkillInstance, const FVector& hitClosestPoint, const FName& hitTag);
	
	void OnStiffRelease();

	UFUNCTION(BlueprintCallable)
	float GetMoveAccelate() const;
	
	void OnSensorResult(const TArray<AActor*>& resultActors);
	
	virtual void RemoveGameplayTag(FGameplayTag Tag) override;

	void HitCheckClear();
	void BoxHitImpact(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance,
		const FTransform& prevOrientationTransform, const FTransform& orientationTransform,
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter, const FName& hitTag);

	void SphereHitImpact(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance,
		const FTransform& startOrientationTransform, const FTransform& endOrientationTransform,
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter, const FName& hitTag);

	UFUNCTION(BlueprintCallable)
	void SetTimeDilation(const FName& layerName, float newTimeDilation = 1.f);

	UFUNCTION(BlueprintPure)
	float GetTimeDilation() const;

	UFUNCTION(BlueprintCallable)
	void RemoveTimeDilation(const FName& layerName);

protected:
	void HitCollection(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance,
		AActor* hitActor,const FVector& hitLocation, const FVector& hitNormal, const FName& hitTag);

	void HitCollections(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance, TArray<FHitResult> hitResults, UClass* actorClassFilter, const FName& hitTag);
	
	virtual void OnAddGameplayTag_Implementation(const FGameplayTag& newTag) override;
	virtual void OnRemoveGameplayTag_Implementation(const FGameplayTag& removedTag) override;

	UFUNCTION()
	void OnUpdatePawnThrowOverlapResults(const TArray<FHitResult>& hitResults);

	void ChangeSkillSet(const FName& ownerId);
	
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

	static FKMCharacterSkillMessageDelegate& GetSkillMessageDelegate()
	{
		static FKMCharacterSkillMessageDelegate newSkillMessageDelegate;
		return newSkillMessageDelegate;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EKMDamagePowerType, TSubclassOf<class UCameraShakeBase>> InflictCameraShakes;

	UPROPERTY(EditAnywhere, Transient)
	TSet<AActor*> ThrowOverlapActors;
	
	FTransform Transform;

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

	FName BeastId = NAME_None;
	const struct FKMTable_BeastRow* BeastTableRow = nullptr;
	const struct FKMTable_BaseStat_BeastRow* BeastStatTableRow = nullptr;
	
	bool bIsBeast = false;

	EKMDamagePowerType InflectPowerType = EKMDamagePowerType::None;
	EKMDamagePowerType HitPowerType = EKMDamagePowerType::None;

	TMap<FName, float> TimeDilations;
};
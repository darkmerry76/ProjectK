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

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMCharacterInstance : public UKMGameObjectInstance
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	class AKMCharacter* GetCharacter() const;

	UFUNCTION(BlueprintCallable)
	void SetBeastTableId(FName beastId);

	UFUNCTION(BlueprintPure)
	FName GetBeatId() const;

	const struct FKMTable_Object_CharacterRow* GetCharacterTable() const;

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
	
	virtual FName GetTableId() const override;

	void SetDepthSort(float newDepthSort);
	float GetDepthSort() const;

	void SetLocation(const FVector& newLocation);
	void SetScale3D(const FVector& newScale3D);
	void SetRotation(const FRotator& newRotation);

	void SetTransform(const FTransform& newTransform);
	virtual FTransform GetTransform() const override;
	
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

	virtual void OnStatChange(EKMStatFactorType factorType, float prevValue, float newValue) override;
	virtual void BroadCastDamageEvent(const FKMDamageEvent& newDamageEvent) override;
	virtual void OnDeath() override;
	virtual bool IsDead() const override;
	virtual bool IsAir() const override;
	
	virtual bool CanLockOn() const { return false; };
	virtual bool CanBeTargeted() const { return true; };
	virtual bool CanReceiveReward() const { return true; }
	
	void StartForceMove(const float& newDirection);

	UFUNCTION(BlueprintPure)
	TSubclassOf<class UCameraShakeBase> GetCameraShakeByPowerType(EKMDamagePowerType powerType) const;
	
	UFUNCTION(BlueprintCallable)
	void ShakeRoot(float newDistance, float newFrequency, float newDuration = 0.2f);

	virtual void Hit(UKMGameObjectInstance* attackerGameObjectInstance, TSharedPtr<class FKMSkillInstance> latestSkillInstance, const FVector& hitClosestPoint, const FName& hitTag) override;
	
	UFUNCTION(BlueprintCallable)
	float GetMoveAccelate() const;
	
	void OnSensorResult(const TArray<AActor*>& resultActors);
	
	virtual void RemoveGameplayTag(FGameplayTag Tag) override;

protected:
	virtual void HitCollection(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance,
		AActor* hitActor,const FVector& hitLocation, const FVector& hitNormal, const FName& hitTag) override;
	
	virtual void OnAddGameplayTag_Implementation(const FGameplayTag& newTag) override;
	virtual void OnRemoveGameplayTag_Implementation(const FGameplayTag& removedTag) override;
	virtual void Inflict(class UKMGameObjectInstance* victimGameObject) override;
	virtual void ShowDamage(EKMStatFactorType factorType, int32 damage) override;
	
	UFUNCTION()
	void OnUpdatePawnThrowOverlapResults(const TArray<FHitResult>& hitResults);

	void ChangeSkillSet(const FName& ownerId);

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<class AKMCharacterBeast> Beast = nullptr;

	TSharedPtr<class FKMTimingParry> TimingParry;
	TSharedPtr<class FKMTimingCancel> TimingCancel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UKMSensor> SensorClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<class UKMSensor> SensorInstance;

	FTimerHandle SensorHandle;

	TSharedPtr<FKMLockOnCluster> LockonTarget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EKMDamagePowerType, TSubclassOf<class UCameraShakeBase>> InflictCameraShakes;

	UPROPERTY(EditAnywhere, Transient)
	TSet<AActor*> ThrowOverlapActors;
	
	FTransform Transform;

	float DepthSort = 0.f;
	bool bIsRun = false;

	float MoveAccelate = 0.f;
	float CharacterDirection = 1.f;
	
	FEMTickerHandle GoodCancelTimerHandle;
	FEMTickerHandle GreatCancelTimerHandle;
	FEMTickerHandle PerfectCancelTimerHandle;
	
	int32 ComboCount = 0;	

	FName BeastId = NAME_None;
	const struct FKMTable_Object_BeastRow* BeastTableRow = nullptr;
	const struct FKMTable_BaseStat_BeastRow* BeastStatTableRow = nullptr;
	
	bool bIsBeast = false;
};
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/KMGameplayTag.h"
#include "GameObject/EMGameObjectInstance.h"
#include "Notify/KMAnimNotifyState_Hit.h"
#include "KMGame/Tables/Generated/KMTableEnums.h"
#include "KMGame/Stat\KMSecondaryBaseStat.h"
#include "KMGameObjectInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMDeathDelegate, class UKMGameObjectInstance* gameObjectInstance);
DECLARE_MULTICAST_DELEGATE_OneParam(FKMDamageDelegate, const FKMDamageEvent& newDamageEvent);
DECLARE_MULTICAST_DELEGATE_FourParams(FKMStatChangeDelegate, class UKMGameObjectInstance* gameObjectInstance, EKMStatFactorType factorType, float prevValue, float newValue);
DECLARE_MULTICAST_DELEGATE_TwoParams(FKMInflictDelegate, int32 comboCount, class UKMGameObjectInstance* victimGameObjectInstance);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FKMCommbatMessageDelegate, const class UKMGameObjectInstance* gameObjectInstance, EKMCommbatMessageType messageType, const FString& newMessage);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FKMSkillMessageDelegate, const class UKMGameObjectInstance* gameObjectInstance, TSharedPtr<class FKMAbilityInstanceBase> abilityInstance, const FString& prefixMessage);

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMGameObjectInstance : public UEMGameObjectInstance
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameObjectInstance", meta=(AllowPrivateAccess=true))
	TObjectPtr<class UKMStatModifierBase> StatModifier;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class AActor> OwnerActor;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "GameObjectInstance", meta=(AllowPrivateAccess=true))
	TSubclassOf<class UKMSkillHandler> SkillHandlerClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "GameObjectInstance", meta=(AllowPrivateAccess=true))
	TObjectPtr<class UKMSkillHandler> SkillHandler = nullptr;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "GameObjectInstance", meta=(AllowPrivateAccess=true))
	TSubclassOf<class UKMStatModifierBase> StatModifierClass;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay() override;

	void SetTable(const struct FKMTable_ObjectRow* newObjectTable);
	const struct FKMTable_ObjectRow* GetTable() const;

	UFUNCTION(BlueprintPure)
	virtual FName GetTableId() const;

	UFUNCTION(BlueprintPure)
	virtual FName GetStatTableId() const;

	UFUNCTION(BlueprintPure)
	virtual FString GetObjectName() const;

	UFUNCTION(BlueprintPure)
	virtual FTransform GetTransform() const;

	UFUNCTION(BlueprintCallable)
	virtual void AddGameplayTag(FGameplayTag newTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnAddGameplayTag(const FGameplayTag& newTag);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveGameplayTag(FGameplayTag removedTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnRemoveGameplayTag(const FGameplayTag& removedTag);

	UFUNCTION(BlueprintPure)
	virtual bool HasGameplayTag(FGameplayTag tag) const;

	UFUNCTION(BlueprintPure)
	int32 GetGameplayTagCount(FGameplayTag tag) const;

	UFUNCTION(BlueprintCallable)
	void SetOwnerActor(AActor* newOwnerActor);

	UFUNCTION(BlueprintPure)
	AActor* GetOwnerActor() const;

	UFUNCTION(BlueprintPure)
	class UKMSkillHandler* GetSkillHandler() const;

	UFUNCTION(BlueprintPure)
	class UKMStatModifierBase* GetStatModifier() const;

	UFUNCTION(BlueprintCallable)
	void HitCheckClear();
	
	UFUNCTION(BlueprintCallable)
	void Stiff(float duration, bool bReset = false);

	UFUNCTION(BlueprintCallable)
	void SetTimeDilation(const FName& layerName, float newTimeDilation);

	UFUNCTION(BlueprintCallable)
	void RemoveTimeDilation(const FName& layerName);

	UFUNCTION(BlueprintPure)
	float GetTimeDilation() const;

	UFUNCTION(BlueprintPure)
	float GetHorizontalDistanceTo(const class UKMGameObjectInstance* toGameObjectInstance) const;

	UFUNCTION(BlueprintPure)
	float GetVerticalDistanceTo(const class UKMGameObjectInstance* toGameObjectInstance) const;

	UFUNCTION(BlueprintCallable)
	virtual void SetDirectionVisual(float newDirection, bool bForceRotate = false, class USkeletalMeshComponent* otherSkeletalMeshComp = nullptr);
	
	UFUNCTION(BlueprintCallable)
	virtual void SetDirection(float newDirection, bool bForceRotate = false);

	UFUNCTION(BlueprintPure)
	virtual float GetDirection() const;

	UFUNCTION(BlueprintCallable)
	void SetInteractionDirection(float newDirection);

	UFUNCTION(BlueprintPure)
	float GetInteractionDirection() const;

	UFUNCTION(BlueprintPure)
	virtual bool IsDead() const;

	UFUNCTION(BlueprintPure)
	virtual bool IsAir() const;
	
	virtual void Hit(class UKMGameObjectInstance* attackerGameObjectInstance, TSharedPtr<class FKMSkillInstance> latestSkillInstance, const FVector& hitClosestPoint, const FName& hitTag);

	void BoxHitImpact(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance,
		const FTransform& startOrientationTransform, const FTransform& endOrientationTransform,
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter, const FName& hitTag);

	void SphereHitImpact(
		const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance,
		const FTransform& startOrientationTransform, const FTransform& endOrientationTransform,
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter, const FName& hitTag);

	UFUNCTION(BlueprintCallable)
	virtual void Inflict(class UKMGameObjectInstance* victimGameObjectInstance);

	UFUNCTION(BlueprintCallable)
	void AddAggroTarget(class UKMGameObjectInstance* attackerGameObjectInstance);

	UFUNCTION(BlueprintPure)
	const class UKMGameObjectInstance* GetBestAggroTarget() const;

	virtual void BroadCastDamageEvent(const FKMDamageEvent& newDamageEvent);
	
	static FKMDeathDelegate& GetDeathDelegate()
	{
		static FKMDeathDelegate deathDelegate;
		return deathDelegate;
	}

	FKMDamageDelegate& GetDamageDelegate()
	{
		return DamageDelegate;
	}
	
	static FKMStatChangeDelegate& GetGlobalStatChangeEvent()
	{
		static FKMStatChangeDelegate statChangeEvent;
		return statChangeEvent;
	}
	
	FKMStatChangeDelegate& GetStatChangeEvent()
	{
		return StatChangeEvent;
	}

	FKMInflictDelegate& GetInflictDelegate()
	{
		return InflictDelegate;
	}

	FKMCommbatMessageDelegate& GetCombatMessageDelegate()
	{
		return CombatMessageDelegate;
	}

	static FKMSkillMessageDelegate& GetSkillMessageDelegate()
	{
		static FKMSkillMessageDelegate newSkillMessageDelegate;
		return newSkillMessageDelegate;
	}

protected:
	virtual void OnStatChange(EKMStatFactorType factorType, float prevValue, float newValue);
	virtual void OnDeath();

	virtual void ShowDamage(EKMStatFactorType factorType, int32 damage);

	virtual void HitCollection(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance, AActor* hitActor,const FVector& hitLocation, const FVector& hitNormal, const FName& hitTag);
	void HitCollections(const TWeakPtr<class FKMSkillInstance>& adjustSkillInstance, TArray<FHitResult> hitResults, UClass* actorClassFilter, const FName& hitTag);
	
	virtual void Tick(float deltaSeconds) override;

	void OnStiffRelease();

protected:
	FKMGameplayTagContainer GameplayTagContainer;
	FKMStatChangeDelegate StatChangeEvent;
	FKMDamageDelegate DamageDelegate;
	FKMInflictDelegate InflictDelegate;
	FKMCommbatMessageDelegate CombatMessageDelegate;
	
	FKMHitCheckData HitCheckData;

	FTimerHandle StiffTimerHandle;

	TMap<FName, float> TimeDilations;

	const struct FKMTable_ObjectRow* ObjectTable = nullptr;

	UPROPERTY()
	TSet<TWeakObjectPtr<UKMGameObjectInstance>> AggroTarget;
	
	EKMDamagePowerType InflectPowerType = EKMDamagePowerType::None;
	EKMDamagePowerType HitPowerType = EKMDamagePowerType::None;

	float Direction = 1.f;
	float InteractionDirection = 1.f;
};
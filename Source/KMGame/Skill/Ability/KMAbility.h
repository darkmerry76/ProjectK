#pragma once

#include "CoreMinimal.h"
#include "EMCurveWarpingComponent.h"
#include "EMMartialArts.h"
#include "GameplayTagContainer.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Core/KMDefine.h"
#include "KMAbility.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbility : public UEMAbility
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rate = 1.f;
	
	UPROPERTY(EditAnywhere, DisplayName="Loop", BlueprintReadWrite)
	bool bIsLoop = false;

	UPROPERTY(EditAnywhere, Category="Curve", BlueprintReadOnly)
	TObjectPtr<class UCurveVector> BlowCurve;

	UPROPERTY(EditAnywhere, Category="Curve", BlueprintReadWrite)
	float Duration = 1.f;
	
	UPROPERTY(EditAnywhere, Category="Curve", BlueprintReadWrite)
	float HorizontalPower = 100.f;

	UPROPERTY(EditAnywhere, Category="Curve", BlueprintReadWrite)
	float VerticalPower = 100.f;

	UPROPERTY(EditAnywhere, Category="Curve", DisplayName="ClearCurve", BlueprintReadWrite)
	bool bIsClearCurve = false;

	UPROPERTY(EditAnywhere, Category="Curve", BlueprintReadWrite)
	EEMCustomMovementMode CustomMovementMode = EEMCustomMovementMode::CMODE_Falling;  

	UPROPERTY()
	class UAnimMontage* Montage = nullptr;

protected:
	TSharedPtr<class FKMLockOnCluster> LockOnCluster;

	struct FAnimMontageInstance* MontageInstance = nullptr;

	FTransform ImpactTransform;
	int32 MartialArtsHandle = INDEX_NONE;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Activate();

	UFUNCTION(BlueprintNativeEvent)
	void OnActivated();

	UFUNCTION(BlueprintCallable)
	virtual void Deactivate(bool bCancel = false);

	UFUNCTION(BlueprintNativeEvent)
	void OnDeacivated(bool bCancel);

	UFUNCTION(BlueprintCallable)
	virtual void Trigger(const FGameplayTag& eventTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnTriggerEvent(const FGameplayTag& eventTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnRequestLoop();

	UFUNCTION(BlueprintNativeEvent)
	void OnRequestEnd();

	void PlayMartialArts(TSharedPtr<class UE::Anim::IAnimNotifyEventContextDataInterface> newContextData, float newRate = 1.f, bool bLooping = false);

	void StopMartialArts();

	void OnMartialArtsEnd(class FEMMartialArtsInstance* martialArtsInstance);

	UFUNCTION(BlueprintNativeEvent, DisplayName="OnMartialArtsEnd")
	void K2_OnMartialArtsEnd(int instanceId);

	UFUNCTION(BlueprintCallable)
	void MontageJump(FName sectionName);

	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetOwnerCharacterInstance() const;

	UFUNCTION(BlueprintPure)
	class AKMCharacter* GetOwnerCharacter() const;
	
	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetTargetCharacterInstance() const;

	UFUNCTION(BlueprintPure)
	class UEMMartialArtsComponent* GetMartialArtsComponent() const;
	
	UFUNCTION(BlueprintPure)
	class AKMCharacter* GetTargetCharacter() const;

	UFUNCTION(BlueprintCallable)
	void SetMontageRate(float newRate);

	UFUNCTION(BlueprintPure)
	float GetMontageRate() const;

	UFUNCTION(BlueprintPure)
	const FTransform& GetImpactTransform() const { return ImpactTransform; }

	void SetLockOnCluster(TSharedPtr<class FKMLockOnCluster> newLockOnCluster);

	UFUNCTION(BlueprintCallable)
	virtual void Impact(const FTransform& newImpactTransform);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnImpact(const FTransform& newImpactTransform);

	UFUNCTION(BlueprintPure)
	FVector GetOffsetAlongOwnerDirection(FVector Direction, float offsetDistance, float weight = 1.f, bool bIgnoreZ = false) const;
	
	UFUNCTION(BlueprintPure)
	FVector GetOffsetAlongOwnerForward(float offsetDistance, float weight = 1.f, bool bIgnoreZ = false) const;

	UFUNCTION(BlueprintPure)
	FVector GetOffsetAlongTargetForward(float offsetDistance, float weight = 1.f, bool bIgnoreZ = false) const;

	UFUNCTION(BlueprintPure)
	FVector GetOffsetToTargetByOwnerForward(float offsetDistance, float weight = 1.f, bool bIgnoreZ = false) const;

	UFUNCTION(BlueprintPure)
	FVector GetOffsetToTargetByOwnerForward2D(FVector2D offset, float weight = 1.f, bool bIgnoreZ = false) const;

	UFUNCTION(BlueprintPure)
	FVector GetOffsetOwnerAlongTargetDirection(float offsetDistance, float weight = 1.f, bool bIgnoreZ = false) const;
	
	UFUNCTION(BlueprintPure)
	FVector GetOffsetTargetAlongOwnerDirection(float offsetDistance, float weight = 1.f, bool bIgnoreZ = false) const;

	UFUNCTION(BlueprintPure)
	FVector GetOwnerFootLocation(float offsetHeight = 0.f) const;

	UFUNCTION(BlueprintCallable)
	void AddOwnerMotionWarpingLocation(FName targetName, FVector targetLocation);

	UFUNCTION(BlueprintCallable)
	void PlayOwnerCurveWarping(EEMCustomMovementMode movementMode, class UCurveBase* newCurveAsset, FVector newTargetLocation,
		float newPlayLength = 1.f, float newZScale = 1.f, bool bIgnoreZ = false);

	UFUNCTION(BlueprintPure)
	class UAnimMontage* GetOwnerAnimationTag(FGameplayTag tag) const;

	UFUNCTION(BlueprintPure)
	int32 GetMartialArtsHandle() const;

	UFUNCTION(BlueprintNativeEvent)
	void OnCurveWarpingInterrupt(const FVector& moveDelta, float deltaTime, const FEMCurveWarpingInstance& curveWarpingInstance, EEMCurveWarpingInteruptType interuptType, EEMCustomMovementMode movementMode);

	UFUNCTION(BlueprintCallable)
	virtual void ForceComplate();

	UFUNCTION(BlueprintCallable)
	void InverseDirection(bool bForce = false);

	UFUNCTION(BlueprintCallable)
	bool SetMontageRateByTag(class AKMCharacter* character, FName tag, float newRate = 1.f);

	UFUNCTION(BlueprintPure)
	float GetMontageRateByTag(class AKMCharacter* character, FName tag) const;

protected:
	struct FAnimMontageInstance* PlayMontage(class UAnimMontage* montage, float playRate = 1.f, FName startSectionName = NAME_None, EKMAnimSlotType slotType = EKMAnimSlotType::DefaultSlot, float slotBlendTime = 0.1f);
	void StopMontage(class UAnimMontage* montage);

	struct FAnimMontageInstance* GetMontageInstanceByTag(class AKMCharacter* character, const FName& tag) const;

protected:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	virtual UWorld* GetWorld() const override;
};
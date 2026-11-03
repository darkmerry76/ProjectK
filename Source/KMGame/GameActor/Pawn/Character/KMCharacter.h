#pragma once

#include "CoreMinimal.h"
#include "KMPawnBase.h"
#include "KMCharacter.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMCharacter : public AKMPawnBase
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UKMSkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMWidgetComponent> WidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DefaultWeaponItem = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMAnimationSetTag> AnimsetTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMSoundSetTag> SoundsetTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAudioComponent> AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> CameraTarget;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<class UKMItemAppearanceInstance> WeaponInstance;

	UPROPERTY(Transient, BlueprintReadWrite)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> AnimOverrideMontageMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	FVector InputVelocity = FVector::ZeroVector;

	UPROPERTY(Transient, BlueprintReadOnly)
	FVector LastetMoveInputVelocity = FVector::ZeroVector;

	UPROPERTY(Transient)
	TWeakObjectPtr<class UKMCharacterInstance> CharacterInstance = nullptr;

	UPROPERTY(Transient)
	const class UKMBeastPDA* BeastPDA = nullptr;

	uint64 LasteLandedFrameCount = 0;
	uint64 LasteWallHitFrameCount = 0;
	uint64 LastetCeilingHitFrameCount = 0;

	FVector RootMotionTranslationScale = FVector::One();

public:
	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetCharacterInstance() const;

	UFUNCTION(BlueprintPure)
	class UKMSkeletalMeshComponent* GetMesh() const;

	virtual class UKMWidgetComponent* GetWidgetComponent() const override;
	
	UFUNCTION(BlueprintPure)
	class USceneComponent* GetCameraTarget() const;

	virtual FTransform GetCarryOffsetTransform_Implementation() const override;

	UFUNCTION(BlueprintPure)
	const FVector& GetInputVelocity() const;
	
	UFUNCTION(BlueprintPure)
	const FVector& GetLatestMoveInputVelocity() const;

	virtual const FHitResult GetLatestWallHitResult_Implementation() const override;

	UFUNCTION(BlueprintPure)
	class UKMItemAppearanceInstance* GetWeaponInstance() const;

	UFUNCTION(BlueprintCallable)
	float MontqagePlayTag(FGameplayTag tag, float playRate = 1.f,
		EMontagePlayReturnType returnValueType = EMontagePlayReturnType::MontageLength, float timeToStartMontageAt=0.f, bool bStopAllMontages = true);

	UFUNCTION(BlueprintCallable)
	float MontqagePlay(class UAnimMontage* animMontage, float playRate = 1.f,
		EMontagePlayReturnType returnValueType = EMontagePlayReturnType::MontageLength, float timeToStartMontageAt=0.f, bool bStopAllMontages = true);

	UFUNCTION(BlueprintCallable)
	void StopAnimMontage(class UAnimMontage* animMontage);

	UFUNCTION(BlueprintPure)
	class UAnimMontage* GetAnimationTagOriginal(FGameplayTag tag) const;

	UFUNCTION(BlueprintPure)
	class UAnimMontage* GetAnimationTag(FGameplayTag tag) const;

	UFUNCTION(BlueprintPure)
	class USoundBase* GetSoundTag(FGameplayTag tag) const;

	UFUNCTION(BlueprintCallable)
	void SetMovementOverrideMontage(class UAnimMontage* jumpMontage, class UAnimMontage* landingMontage);

	UFUNCTION(BlueprintCallable)
	void RevertOverrideAnimationTag(FGameplayTag tag);
	
	UFUNCTION(BlueprintCallable)
	void RemoveMovementOverrideMontage();

	UFUNCTION(BlueprintCallable)
	void SetBeastPDA(const class UKMBeastPDA* newBeastPDA);

	UFUNCTION(BlueprintPure)
	const class UKMBeastPDA* GetBeastPDA() const;

	UFUNCTION(BlueprintPure)
	virtual FTransform GetApproachPullPoint(EKMApproachPullPointType approachPullPointType = EKMApproachPullPointType::Auto) const override;

	virtual void ApplyCapsuleFootAlignment(float addFootHeight) override;
	virtual void RestoreCapsuleFootAlignment() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void OnConstruction(const FTransform& transform) override;

	virtual void StartJump() override;

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	virtual void LandHit(const FHitResult& hitResult) override;
};
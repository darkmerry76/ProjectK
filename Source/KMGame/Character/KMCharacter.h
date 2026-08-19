#pragma once

#include "CoreMinimal.h"
#include "Character/EMCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "GameObject/KMCharacterInstance.h"
#include "KMCharacter.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMCharacter : public AEMCharacterBase
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DefaultWeaponItem = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMAnimationSetTag> AnimsetTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMSoundSetTag> SoundsetTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UEMCurveWarpingComponent> CurveWarping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMMartialArtsComponent> MartialArtsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAudioComponent> AudioComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UKMItemAppearanceInstance> WeaponInstance;

	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> AnimOverrideMontageMap;

	UPROPERTY(BlueprintReadOnly)
	FVector InputVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector LastetMoveInputVelocity = FVector::ZeroVector;

	UPROPERTY(Transient)
	const class UKMBeastPDA* BeastPDA = nullptr;

	uint64 LasteLandedFrameCount = 0;

public:
	class UKMCharacterInstance* GetCharacterInstance() const;
	virtual void PossessedByCharacterInstance(class UEMGameObjectInstance* newCharacterInstance) override;

	class UEMMartialArtsComponent* GetMartialArtsComponent() const;

	class UEMCurveWarpingComponent* GetCurveWarping() const { return CurveWarping; }

	void SetMirror(bool bMirror);
	bool GetMirror() const;

	const FVector& GetInputVelocity() const;
	const FVector& GetLatestMoveInputVelocity() const;

	UFUNCTION(BlueprintPure)
	class UKMItemAppearanceInstance* GetWeaponInstance() const;

	UFUNCTION(BlueprintCallable)
	float MontqagePlayTag(FGameplayTag tag, float playRate = 1.f,
		EMontagePlayReturnType returnValueType = EMontagePlayReturnType::MontageLength, float timeToStartMontageAt=0.f, bool bStopAllMontages = true);

	UFUNCTION(BlueprintCallable)
	float MontqagePlay(class UAnimMontage* animMontage, float playRate = 1.f,
		EMontagePlayReturnType returnValueType = EMontagePlayReturnType::MontageLength, float timeToStartMontageAt=0.f, bool bStopAllMontages = true);

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

	void SetBeastPDA(const class UKMBeastPDA* newBeastPDA);
	const class UKMBeastPDA* GetBeastPDA() const;

	virtual void Landed(const FHitResult& hitResult) override;

	UFUNCTION(BlueprintPure)
	bool IsAir() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

protected:
	bool bIsMirror = false;
};
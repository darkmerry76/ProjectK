#pragma once

#include "CoreMinimal.h"
#include "Character/EMCharacterBase.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "GameObject/KMCharacterInstance.h"
#include "KMCharacter.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API AKMCharacter : public AEMCharacterBase, public IKMPawnInterface
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
	TObjectPtr<class UKMCurveWarpingComponent> CurveWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMMartialArtsComponent> MartialArtsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAudioComponent> AudioComponent;

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

public:
	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetCharacterInstance() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMGameObjectInstance* GetGameObjectInstance() const override;
	
	virtual void PossessedByGameObjectInstance(UKMGameObjectInstance* newGameObjectInstance) override;

	UFUNCTION(BlueprintPure)
	virtual class UPrimitiveComponent* GetMovementShapeComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual class UKMMartialArtsComponent* GetMartialArtsComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual class UKMCurveWarpingComponent* GetCurveWarpingComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual class UMeshComponent* GetMasterMeshComponent() const override;

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
\
	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag) override;

	UFUNCTION(BlueprintImplementableEvent)
	void Receive_OnImpact(const FName& skillEffectName, const FVector& hitClosestPoint, const FName& hitTag);
	
	virtual void OnDeath() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void Receive_OnDeath();

	UFUNCTION(BlueprintCallable)
	virtual void Crarry(class UKMGameObjectInstance* carryedGameObjectInstance) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Crarried(class UKMGameObjectInstance* carryGameObjectInstance) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnCarry")
	void Receive_OnCarry(class UKMGameObjectInstance* carryedGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnCarried")
	void Receive_OnCarried(class UKMGameObjectInstance* carryedGameObjectInstance);

protected:
	bool bIsMirror = false;
};
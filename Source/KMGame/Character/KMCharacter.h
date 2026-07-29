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

	UPROPERTY(Transient)
	const class UKMBeastPDA* BeastPDA = nullptr;

public:
	class UKMCharacterInstance* GetCharacterInstance() const;
	virtual void PossessedByCharacterInstance(class UEMGameObjectInstance* newCharacterInstance) override;

	class UEMMartialArtsComponent* GetMartialArtsComponent() const;

	class UEMCurveWarpingComponent* GetCurveWarping() const { return CurveWarping; }

	void SetMirror(bool bMirror);
	bool GetMirror() const;

	const FVector& GetInputVelocity() const;

	class UKMItemAppearanceInstance* GetWeaponInstance() const;

	class UAnimMontage* GetAnimationTag(FGameplayTag tag) const;

	UFUNCTION(BlueprintCallable)
	void SetMovementOverrideMontage(class UAnimMontage* jumpMontage, class UAnimMontage* landingMontage);

	UFUNCTION(BlueprintCallable)
	void RemoveMovementOverrideMontage();

	void SetBeastPDA(const class UKMBeastPDA* newBeastPDA);
	const class UKMBeastPDA* GetBeastPDA() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

protected:
	bool bIsMirror = false;
};
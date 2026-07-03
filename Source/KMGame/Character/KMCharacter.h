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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DefaultWeaponItem = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMAnimationSetSkill> SkillAnimset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMAnimationSetEffect> EffectAnimset;

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

	UPROPERTY(BlueprintReadOnly)
	FVector InputVelocity = FVector::ZeroVector;

public:
	class UKMCharacterInstance* GetCharacterInstance() const;
	virtual void PossessedByCharacterInstance(class UEMGameObjectInstance* newCharacterInstance) override;

	class UEMMartialArtsComponent* GetMartialArtsComponent() const;

	class UEMCurveWarpingComponent* GetCurveWarping() const { return CurveWarping; }

	void SetMirror(bool bMirror);
	bool GetMirror() const;

	const FVector& GetInputVelocity() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

protected:
	bool bIsMirror = false;
};
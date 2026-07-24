#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "Core/KMDefine.h"
#include "KMAnimNotifyState_Animation.generated.h"

USTRUCT()
struct KMGAME_API FKMAnimNotifyState_Animation_Context
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Transient)
	TObjectPtr<class UAnimMontage> ActivatedMontage;

	FAnimMontageInstance* MontageInstance = nullptr;

	float ElapsedTime = 0.f;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_Animation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Animation")
class KMGAME_API UKMAnimNotifyState_Animation : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	bool bUseSkillSet = true;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, BlueprintReadOnly, meta=(AllowPrivateAccess=true, EditCondition="bUseSkillSet", DisplayAfter="bUseSkillSet"))
	FEMAnimationSetTag AnimationSetTag;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="AnimationSetTag"))
	TObjectPtr<class UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="Immediate", meta=(AllowPrivateAccess=true, DisplayAfter="Montage"))
	bool bIsImmediate = true;

	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="StartShowMeshComponent", meta=(AllowPrivateAccess=true, DisplayAfter="Montage"))
	bool bIsStartShowMeshComponent = false;

	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="EndRemoveTagMeshComponent", meta=(AllowPrivateAccess=true, DisplayAfter="Montage"))
	bool bIsEndRemoveTagMeshComponent = true;

	TMap<TObjectPtr<class USkeletalMeshComponent>, TSharedPtr<FKMAnimNotifyState_Animation_Context>> Context;

	float CustomDuration = 0.f;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	virtual bool IsCustomDuration() const override;
	virtual float GetCustomDuration() const override;

	class UAnimMontage* GetUsedMontage(AActor* actor);

protected:
	virtual FString GetNotifyName_Implementation() const override;
	
#if WITH_EDITOR
	virtual void SetEditorPosition(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float currentTime, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void PostEditChangeProperty(AActor* ownerActor, FPropertyChangedEvent& propertyChangedEvent) override;
#endif
};

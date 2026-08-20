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
	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="Master", meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	bool bIsMaster = false;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="UseSkillSet", meta=(AllowPrivateAccess=true, DisplayAfter="bIsMaster"))
	bool bIsUseSkillSet = true;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, BlueprintReadOnly, meta=(AllowPrivateAccess=true, EditCondition="bIsUseSkillSet", DisplayAfter="bUseSkillSet"))
	FEMAnimationSetTag AnimationSetTag;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="AnimationSetTag"))
	TObjectPtr<class UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="Montage"))
	EKMAnimSlotType SlotType = EKMAnimSlotType::DefaultSlot;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="SlotType"))
	FName MontageInstanceTag = NAME_None;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, EditCondition="SlotType==EKMAnimSlotType::OverrideSlot", DisplayAfter="MontageInstanceTag"))
	float SlotBlendInTime = 0.1f;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, EditCondition="SlotType==EKMAnimSlotType::OverrideSlot", DisplayAfter="SlotBlendInTime"))
	float SlotBlendOutTime = 0.1f;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="EndMontageStop", meta=(AllowPrivateAccess=true, DisplayAfter="SlotBlendOutTime"))
	bool bIsEndMontageStop = false;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="StartShowMeshComponent", meta=(AllowPrivateAccess=true, DisplayAfter="bIsEndMontageStop"))
	bool bIsStartShowMeshComponent = false;

	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="EndRemoveTagMeshComponent", meta=(AllowPrivateAccess=true, DisplayAfter="bIsStartShowMeshComponent"))
	bool bIsEndRemoveTagMeshComponent = true;

	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="OverrideMovementAnimSet", meta=(AllowPrivateAccess=true, DisplayAfter="bIsStartShowMeshComponent"))
	bool bIsOverrideMovementAnimSet = false;

	UPROPERTY(EditAnywhere, Category=AnimNotify, DisplayName="EqualsPlay", meta=(AllowPrivateAccess=true, DisplayAfter="bIsOverrideMovementAnimSet"))
	bool bIsEqualsPlay = true;
	
	TMap<TObjectPtr<class USkeletalMeshComponent>, TSharedPtr<FKMAnimNotifyState_Animation_Context>> Context;

	float CustomDuration = 0.f;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	virtual bool IsCustomDuration() const override;
	virtual float GetCustomDuration() const override;

	class UAnimMontage* GetUsedMontage(AActor* actor, const FName& slotName) const;

protected:
	virtual FString GetNotifyName_Implementation() const override;
	
	void CollectionMontageSection(USkeletalMeshComponent* meshComp, const FAnimNotifyEvent& notifyEvent);
	
#if WITH_EDITOR
	virtual void SetEditorPosition(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float currentTime, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void PostEditChangeProperty(AActor* ownerActor, FPropertyChangedEvent& propertyChangedEvent) override;
	virtual void DrawInEditor(class FPrimitiveDrawInterface* pDI, class USkeletalMeshComponent* meshComp, const class UAnimSequenceBase* animation, const FAnimNotifyEvent& notifyEvent) const;
#endif
};

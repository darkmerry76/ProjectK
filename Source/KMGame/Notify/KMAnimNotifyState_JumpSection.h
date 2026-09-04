#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "Core/KMDefine.h"
#include "KMAnimNotifyState_JumpSection.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_JumpSection
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play JumpSection")
class KMGAME_API UKMAnimNotifyState_JumpSection : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	FName SectionName = NAME_None;
	
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="SectionName"))
	EKMAnimSlotType SlotType = EKMAnimSlotType::DefaultSlot;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="SlotType"))
	FName MontageInstanceTag = NAME_None;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, EditCondition="SlotType==EKMAnimSlotType::OverrideSlot", DisplayAfter="MontageInstanceTag"))
	float SlotBlendOutTime = 0.1f;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};

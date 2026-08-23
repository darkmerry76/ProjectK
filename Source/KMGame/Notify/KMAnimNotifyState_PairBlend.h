#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_PairBlend.generated.h"

USTRUCT()
struct KMGAME_API FKMAnimNotifyState_Pair_Context
{
	GENERATED_USTRUCT_BODY()

	bool IsValid() const;

	UPROPERTY()
	TObjectPtr<class UKMCharacterInstance> LeaderCharacterInstance = nullptr;
	struct FAnimMontageInstance* LeaderMontageInstance = nullptr;

	UPROPERTY()
	TObjectPtr<class UKMCharacterInstance> FollowerCharacterInstance = nullptr;
	struct FAnimMontageInstance* FollowerMontageInstance = nullptr;

	float ElapsedTime = 0.f;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_PairBlend
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play PairBlend")
class KMGAME_API UKMAnimNotifyState_PairBlend : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="StartBlend", Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	bool bIsStartBlend = false;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(EditCondition="bIsStartBlend", AllowPrivateAccess=true, DisplayAfter="bIsStartBlend"))
	float StartBlendTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="StartBlendTime"))
	FVector PairOffset = FVector::ZeroVector; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="StopBlend", Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="PairOffset"))
	bool bIsStopBlend = false;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(EditCondition="bIsStopBlend", AllowPrivateAccess=true, DisplayAfter="bIsStopBlend"))
	float StopBlendTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, DisplayName="BlockReflection", meta=(AllowPrivateAccess=true, DisplayAfter="StopBlendTime"))
	bool bIsBlockReflection = false;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="bIsBlockReflection"))
	FName LeaderMontageInstanceTag = NAME_None;

	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="LeaderMontageInstanceTag"))
	FName FollowMontageInstanceTag = NAME_None;
	
	TMap<TObjectPtr<class USkeletalMeshComponent>, TSharedPtr<FKMAnimNotifyState_Pair_Context>> PairContexts;

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	void FollowAnimation(const class USkeletalMeshComponent* meshComp, const FAnimNotifyEventReference& eventReference) const;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};

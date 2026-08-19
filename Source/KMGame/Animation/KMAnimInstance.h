#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimNodeReference.h"
#include "AnimNode/KMAnimNode_MultiSlot.h"
#include "AnimNode/KMAnimNode_Shake.h"
#include "KMAnimInstance.generated.h"

struct FKMPairPositionBlendInfo
{
	bool IsValid() const
	{
		return bIsEnableBlend;
	}
	
	bool bIsEnableBlend = false;
	FTransform StartWorldTransform = FTransform::Identity;
	FVector PreviousWorldOffset = FVector::ZeroVector;
	FVector WorldOffset = FVector::ZeroVector;

	FVector FinalWorldPosition = FVector::ZeroVector;

	float Duration = 0.f;
	float EplisedTime = 0.f;
};

class FKMAnimInstanceProxy : public FAnimInstanceProxy
{
public:
	FKMAnimInstanceProxy(class UAnimInstance* instance);
	
	const FKMMultiSlotBlendInfo& GetSlotBlendInfo() const;
	const FKMAnimNodeShakeData& GetShakeData() const;
	const FKMPairPositionBlendInfo& GetPairBlendInfo() const;

protected:
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;

protected:
	FKMMultiSlotBlendInfo SlotBlendInfo = { };
	FKMAnimNodeShakeData ShakeData = { };
	FKMPairPositionBlendInfo PairBlendInfo = { };
};

UCLASS(Abstract)
class KMGAME_API UKMAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBlendSpace1D> MoveBlend;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	float CurrentDirection = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float PrevActorDirection = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AddPelvisDirection = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LerpDirectionSpeed = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> HiddenBones;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FKMMultiSlotBlendInfo SlotBlendInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSlotBlending = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector PairLocalOffset = FVector::ZeroVector;;

	FKMMultiSlotBlendInfo NextSlotBlendInfo;
	float StartBlendWeight = 0.f;
	float SlotBlendTime = 0.f;
	float SlotBlendElapsedTime = 0.f;
	
	float MovementElipsedTime = 0.f;

	FKMAnimNodeShakeData ShakeData;
	

	FKMPairPositionBlendInfo PairBlendInfo;

protected:
	float NextDirection = 0.f;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaSeconds) override;
	
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent) override;
#endif
	
public:
	void SetCurrentDirection(float newCurrentDirection);
	float GetCurrentDirection() const;

	void SetNextDirection(float newNextDirection);
	float GetNextDirection() const;

	const TArray<FName>& GetHiddenBones() const;

	UFUNCTION(BlueprintCallable)
	void ResetMovementElipsedTime();

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	float GetMovementElapsedTime() const;

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe))
	bool IsCustomWalking() const;

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void BlendSlot(EKMAnimSlotType newSlotType, float newWeight, float blendTime = 0.f);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void StartShake(float newDistance, float newFrequency, float newDuration = 0.2f);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void BlendPairPosition(const FTransform& startWorldTransform, const FVector& targetWorldOffset, float newDuration = 0.2f);
	
	const FKMMultiSlotBlendInfo& GetSlotBlendInfo() const;
	const FKMMultiSlotBlendInfo& GetNextSlotBlendInfo() const;
	const FKMPairPositionBlendInfo& GetPairBlendInfo() const;

	const FKMAnimNodeShakeData& GetShakeData() const;

protected:
	void TickSlotBlend(float deltaTime);
	void TickShake(float deltaTime);
	void TickPairBlend(float deltaTime);

protected:
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe, AllowPrivateAccess="true"))
	void InitOnMoveBlendSpaceUpdate(const FAnimUpdateContext& context, const FAnimNodeReference& node);
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe, AllowPrivateAccess="true"))
	void OnMoveBlendSpaceUpdate(const FAnimUpdateContext& context, const FAnimNodeReference& node);
};
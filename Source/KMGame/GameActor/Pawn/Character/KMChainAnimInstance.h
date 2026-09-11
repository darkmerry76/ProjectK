#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "KMChainAnimInstance.generated.h"

class FKMChainAnimInstanceProxy : public FAnimInstanceProxy
{
public:
	FKMChainAnimInstanceProxy(class UAnimInstance* instance);
	
	const FPoseSnapshot& GetPoseSnapshot() const;
	float GetBlendAlpha() const;
	bool IsEnableAttack() const;
	const FVector& GetTargetLocation() const;

protected:
	virtual void PreUpdate(UAnimInstance* animInstance, float deltaSeconds) override;

protected:
	FPoseSnapshot Snapshot;
	float BlendAlpha = 0.f;
	bool EnableAttack = false;
	FVector TargetLocation = FVector::ZeroVector;
};

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMChainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPoseSnapshot Snapshot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendAlpha = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EnableAttack = false;

	FVector TargetLocation = FVector::ZeroVector;
	
	void SetTargetLocation(const FVector& newTargetlocation);

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
};
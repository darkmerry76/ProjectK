#pragma once

#include "CoreMinimal.h"
#include "LayeredMoveBase.h"
#include "DefaultMovementSet/LayeredMoves/AnimRootMotionLayeredMove.h"
#include "DefaultMovementSet/LayeredMoves/MontageStateProvider.h"
#include "EMMoverLayer.generated.h"

USTRUCT(BlueprintType)
struct EMCOREGAME_API FEMLayeredMove_MoveTo : public FLayeredMoveBase
{
	GENERATED_BODY()

	FEMLayeredMove_MoveTo();
	virtual ~FEMLayeredMove_MoveTo() override = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mover)
	FVector StartLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mover)
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mover)
	bool bRestrictSpeedToExpected;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mover)
	TObjectPtr<class UCurveVector> PathOffsetCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mover)
	TObjectPtr<UCurveFloat> TimeMappingCurve;
	
	virtual bool GenerateMove(const FMoverTickStartData& startState, const FMoverTimeStep& timeStep, const UMoverComponent* moverComp, UMoverBlackboard* simBlackboard, FProposedMove& outProposedMove) override;

	virtual FLayeredMoveBase* Clone() const override;

	virtual void NetSerialize(FArchive& ar) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual FString ToSimpleString() const override;

	virtual void AddReferencedObjects(class FReferenceCollector& collector) override;
	
protected:
	FVector GetPathOffsetInWorldSpace(const float moveFraction) const;
	float EvaluateFloatCurveAtFraction(const UCurveFloat& curve, const float fraction) const;
};

template<>
struct TStructOpsTypeTraits<FEMLayeredMove_MoveTo> : public TStructOpsTypeTraitsBase2<FEMLayeredMove_MoveTo >
{
	enum
	{
		WithCopy = true
	};
};

USTRUCT(BlueprintType)
struct EMCOREGAME_API FEMLayeredMove_AnimRootMotion : public FLayeredMove_AnimRootMotion
{
	GENERATED_BODY()

	FEMLayeredMove_AnimRootMotion();
	virtual ~FEMLayeredMove_AnimRootMotion() override = default;

	virtual bool GenerateMove(const FMoverTickStartData& startState, const FMoverTimeStep& timeStep, const class UMoverComponent* moverComp, class UMoverBlackboard* simBlackboard, FProposedMove& outProposedMove) override;
	virtual bool HasGameplayTag(FGameplayTag tagToFind, bool bExactMatch) const override;

	virtual struct FLayeredMoveBase* Clone() const override;
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FString ToSimpleString() const override;
};

template<>
struct TStructOpsTypeTraits<FEMLayeredMove_AnimRootMotion> : public TStructOpsTypeTraitsBase2<FEMLayeredMove_AnimRootMotion>
{
	enum
	{
		WithCopy = true
	};
};
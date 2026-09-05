#pragma once

#include "CoreMinimal.h"
#include "KMAbilitySkill.h"
#include "KMAbilityPutDown.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityPutDown : public UKMAbilitySkill
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Putdown")
	FName PutDownMontageInstanceTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Putdown")
	float ItemPivotExtractTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Putdown")
	FName ItemPivotBoneName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Putdown")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Putdown")
	float SearchAngleRange = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Putdown")
	float SearchAngleInterval = 5.f;

	UPROPERTY(Transient)
	TArray<AActor*> ActorsToIgnore;
	
	FTransform OriginTransform;
	FTransform ItemPivotBoneTransform;

	FTransform ItemTargetTransform;
	bool bIsAvailableItem = false;

	bool bIsPutDowned = false;

	float beforeInteractionDirection = -1.f;

	int32 PlacementMeshcomponentId = INDEX_NONE;

public:
	bool StartPutDown(FName attackSocket, float blendingDuration = 0.2f);
	void CompletePutDown();
	
protected:	
	virtual void Activate() override;
	virtual void Deactivate(bool bCancel) override;
	virtual void Tick(float deltaTime) override;

protected:
	bool CalcAvailableTargetTransform(FTransform& outAvailableTargetTransform) const;

	FTransform GetOriginWorldTransform() const;

	bool TestOverlapWorld(const FTransform& testWorldTransform) const;

	virtual void OnRequestEnd_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartPutDown();
};
#pragma once

#include "CoreMinimal.h"
#include "KMBTTaskNode.h"
#include "KMBTTaskNode_MoveFollow.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMBTTaskNode_MoveFollow : public UKMBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	FVector ComputeDesiredPosition(const class UKMCharacterInstance* ownerCharacterInstance,
		const TSharedPtr<class FKMLockOnCluster>& lockOnCluster, float attackRange) const;

	FVector FindNonOverlappingAttackPosition(
		const FVector& ownerLocation,
		const FVector& targetLocation,
		const TArray<FVector>& otherAttackPositions,
		float attackRange,
		float minDistanceBetweenUnits) const;
};
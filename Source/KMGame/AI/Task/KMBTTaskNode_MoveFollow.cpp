#include "KMBTTaskNode_MoveFollow.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameObject/KMCharacterInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_Skill.h"
#include "Util/KMUtil.h"

UKMBTTaskNode_MoveFollow::UKMBTTaskNode_MoveFollow(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "MoveFollow";
	bNotifyTick = true;
}

FVector UKMBTTaskNode_MoveFollow::FindNonOverlappingAttackPosition(
	const FVector& ownerLocation,
	const FVector& targetLocation,
	const TArray<FVector>& otherAttackPositions,
	float attackRange,
	float minDistanceBetweenUnits) const
{
	FVector baseDir = (ownerLocation - targetLocation).GetSafeNormal();
	FVector bestPos = targetLocation + baseDir * attackRange;

	const int32 maxIterations = 36;
	const float stepDegrees = 10.f;

	for (int32 i = 0; i < maxIterations; ++i)
	{
		float angle = stepDegrees * i;

		float offsetAngle = angle;

		FVector rotatedDir = baseDir.RotateAngleAxis(offsetAngle, FVector::UpVector);
		FVector candidatePos = targetLocation + rotatedDir * attackRange;

		bool bOverlap = false;
		for (const FVector& otherPos : otherAttackPositions)
		{
			if (FVector::DistSquared(otherPos, candidatePos) < FMath::Square(minDistanceBetweenUnits))
			{
				bOverlap = true;
				break;
			}
		}

		if (!bOverlap)
		{
			bestPos = candidatePos;
			break;
		}
	}
	return bestPos;
}

FVector UKMBTTaskNode_MoveFollow::ComputeDesiredPosition(
	const UKMCharacterInstance* ownerCharacterInstance, const TSharedPtr<FKMLockOnCluster>& lockOnCluster, float attackRange) const
{
	UKMGameObjectInstance* targetGameObjectInstance = lockOnCluster->GetBestTarget();
	check(IsValid(targetGameObjectInstance));

	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
	check(IsValid(targetSubsystem));
	
	check(!lockOnCluster->Targets.IsEmpty());

	TSharedPtr<FKMTargetCluster> targetCluster = targetSubsystem->GetTargetCluster(lockOnCluster->GetBestTargetKey());

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));
	
	TArray<FVector> occupiedPositions;
	for (auto attacker : targetCluster->Attackers)
	{
		UKMCharacterInstance* otherAttackerInstance = Cast<UKMCharacterInstance>(gameObjectSubsystem->GetGameObject(attacker));
		if(!IsValid(otherAttackerInstance))
		{
			continue;
		}

		if (ownerCharacterInstance == otherAttackerInstance)
		{
			continue;
		}
/*		TSharedPtr<FKMMovementTarget> moveTarget = otherAttackerInstance->GetMovementTarget();
		if (!moveTarget.IsValid())
		{
			continue;
		}
		
		occupiedPositions.Add(moveTarget->GetDestinationTransform().GetLocation());*/
	}
	
	FVector ownerLocation = ownerCharacterInstance->GetTransform().GetLocation();
	FVector targetLocation = targetGameObjectInstance->GetTransform().GetLocation();

	return FindNonOverlappingAttackPosition(ownerLocation, targetLocation, occupiedPositions, attackRange, 150.f);
}

EBTNodeResult::Type UKMBTTaskNode_MoveFollow::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	UBlackboardComponent* blackboardComponent = ownerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent));

	FKMSkillKey skillKey = FKMSkillKey::CreateKey(
		blackboardComponent->GetValueAsName(TEXT("SkillId")),
		blackboardComponent->GetValueAsInt(TEXT("SkillLevel")));

	if (!skillKey.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	if (skillKey.TableRecord->Range < 0.001f)
	{
		return EBTNodeResult::Failed;
	}

	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance));
	
	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
	check(IsValid(targetSubsystem));

	TSharedPtr<FKMLockOnCluster> lockOnCluster = targetSubsystem->GetLockOnCluster(ownerCharacterInstance->GetId());
	if (!lockOnCluster.IsValid() || !lockOnCluster->IsBestTargetAvailable())
	{
		return EBTNodeResult::Failed;
	}

	FTransform destinationTransform;
	destinationTransform.SetLocation(ComputeDesiredPosition(ownerCharacterInstance, lockOnCluster, skillKey.TableRecord->Range));
	
/*	if (!ownerCharacterInstance->MoveTo(destinationTransform))
	{
		return EBTNodeResult::Failed;
	}*/
	
	return EBTNodeResult::InProgress;
}

void UKMBTTaskNode_MoveFollow::TickTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory, float deltaSeconds)
{
	UKMCharacterInstance* sourceCharacterInstance = UKMUtil::GetCharacterInstanceByController(ownerComp.GetAIOwner());
	check(IsValid(sourceCharacterInstance));

/*	if (!sourceCharacterInstance->IsMoving())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}*/
}

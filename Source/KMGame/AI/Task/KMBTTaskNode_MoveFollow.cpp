#include "KMBTTaskNode_MoveFollow.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KMCharacter.h"
#include "Skill/KMSkillTypes.h"
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

		if (bOverlap == false)
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
	UKMCharacterInstance* targetCharacterInstance = lockOnCluster->GetBestTarget();
	check(IsValid(targetCharacterInstance) == true);

	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
	check(IsValid(targetSubsystem) == true);
	
	check(lockOnCluster->Targets.Num() > 0);

	TSharedPtr<FKMTargetCluster> targetCluster = targetSubsystem->GetTargetCluster(lockOnCluster->GetBestTargetKey());

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem) == true);
	
	TArray<FVector> occupiedPositions;
	for (auto attacker : targetCluster->Attackers)
	{
		UKMCharacterInstance* otherAttackerInstance = Cast<UKMCharacterInstance>(gameObjectSubsystem->GetGameObject(attacker));
		if(IsValid(otherAttackerInstance) == false)
		{
			continue;
		}

		if (ownerCharacterInstance == otherAttackerInstance)
			continue;

/*		TSharedPtr<FKMMovementTarget> moveTarget = otherAttackerInstance->GetMovementTarget();
		if (moveTarget.IsValid() == false)
		{
			continue;
		}
		
		occupiedPositions.Add(moveTarget->GetDestinationTransform().GetLocation());*/
	}
	
	FVector ownerLocation = ownerCharacterInstance->GetTransform().GetLocation();
	FVector targetLocation = targetCharacterInstance->GetTransform().GetLocation();

	return FindNonOverlappingAttackPosition(ownerLocation, targetLocation, occupiedPositions, attackRange, 150.f);
}

EBTNodeResult::Type UKMBTTaskNode_MoveFollow::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* blackboardComponent = OwnerComp.GetBlackboardComponent();
	check(IsValid(blackboardComponent) == true);

	FKMSkillKey skillKey = FKMSkillKey::CreateKey(
		blackboardComponent->GetValueAsName(TEXT("SkillId")),
		blackboardComponent->GetValueAsInt(TEXT("SkillLevel")));

	if (skillKey.IsValid() == false)
	{
		return EBTNodeResult::Failed;
	}

	if (skillKey.TableRecord->Range < 0.001f)
	{
		return EBTNodeResult::Failed;
	}

	UKMCharacterInstance* ownerCharacterInstance = UKMUtil::GetCharacterInstanceByController(OwnerComp.GetAIOwner());
	check(IsValid(ownerCharacterInstance) == true);
	
	UKMTargetSubsystem* targetSubsystem = UKMTargetSubsystem::GetTargetSubsystem(this);
	check(IsValid(targetSubsystem) == true);

	TSharedPtr<FKMLockOnCluster> lockOnCluster = targetSubsystem->GetLockOnCluster(ownerCharacterInstance->GetId());
	if (lockOnCluster.IsValid() == false || lockOnCluster->IsBestTargetAvailable() == false)
	{
		return EBTNodeResult::Failed;
	}

	FTransform destinationTransform;
	destinationTransform.SetLocation(ComputeDesiredPosition(ownerCharacterInstance, lockOnCluster, skillKey.TableRecord->Range));
	
/*	if (ownerCharacterInstance->MoveTo(destinationTransform) == false)
	{
		return EBTNodeResult::Failed;
	}*/
	
	return EBTNodeResult::InProgress;
}

void UKMBTTaskNode_MoveFollow::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UKMCharacterInstance* sourceCharacterInstance = UKMUtil::GetCharacterInstanceByController(OwnerComp.GetAIOwner());
	check(IsValid(sourceCharacterInstance) == true);

/*	if (sourceCharacterInstance->IsMoving() == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}*/
}

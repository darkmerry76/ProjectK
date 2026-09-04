#include "KMAbilityPutDown.h"
#include "Animation/KMAnimInstance.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMGameObjectInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMIndicatorSubsystem.h"

UKMAbilityPutDown::UKMAbilityPutDown(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityPutDown::Activate()
{
	UKMAbility::Activate();

	check(SearchAngleInterval > 0.f);

	ActorsToIgnore.Reset();

	AActor* ownerActor = GetOwnerActor();
	if (IsValid(ownerActor))
	{
		ActorsToIgnore.Emplace(ownerActor);
	}
	if (IsValid(GetTargetActor()))
	{
		ActorsToIgnore.Emplace(GetTargetActor());
	}

	OriginTransform = GetOriginWorldTransform();
	ItemPivotBoneTransform = GetMontageComponentSpaceBoneTransform(GetOwnerCharacter(), PutDownMontageInstanceTag, ItemPivotExtractTime, ItemPivotBoneName, false);

	bIsAvailableItem = CalcAvailableTargetTransform(ItemTargetTransform);

	if (IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(GetTargetActor()))
	{
		UMeshComponent* masterMeshComponent = pawnInterface->GetMasterMeshComponent();
		if (IsValid(masterMeshComponent))
		{
			UKMIndicatorSubsystem* indicatorSubsystem = UKMIndicatorSubsystem::GetIndicatorSubsystem(this);
			check(IsValid(indicatorSubsystem));

			FTransform newTransform = ItemTargetTransform;
			newTransform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
			PlacementMeshcomponentId = indicatorSubsystem->SpawnPlacementMeshComponent(masterMeshComponent, newTransform);
			if (UMeshComponent* placementComponent = indicatorSubsystem->GetMeshComponent(PlacementMeshcomponentId))
			{
				placementComponent->SetRelativeTransform(masterMeshComponent->GetRelativeTransform());
			}
		}
	}

	PostActivated();
}

void UKMAbilityPutDown::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);
}

void UKMAbilityPutDown::OnRequestEnd_Implementation()
{
	OriginTransform = GetOriginWorldTransform();
	bIsAvailableItem = CalcAvailableTargetTransform(ItemTargetTransform);

	UKMIndicatorSubsystem* indicatorSubsystem = UKMIndicatorSubsystem::GetIndicatorSubsystem(this);
	check(IsValid(indicatorSubsystem));

	indicatorSubsystem->RemoveMeshComponent(PlacementMeshcomponentId);

	if (!bIsAvailableItem || TestOverlapWorld(ItemTargetTransform))
	{
		return;
	}

	if (UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance())
	{
		check(IsValid(ownerGameObjectInstance->GetSkillHandler()));
		ownerGameObjectInstance->GetSkillHandler()->TriggerEvent(FKMGameplayTagName::Event_PutDown_Tag);
		ownerGameObjectInstance->SetDirection(ownerGameObjectInstance->GetInteractionDirection());
	}
	
	PlayMartialArts(nullptr, Rate, false);
}

bool UKMAbilityPutDown::TestOverlapWorld(const FTransform& testWorldTransform) const
{
	TArray<AActor*> outActors;
	bool bOverlapResult = UKismetSystemLibrary::CapsuleOverlapActors(GetWorld(), testWorldTransform.GetLocation(), 80.f, 40.f, ObjectTypeQueries, AActor::StaticClass(), ActorsToIgnore,outActors);
	
	return bOverlapResult && !outActors.IsEmpty();
}

bool UKMAbilityPutDown::CalcAvailableTargetTransform(FTransform& outAvailableTargetTransform) const
{
	const float halfAngle = SearchAngleRange * 0.5f;
	
	for (float angle = 0.f; angle <= halfAngle; angle += SearchAngleInterval)
	{
		const int32 maxStep = FMath::IsNearlyZero(angle) ? 1 : 2;
		for (int32 stepIndex = 0; stepIndex < maxStep; ++stepIndex)
		{
			const float signedAngle = angle * (stepIndex == 0 ? 1.f : -1.f);

			FTransform rotationTransform(FRotator(0.f, signedAngle, 0.f));
			FTransform testComponentTransform = ItemPivotBoneTransform * rotationTransform;
			FTransform testWorldTransform = testComponentTransform * OriginTransform;
			
			if (!TestOverlapWorld(testWorldTransform))
			{
				outAvailableTargetTransform = testWorldTransform;
				return true;
			}
		}
	}
	return false;
}

FTransform UKMAbilityPutDown::GetOriginWorldTransform() const
{
	if (AKMCharacter* ownerCharacter = GetOwnerCharacter())
	{
		check(IsValid(ownerCharacter->GetMesh()));

		FTransform outTransform = ownerCharacter->GetMesh()->GetComponentToWorld();
		if (UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(ownerCharacter->GetMesh()->GetAnimInstance()))
		{
			outTransform.SetRotation(FRotator(0.f, FMath::Lerp(-180.f, 180.f, animInstance->GetCurrentDirection()) + 90.f, 0.f).Quaternion());
		}

		return outTransform;
	}
	
	if (AActor* ownerActor = GetOwnerActor())
	{
		return ownerActor->GetActorTransform();
	}
	
	return FTransform::Identity;
}

void UKMAbilityPutDown::Tick(float deltaTime)
{
	AActor* ownerActor = GetOwnerActor();
	if (!IsValid(ownerActor))
	{
		return;
	}
	
	IKMPawnInterface* ownerPawnInteface = Cast<IKMPawnInterface>(ownerActor);
	if (!ownerPawnInteface)
	{
		return;
	}

	UKMGameObjectInstance* ownerGameObjectInstance = ownerPawnInteface->GetGameObjectInstance();
	if (!IsValid(ownerGameObjectInstance))
	{
		return;
	}

	if (!FMath::IsNearlyEqual(ownerGameObjectInstance->GetInteractionDirection(), beforeInteractionDirection))
	{
		float angle = ownerGameObjectInstance->GetInteractionDirection() * PI * 2.f;

		FVector newForwardVector(FMath::Cos(angle), FMath::Sin(angle),0.f);

		FRotator newRotation = newForwardVector.Rotation();
		
		ownerActor->SetActorRotation(newRotation);
		
		if (UKMCharacterInstance* ownerCharacterInstance = Cast<UKMCharacterInstance>(ownerGameObjectInstance))
		{
			if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor))
			{
				ownerCharacter->GetRootComponent()->UpdateComponentToWorld();
				ownerCharacterInstance->SetDirectionVisual(ownerGameObjectInstance->GetInteractionDirection());
				
				if (!ownerCharacter->GetMesh()->IsPostEvaluatingAnimation())
				{
					ownerCharacter->GetMesh()->RefreshBoneTransforms();
				}
			}
		}
		beforeInteractionDirection = ownerGameObjectInstance->GetInteractionDirection();
	}
	OriginTransform = GetOriginWorldTransform();
	if (CalcAvailableTargetTransform(ItemTargetTransform))
	{
		UKMIndicatorSubsystem* indicatorSubsystem = UKMIndicatorSubsystem::GetIndicatorSubsystem(this);
		check(IsValid(indicatorSubsystem));

		if (UMeshComponent* placementComponent = indicatorSubsystem->GetMeshComponent(PlacementMeshcomponentId))
		{
			placementComponent->GetOwner()->SetActorLocation(ItemTargetTransform.GetLocation());
		}
	}
}
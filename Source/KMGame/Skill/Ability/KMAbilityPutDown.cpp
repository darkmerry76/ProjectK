#include "KMAbilityPutDown.h"
#include "Animation/KMAnimInstance.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameObject/KMGameObjectInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Skill/KMSkillHandler.h"
#include "System/KMIndicatorSubsystem.h"
#include "Util/KMUtil.h"

UKMAbilityPutDown::UKMAbilityPutDown(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilityPutDown::Activate()
{
	UKMAbility::Activate();

	check(SearchAngleInterval > 0.f);

	ActorsToIgnore.Reset();

	AActor* ownerActor = GetOwnerActor();
	if (!IsValid(ownerActor))
	{
		return;
	}

	UKMGameObjectInstance* carriedGameObjectInstance = GetTargetGameObjectInstance();
	if (!IsValid(carriedGameObjectInstance))
	{
		return;
	}

	AActor* carriedActor = carriedGameObjectInstance->GetOwnerActor();
	if (!IsValid(carriedActor))
	{
		return;
	}
	
	ActorsToIgnore.Emplace(ownerActor);
	ActorsToIgnore.Emplace(carriedActor);

	OriginTransform = GetOriginWorldTransform();
	ItemPivotBoneTransform = GetMontageComponentSpaceBoneTransform(GetOwnerCharacter(), PutDownMontageInstanceTag, ItemPivotExtractTime, ItemPivotBoneName, false);
	ItemPivotBoneTransform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
	ItemPivotBoneTransform.SetLocation(FVector(0.f, ItemPivotBoneTransform.GetLocation().Size2D(), ItemPivotBoneTransform.GetLocation().Z));

	bIsAvailableItem = CalcAvailableTargetTransform(ItemTargetTransform);

	if (IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(carriedActor))
	{
		UMeshComponent* originPlacementMeshComponent = pawnInterface->GetPlacementMeshComponent();
		if (IsValid(originPlacementMeshComponent))
		{
			UKMIndicatorSubsystem* indicatorSubsystem = UKMIndicatorSubsystem::GetIndicatorSubsystem(this);
			check(IsValid(indicatorSubsystem));

			FTransform newTransform = pawnInterface->GetCarryOffsetTransform_Implementation() * ItemTargetTransform;
			newTransform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
			PlacementMeshcomponentId = indicatorSubsystem->SpawnPlacementMeshComponent(originPlacementMeshComponent, newTransform);
			if (UMeshComponent* newPlacementComponent = indicatorSubsystem->GetMeshComponent(PlacementMeshcomponentId))
			{
				FTransform newRelativeTransform = originPlacementMeshComponent->GetRelativeTransform();
				newRelativeTransform.SetScale3D(originPlacementMeshComponent->GetComponentScale() * 1.05f);
				newPlacementComponent->SetRelativeTransform(newRelativeTransform);
			}
		}
	}

	PostActivated();
}

void UKMAbilityPutDown::Deactivate(bool bCancel)
{
	Super::Deactivate(bCancel);

	UKMIndicatorSubsystem* indicatorSubsystem = UKMIndicatorSubsystem::GetIndicatorSubsystem(this);
	check(IsValid(indicatorSubsystem));

	indicatorSubsystem->RemoveMeshComponent(PlacementMeshcomponentId);
}

void UKMAbilityPutDown::OnRequestEnd_Implementation()
{
	if (!bIsAvailableItem || TestOverlapWorld(ItemTargetTransform))
	{
		ForceComplate();
		return;
	}

	if (UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance())
	{
		check(IsValid(ownerGameObjectInstance->GetSkillHandler()));
		ownerGameObjectInstance->GetSkillHandler()->TriggerEvent(FKMGameplayTagName::Event_PutDown_Tag);
		
		FVector targetToDirection = ItemTargetTransform.GetLocation() - OriginTransform.GetLocation();
		targetToDirection.Z = 0.0f;
		targetToDirection.Normalize();

		ownerGameObjectInstance->SetDirection(UKMUtil::GetCircularAngle2D(FVector2D(targetToDirection)));
	}
	
	PlayMartialArts(nullptr, Rate, false);

	bIsPutDowned = true;
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

AActor* UKMAbilityPutDown::GetTargetActor() const
{
	UKMGameObjectInstance* targetGameObjectInstance = GetTargetGameObjectInstance();
	if (!IsValid(targetGameObjectInstance))
	{
		return nullptr;
	}

	return targetGameObjectInstance->GetOwnerActor();
}

UKMGameObjectInstance* UKMAbilityPutDown::GetTargetGameObjectInstance() const
{
	UKMGameObjectInstance* ownerGameObjectInstance = GetOwnerGameObjectInstance();
	if (!IsValid(ownerGameObjectInstance))
	{
		return nullptr;
	}

	return ownerGameObjectInstance->GetCarriedGameObjectInstance();
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

bool UKMAbilityPutDown::StartPutDown(FName attackSocket, float blendingDuration)
{
	AActor* ownerActor = GetOwnerActor();
	if (!IsValid(ownerActor))
	{
		return false;
	}
	
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(ownerActor);
	if (!ownerPawnInterface)
	{
		return false;
	}

	UKMGameObjectInstance* carriedGameObjectInstance = GetTargetGameObjectInstance();
	if (!IsValid(carriedGameObjectInstance))
	{
		return false;
	}

	AActor* carriedActor = carriedGameObjectInstance->GetOwnerActor();
	if (!IsValid(carriedActor))
	{
		return false;
	}
	
	IKMPawnInterface* carriedPawnInterface = Cast<IKMPawnInterface>(carriedActor);
	if (!carriedPawnInterface)
	{
		return false;
	}
	
	carriedActor->SetActorLocation(ItemTargetTransform.GetLocation());
	
	FTransform carriedWorldTransform = ItemTargetTransform;
	carriedWorldTransform.SetRotation(FRotator(0.f, 0.f, 0.f).Quaternion());
	carriedActor->AttachToComponent(ownerActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor))
	{
		if (UKMAttachedBlendingComponent* carriedAttachedBlendingComponent = carriedPawnInterface->GetAttachedBlendingComponent())
		{
			carriedAttachedBlendingComponent->StartBlending(ownerCharacter->GetMesh(), attackSocket, carriedWorldTransform, blendingDuration * -1.f);
		}
	}
	
	OnStartPutDown();
	return true;
}

void UKMAbilityPutDown::CompletePutDown()
{
	UKMGameObjectInstance* carriedGameObjectInstance = GetTargetGameObjectInstance();
	if (!IsValid(carriedGameObjectInstance))
	{
		return;
	}

	AActor* carriedActor = carriedGameObjectInstance->GetOwnerActor();
	if (!IsValid(carriedActor))
	{
		return;
	}
	
	if (IKMPawnInterface* carriedPawnInterface = Cast<IKMPawnInterface>(carriedActor))
	{
		UKMAttachedBlendingComponent* carriedAttachedBlendingComponent = carriedPawnInterface->GetAttachedBlendingComponent();
		if (IsValid(carriedAttachedBlendingComponent))
		{
			carriedAttachedBlendingComponent->StopBlending();
		}
		carriedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	carriedGameObjectInstance->ComplatePutdowned(GetOwnerGameObjectInstance());
}

void UKMAbilityPutDown::Tick(float deltaTime)
{
	if (bIsPutDowned)
	{
		return;
	}
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

	FTransform tempItemTargetTransform;
	if (CalcAvailableTargetTransform(tempItemTargetTransform))
	{
		bIsAvailableItem = true;
		ItemTargetTransform = tempItemTargetTransform;	
	}
	
	UKMIndicatorSubsystem* indicatorSubsystem = UKMIndicatorSubsystem::GetIndicatorSubsystem(this);
	check(IsValid(indicatorSubsystem));

	if (UMeshComponent* placementComponent = indicatorSubsystem->GetMeshComponent(PlacementMeshcomponentId))
	{
		placementComponent->GetOwner()->SetActorHiddenInGame(!bIsAvailableItem);
		placementComponent->GetOwner()->SetActorLocation(ItemTargetTransform.GetLocation());
	}
}
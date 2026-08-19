#include "KMAnimNotifyState_PairBlend.h"
#include "Character/KMCharacter.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Skill/Ability/KMAbilityPaired.h"

UKMAnimNotifyState_PairBlend::UKMAnimNotifyState_PairBlend(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	SetGroupType(EEMNotifyGroupType::Animation);
#endif
}

FString UKMAnimNotifyState_PairBlend::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	return notifyName;
}

void UKMAnimNotifyState_PairBlend::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	if (!bIsStartBlend)
	{
		return;
	}
	
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	UKMCharacterMovementComponent* ownerCharacterMovementComponent = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
	if (!IsValid(ownerCharacterMovementComponent))
	{
		return;
	}

	UKMCurveWarpingComponent* curveWarpingComponent = Cast<UKMCurveWarpingComponent>(ownerCharacter->GetCurveWarping());
	if (!IsValid(curveWarpingComponent))
	{
		return;
	}

	if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
	{
		if (UKMAbilityPaired* abilityPaired = Cast<UKMAbilityPaired>(martialArtsData->GetAbility()))
		{
			AKMCharacter* casterCharacter = abilityPaired->GetCasterCharacter();
			if (!IsValid(casterCharacter))
			{
				return;
			}
			UKMCharacterMovementComponent* casterCharacterMovementComponent = Cast<UKMCharacterMovementComponent>(casterCharacter->GetCharacterMovement());
			if (!IsValid(casterCharacterMovementComponent))
			{
				return;
			}
			
			FVector warpLocation = casterCharacter->GetActorLocation() + (casterCharacter->GetActorForwardVector() * PairOffset.X) + (casterCharacter->GetActorRightVector() * PairOffset.Y);
			curveWarpingComponent->PlayLinearWarp(warpLocation, 0.1f);

			casterCharacterMovementComponent->RegisterMoveBlockReflection(ownerCharacter);			
		}
	}
}

void UKMAnimNotifyState_PairBlend::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
}

void UKMAnimNotifyState_PairBlend::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(meshComp->GetOwner());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	UKMCharacterMovementComponent* characterMovementComponent = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
	if (!IsValid(characterMovementComponent))
	{
		return;
	}

	if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
	{
		if (UKMAbilityPaired* abilityPaired = Cast<UKMAbilityPaired>(martialArtsData->GetAbility()))
		{
			AKMCharacter* casterCharacter = abilityPaired->GetCasterCharacter();
			if (!IsValid(casterCharacter))
			{
				return;
			}
			UKMCharacterMovementComponent* casterCharacterMovementComponent = Cast<UKMCharacterMovementComponent>(casterCharacter->GetCharacterMovement());
			if (!IsValid(casterCharacterMovementComponent))
			{
				return;
			}
			casterCharacterMovementComponent->UnregisterMoveBlockReflection(ownerCharacter);
		}
	}
}
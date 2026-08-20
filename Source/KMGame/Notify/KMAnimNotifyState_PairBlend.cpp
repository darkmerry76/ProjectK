#include "KMAnimNotifyState_PairBlend.h"
#include "Animation/KMAnimInstance.h"
#include "Character/KMCharacter.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Skill/Ability/KMAbilityPaired.h"
#include "Skill/Ability/KMAbilitySkill.h"
#include "Util/KMUtil.h"

bool FKMAnimNotifyState_Pair_Context::IsValid() const
{
	return LeaderCharacterInstance && LeaderMontageInstance && FollowerCharacterInstance && FollowerMontageInstance;	
}

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

void UKMAnimNotifyState_PairBlend::FollowAnimation(const USkeletalMeshComponent* meshComp, const FAnimNotifyEventReference& eventReference) const
{
	const TSharedPtr<FKMAnimNotifyState_Pair_Context>* existPairContext = PairContexts.Find(meshComp);
	if (!existPairContext)
	{
		return;
	}
	
	TSharedPtr<FKMAnimNotifyState_Pair_Context> pairContext = *existPairContext;
	if (!pairContext.IsValid())
	{
		return;
	}
	if (!pairContext->IsValid())
	{
		const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>();
		if (!martialArtsData)
		{
			return;
		}

		TArray<AKMCharacter*> characters;
		if (UKMAbilitySkill* abilitySkill = Cast<UKMAbilitySkill>(martialArtsData->GetAbility()))
		{
			characters.Emplace(abilitySkill->GetOwnerCharacter());
			characters.Emplace(abilitySkill->GetTargetCharacter());
		}
		else if (UKMAbilityEffect* abilityEffect = Cast<UKMAbilityEffect>(martialArtsData->GetAbility()))
		{
			characters.Emplace(abilityEffect->GetOwnerCharacter());
			characters.Emplace(abilityEffect->GetCasterCharacter());
		}
		
		for (auto characterItr : characters)
		{
			if (FAnimMontageInstance* leaderMontageInstance = UKMUtil::FindMontageInstaceTagByCharacter(characterItr, LeaderMontageInstanceTag))
			{
				pairContext->LeaderCharacterInstance = characterItr->GetCharacterInstance();
				pairContext->LeaderMontageInstance = leaderMontageInstance;
			}
			else if (FAnimMontageInstance* followMontageInstance = UKMUtil::FindMontageInstaceTagByCharacter(characterItr, FollowMontageInstanceTag))
			{
				pairContext->FollowerCharacterInstance = characterItr->GetCharacterInstance();
				pairContext->FollowerMontageInstance = followMontageInstance;
			}
		}
	}

	if (!pairContext->IsValid())
	{
		return;
	}

	if (pairContext->FollowerMontageInstance->GetMontageSyncLeader() != pairContext->LeaderMontageInstance)
	{
		pairContext->FollowerMontageInstance->MontageSync_Follow(pairContext->LeaderMontageInstance);
	}

	pairContext->FollowerCharacterInstance->SetTimeDilation(TEXT("Follow"), pairContext->LeaderCharacterInstance->GetTimeDilation());
}

void UKMAnimNotifyState_PairBlend::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
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

	TSharedPtr<FKMAnimNotifyState_Pair_Context> newPairContext = MakeShared<FKMAnimNotifyState_Pair_Context>();
	PairContexts.Emplace(meshComp,newPairContext);

	if (bIsStartBlend)
	{
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
}

void UKMAnimNotifyState_PairBlend::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (LeaderMontageInstanceTag != NAME_None && FollowMontageInstanceTag != NAME_None)
	{
		FollowAnimation(meshComp, eventReference);
	}
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

	const TSharedPtr<FKMAnimNotifyState_Pair_Context>* pairContext = PairContexts.Find(meshComp);

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

	if (pairContext && (*pairContext)->IsValid())
	{
		(*pairContext)->LeaderMontageInstance->MontageSync_StopLeading();
		(*pairContext)->FollowerMontageInstance->MontageSync_StopFollowing();
		(*pairContext)->FollowerCharacterInstance->RemoveTimeDilation(TEXT("Follow"));
	}
}
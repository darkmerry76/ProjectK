#include "KMAnimNotifyState_PairBlend.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
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
			else 
			{
				FAnimMontageInstance* followMontageInstance = UKMUtil::FindMontageInstaceTagByCharacter(characterItr, FollowMontageInstanceTag);
				if (!followMontageInstance)
				{
					followMontageInstance = UKMUtil::GetActiveMontageInstance(characterItr);
				}
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

	UKMCurveWarpingComponent* curveWarpingComponent = Cast<UKMCurveWarpingComponent>(ownerCharacter->GetCurveWarpingComponent());
	if (!IsValid(curveWarpingComponent))
	{
		return;
	}

	TSharedPtr<FKMAnimNotifyState_Pair_Context> newPairContext = MakeShared<FKMAnimNotifyState_Pair_Context>();
	PairContexts.Emplace(meshComp,newPairContext);

	if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
	{
		if (UKMAbilityEffect* abilityEffect = Cast<UKMAbilityEffect>(martialArtsData->GetAbility()))
		{
			AKMCharacter* casterCharacter = abilityEffect->GetCasterCharacter();
			if (!IsValid(casterCharacter))
			{
				return;
			}
			UKMCharacterMovementComponent* casterCharacterMovementComponent = Cast<UKMCharacterMovementComponent>(casterCharacter->GetCharacterMovement());
			if (!IsValid(casterCharacterMovementComponent))
			{
				return;
			}

			if (bIsStartBlend)
			{
				FVector warpLocation = casterCharacter->GetActorLocation() + (casterCharacter->GetActorForwardVector() * PairOffset.X) + (casterCharacter->GetActorRightVector() * PairOffset.Y);
				curveWarpingComponent->PlayLinearWarp(warpLocation, 0.1f);
			}
			if (bIsBlockReflection)
			{
				casterCharacterMovementComponent->RegisterMoveBlockReflection(ownerCharacter);
			}
		}
	}
}

void UKMAnimNotifyState_PairBlend::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (LeaderMontageInstanceTag != NAME_None)
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
		if (UKMAbilityEffect* abilityEffect = Cast<UKMAbilityEffect>(martialArtsData->GetAbility()))
		{
			AKMCharacter* casterCharacter = abilityEffect->GetCasterCharacter();
			if (!IsValid(casterCharacter))
			{
				return;
			}
			UKMCharacterMovementComponent* casterCharacterMovementComponent = Cast<UKMCharacterMovementComponent>(casterCharacter->GetCharacterMovement());
			if (!IsValid(casterCharacterMovementComponent))
			{
				return;
			}
			if (bIsBlockReflection)
			{
				casterCharacterMovementComponent->UnregisterMoveBlockReflection(ownerCharacter);
			}
		}
	}

	if (pairContext && (*pairContext)->IsValid())
	{
		if ((*pairContext)->FollowerMontageInstance->GetMontageSyncLeader() == (*pairContext)->LeaderMontageInstance)
		{
			(*pairContext)->FollowerMontageInstance->MontageSync_StopFollowing();
			(*pairContext)->LeaderMontageInstance->MontageSync_StopLeading();
			(*pairContext)->FollowerCharacterInstance->RemoveTimeDilation(TEXT("Follow"));
		}
	}

	PairContexts.Remove(meshComp);
}
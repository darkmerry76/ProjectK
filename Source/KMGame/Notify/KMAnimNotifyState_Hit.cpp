#include "KMAnimNotifyState_Hit.h"
#include "Component/KMEditorDrawDebugComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/Ability/KMAbility.h"
#include "Skill/Ability/KMAbilityEffect.h"
#include "Skill/Ability/KMAbilitySkill.h"

UKMAnimNotifyState_Hit::UKMAnimNotifyState_Hit(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetGroupType(EEMNotifyGroupType::Plan);
}

FString UKMAnimNotifyState_Hit::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (!SocketName.IsValid())
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *SocketName.ToString());		
	}
	return notifyName;
}

void UKMAnimNotifyState_Hit::GetFinalTransform(const USceneComponent* ownerComponent, FTransform& outTransform) const
{
	if (!IsValid(ownerComponent))
	{
		return;
	}
	outTransform = ownerComponent->GetSocketTransform(SocketName);
	if (!FollowSocketRotation)
	{
		outTransform.SetRotation(ownerComponent->GetOwner()->GetActorTransform().GetRotation());
	}
	
	outTransform.SetRotation(outTransform.GetRotation() * HitTransform.GetRotation());
	outTransform.SetLocation(outTransform.GetLocation() + (outTransform.GetRotation().RotateVector(HitTransform.GetLocation())));
	outTransform.SetScale3D(HitTransform.GetScale3D());
}

void UKMAnimNotifyState_Hit::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(meshComp))
	{
		HitCheckClear(meshComp->GetOwner());
		
		FTransform finalTransform;
		GetFinalTransform(meshComp, finalTransform);
		Context.FindOrAdd(meshComp->GetOwner()).PreviousTransform = finalTransform;
	}
}

void UKMAnimNotifyState_Hit::NotifyBeginEx(AActor* actor, UEMMartialArts* martialArts, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(actor))
	{
		HitCheckClear(actor);
		
		FTransform finalTransform;
		GetFinalTransform(actor->GetRootComponent(), finalTransform);
		Context.FindOrAdd(actor).PreviousTransform = finalTransform;
	}
}

void UKMAnimNotifyState_Hit::DoHit(const USceneComponent* ownerComponent, const FAnimNotifyEventReference& eventReference)
{
	AActor* ownerActor = ownerComponent->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}
	
	FKMAnimNotifyState_Hit_Context* hitContext = Context.Find(ownerActor);
	if (!hitContext)
	{
		return;
	}

	if (bIsOnce && hitContext->HitResults.Num() > 0)
	{
		return;
	}
	
	IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(ownerActor);
	if (!pawnInterface)
	{
		return;
	}

	UKMGameObjectInstance* ownerGameObjectInstance = pawnInterface->GetGameObjectInstance();
	if (!IsValid(ownerGameObjectInstance))
	{
		return;
	}

	TWeakPtr<FKMSkillInstance> latestSkillInstance = nullptr;

	if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
	{
		if (UKMAbilitySkill* abilitySkill = Cast<UKMAbilitySkill>(martialArtsData->GetAbility()))
		{
			latestSkillInstance = abilitySkill->GetSkillInstance();
			abilitySkill->ClearHitResults();
		}
		else if (UKMAbilityEffect* abilityEffect = Cast<UKMAbilityEffect>(martialArtsData->GetAbility()))
		{
			if (abilityEffect->GetSkillEffectInstance())
			{
				latestSkillInstance = abilityEffect->GetSkillEffectInstance()->GetOwnerSkillInstance();
			}
		}
	}
	if (!latestSkillInstance.IsValid() && IsValid(ownerGameObjectInstance->GetSkillHandler()))
	{
		latestSkillInstance = ownerGameObjectInstance->GetSkillHandler()->GetLatestActiveSkillInstance();
	}
	
	FTransform finalTransform;
	GetFinalTransform(ownerComponent, finalTransform);

	TArray<FHitResult> hitResults;
	if (CollisonType == EKMCollisonType::Box)
	{
		ownerGameObjectInstance->BoxHitImpact(latestSkillInstance, hitContext->PreviousTransform, finalTransform, ObjectTypeQuery, ActorClassFilter, bIsOnce, bIsOnlyHitTest, HitTag, hitResults);
	}
	else
	{
		ownerGameObjectInstance->SphereHitImpact(latestSkillInstance, hitContext->PreviousTransform, finalTransform, ObjectTypeQuery, ActorClassFilter, bIsOnce, bIsOnlyHitTest, HitTag, hitResults);
	}
	
	if (!hitResults.IsEmpty() && bIsAppendSkillHitResult)
	{
		hitContext->HitResults.Append(hitResults);
	}
	hitContext->PreviousTransform = finalTransform;
}

void UKMAnimNotifyState_Hit::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(meshComp) && bIsEnable)
	{
		DoHit(meshComp, eventReference);
	}
}

void UKMAnimNotifyState_Hit::NotifyTickEx(class AActor* actor, class UEMMartialArts* martialArts, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(actor) && bIsEnable)
	{
		DoHit(actor->GetRootComponent(), eventReference);
	}
}

void UKMAnimNotifyState_Hit::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	if(!IsValid(meshComp))
	{
		return;
	}
	
	AActor* ownerActor = meshComp->GetOwner();

	if (bIsHitCheckerClear)
	{
		HitCheckClear(ownerActor);
	}

	if (FKMAnimNotifyState_Hit_Context* hitContext = Context.Find(ownerActor))
	{
		if (!hitContext->HitResults.IsEmpty())
		{
			if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
			{
				if (UKMAbilitySkill* abilitySkill = Cast<UKMAbilitySkill>(martialArtsData->GetAbility()))
				{
					abilitySkill->AppendHitResult(hitContext->HitResults);
				}
			}
		}
	}

	Context.Remove(ownerActor);
}

void UKMAnimNotifyState_Hit::NotifyEndEx(AActor* actor, UEMMartialArts* martialArts, const FAnimNotifyEventReference& eventReference)
{
	Context.Remove(actor);

	if (bIsHitCheckerClear)
	{
		HitCheckClear(actor);
	}
}

void UKMAnimNotifyState_Hit::HitCheckClear(AActor* actor)
{
	if (IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(actor))
	{
		if (UKMGameObjectInstance* ownerGameObjectInstance = pawnInterface->GetGameObjectInstance())
		{
			ownerGameObjectInstance->HitCheckClear();
		}
	}
}

#if WITH_EDITOR
void UKMAnimNotifyState_Hit::DrawInEditor(FPrimitiveDrawInterface* pDI, USkeletalMeshComponent* meshComp, const UAnimSequenceBase* animation, const FAnimNotifyEvent& notifyEvent) const
{
	AActor* ownerActor = meshComp->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}
	
	FTransform socketTransform = meshComp->GetSocketTransform(SocketName);
	if (!FollowSocketRotation)
	{
		socketTransform.SetRotation(ownerActor->GetActorTransform().GetRotation());
	}
	socketTransform.SetRotation(socketTransform.GetRotation() * HitTransform.GetRotation());
	socketTransform.SetLocation(socketTransform.GetLocation() + (socketTransform.GetRotation().RotateVector(HitTransform.GetLocation())));

	if (CollisonType == EKMCollisonType::Box)
	{
		DrawWireBox(pDI, socketTransform.ToMatrixWithScale(), FBox(HitTransform.GetScale3D() * -1.f, HitTransform.GetScale3D()), FColor::Red,SDPG_World);
	}
	else if (CollisonType == EKMCollisonType::Sphere)
	{
		DrawWireSphere(pDI, socketTransform.GetLocation(), FColor::Red, HitTransform.GetScale3D().X * 100.f, 32, SDPG_World);
	}
}
#endif

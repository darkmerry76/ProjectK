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

void UKMAnimNotifyState_Hit::GetFinalTransform(const AKMCharacter* ownerCharacter, const USkeletalMeshComponent* meshComp, FTransform& outTransform) const
{
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	outTransform = meshComp->GetSocketTransform(SocketName);
	if (!FollowSocketRotation)
	{
		outTransform.SetRotation(ownerCharacter->GetActorTransform().GetRotation());
	}
	
	outTransform.SetRotation(outTransform.GetRotation() * HitTransform.GetRotation());
	outTransform.SetLocation(outTransform.GetLocation() + (outTransform.GetRotation().RotateVector(HitTransform.GetLocation())));
	outTransform.SetScale3D(HitTransform.GetScale3D());
}

void UKMAnimNotifyState_Hit::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	AActor* ownerActor = meshComp->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}

#if WITH_EDITOR
	EditorDrawDebugComponent = ownerActor->FindComponentByClass<UKMEditorDrawDebugComponent>();
	if (!IsValid(EditorDrawDebugComponent) && !meshComp->GetWorld()->IsGameWorld())
	{
		EditorDrawDebugComponent = Cast<UKMEditorDrawDebugComponent>(ownerActor->AddComponentByClass(UKMEditorDrawDebugComponent::StaticClass(), false, FTransform::Identity, false));
	}
#endif
	
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor);
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	FTransform finalTransform;
	GetFinalTransform(ownerCharacter, meshComp, finalTransform);
	HitPreviousTransforms.FindOrAdd(meshComp) = finalTransform;
}

void UKMAnimNotifyState_Hit::NotifyTick(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference)
{
	AActor* ownerActor = meshComp->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor);
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UKMCharacterInstance* ownerCharacterInstance = ownerCharacter->GetCharacterInstance();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	TWeakPtr<FKMSkillInstance> latestSkillInstance = nullptr;
	if (const FKMMartialArtsSkillContextData* martialArtsData = eventReference.GetContextData<FKMMartialArtsSkillContextData>())
	{
		if (UKMAbilitySkill* abilitySkill = Cast<UKMAbilitySkill>(martialArtsData->GetAbility()))
		{
			latestSkillInstance = abilitySkill->GetSkillInstance();
		}
		else if (UKMAbilityEffect* abilityEffect = Cast<UKMAbilityEffect>(martialArtsData->GetAbility()))
		{
			if (abilityEffect->GetSkillEffectInstance())
			{
				latestSkillInstance = abilityEffect->GetSkillEffectInstance()->GetOwnerSkillInstance();
			}
		}
	}
	if (!latestSkillInstance.IsValid() && IsValid(ownerCharacterInstance->GetSkillHandler()))
	{
		latestSkillInstance = ownerCharacterInstance->GetSkillHandler()->GetLatestActiveSkillInstance();
	}
	
	FTransform& previousTransform = HitPreviousTransforms.FindOrAdd(meshComp);
	
	FTransform finalTransform;
	GetFinalTransform(ownerCharacter, meshComp, finalTransform);

	if (CollisonType == EKMCollisonType::Box)
	{
		ownerCharacterInstance->BoxHitImpact(latestSkillInstance, previousTransform, finalTransform, ObjectTypeQuery, ActorClassFilter, HitTag);
	}
	else
	{
		ownerCharacterInstance->SphereHitImpact(latestSkillInstance, previousTransform, finalTransform, ObjectTypeQuery, ActorClassFilter, HitTag);
	}
	previousTransform = finalTransform;
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

void UKMAnimNotifyState_Hit::NotifyEnd(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
#if WITH_EDITOR
	AActor* ownerActor = meshComp->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}
	EditorDrawDebugComponent = ownerActor->FindComponentByClass<UKMEditorDrawDebugComponent>();
	if (IsValid(EditorDrawDebugComponent))
	{
		EditorDrawDebugComponent->bAllowAnyoneToDestroyMe = true;
		EditorDrawDebugComponent->DestroyComponent();
	}
#endif

	HitPreviousTransforms.Remove(meshComp);

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor);
	if (IsValid(ownerCharacter))
	{
		if (UKMCharacterInstance* ownerCharacterInstance = ownerCharacter->GetCharacterInstance())
		{
			ownerCharacterInstance->HitCheckClear();
		}
	}
}
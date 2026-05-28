#include "KMAnimNotifyState_Hit.h"
#include "Character/KMCharacter.h"
#include "Component/KMEditorDrawDebugComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/Ability/KMAbility.h"
#include "Skill/Ability/KMAbilityEffect.h"
#include "System/KMTargetSubsystem.h"

UKMAnimNotifyState_Hit::UKMAnimNotifyState_Hit(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	ObjectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ActorClassFilter = ACharacter::StaticClass();
}

void UKMAnimNotifyState_Hit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AActor* ownerActor = MeshComp->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}

#if WITH_EDITOR
	EditorDrawDebugComponent = ownerActor->FindComponentByClass<UKMEditorDrawDebugComponent>();
	if (!IsValid(EditorDrawDebugComponent))
	{
		EditorDrawDebugComponent = Cast<UKMEditorDrawDebugComponent>(ownerActor->AddComponentByClass(UKMEditorDrawDebugComponent::StaticClass(), false, FTransform::Identity, false));
	}
#endif

	HitCheckPair.FindOrAdd(MeshComp) = FKMHitCheckData();
}

void UKMAnimNotifyState_Hit::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	AActor* ownerActor = MeshComp->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}

	UKMSkillHandler* ownerCharacterSkillHandler = nullptr;
	UKMCharacterInstance* ownerCharacterInstance = nullptr;
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor);
	if (IsValid(ownerCharacter))
	{
		ownerCharacterInstance = ownerCharacter->GetCharacterInstance();
		check(IsValid(ownerCharacterInstance));
		
		ownerCharacterSkillHandler = ownerCharacterInstance->GetSkillHandler();
	}
	
	if (!IsValid(ownerCharacterSkillHandler))
	{
		return;
	}

	if (!ownerCharacterSkillHandler->GetLatestActiveSkillInstance().IsValid())
	{
		return;
	}
	
	FTransform socketTransform = MeshComp->GetSocketTransform(SocketName);
	if (!FollowSocketRotation)
	{
		socketTransform.SetRotation(ownerActor->GetActorTransform().GetRotation());
	}
	socketTransform.SetRotation(socketTransform.GetRotation() * HitTransform.GetRotation());
	socketTransform.SetLocation(socketTransform.GetLocation() + (socketTransform.GetRotation().RotateVector(HitTransform.GetLocation())));

	FKMHitCheckData* hitCheckSet = HitCheckPair.Find(MeshComp);
	check(hitCheckSet);
	
	TArray<AActor*> overlapActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Emplace(ownerCharacter);
	if (UKismetSystemLibrary::BoxOverlapActorsWithOrientation(MeshComp, socketTransform.GetLocation(),
		HitTransform.GetScale3D(), socketTransform.GetRotation().Rotator(), ObjectTypeQuery, ActorClassFilter, ActorsToIgnore, overlapActors))
	{
		for (auto actorItr = overlapActors.CreateIterator(); actorItr; ++actorItr)
		{
			if (ownerActor == *actorItr)
			{
				continue;
			}
			if (hitCheckSet->Actors.Contains(*actorItr))
			{
				continue;
			}

			AKMCharacter* hitharacter = Cast<AKMCharacter>(*actorItr);
			if (!IsValid(hitharacter))
			{
				continue;
			}

			UKMCharacterInstance* hitCharacterInstance = hitharacter->GetCharacterInstance();
			check(IsValid(hitCharacterInstance));

			if (hitCharacterInstance->IsDead() ||
				hitCharacterInstance->HasGameplayTag(FKMGameplayTagName::State_Blow_Tag) ||
				hitCharacterInstance->HasGameplayTag(FKMGameplayTagName::State_Intangible_Tag))
			{
				continue;
			}

			hitCheckSet->Actors.FindOrAdd(*actorItr);

			ownerCharacterInstance->Inflict(hitCharacterInstance);
			if (!ownerCharacterSkillHandler->GetLatestActiveSkillInstance().IsValid())
			{
				continue;
			}
			TSharedPtr<FKMSkillInstance> latestSkillInstance = MakeShared<FKMSkillInstance>(*ownerCharacterSkillHandler->GetLatestActiveSkillInstance().Get());
			UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(hitharacter->GetRootComponent());
			FVector closestPoint;
			rootComp->GetClosestPointOnCollision(socketTransform.GetLocation(), closestPoint);

			if (latestSkillInstance.IsValid())
			{
				latestSkillInstance->Target = MakeShared<FKMLockOnCluster>(ownerCharacterInstance);
				latestSkillInstance->Target->Targets.Emplace(hitharacter->GetCharacterInstance()->GetId());
				
				UKMSkillHandler* hitCharacterSkillHandler = hitCharacterInstance->GetSkillHandler();
				check(IsValid(hitCharacterSkillHandler));

				hitCharacterInstance->Hit(ownerCharacterInstance, latestSkillInstance, closestPoint);
			}
		}
	}
}

#if WITH_EDITOR
void UKMAnimNotifyState_Hit::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	AActor* ownerActor = MeshComp->GetOwner();
	if(!IsValid(ownerActor))
	{
		return;
	}
	
	FTransform socketTransform = MeshComp->GetSocketTransform(SocketName);
	if (!FollowSocketRotation)
	{
		socketTransform.SetRotation(ownerActor->GetActorTransform().GetRotation());
	}
	socketTransform.SetRotation(socketTransform.GetRotation() * HitTransform.GetRotation());
	socketTransform.SetLocation(socketTransform.GetLocation() + (socketTransform.GetRotation().RotateVector(HitTransform.GetLocation())));

	DrawWireBox(
		PDI,
		socketTransform.ToMatrixWithScale(),
		FBox(HitTransform.GetScale3D() * -1.f, HitTransform.GetScale3D()),
		FColor::Red,
		SDPG_World);
}
#endif

void UKMAnimNotifyState_Hit::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
#if WITH_EDITOR
	AActor* ownerActor = MeshComp->GetOwner();
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
	HitCheckPair.Remove(MeshComp);
}
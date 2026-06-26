#include "KMAnimNotifyState_Hit.h"
#include "Character/KMCharacter.h"
#include "Component/KMEditorDrawDebugComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/Ability/KMAbility.h"
#include "System/KMTargetSubsystem.h"

UKMAnimNotifyState_Hit::UKMAnimNotifyState_Hit(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	ObjectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ActorClassFilter = ACharacter::StaticClass();

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

void UKMAnimNotifyState_Hit::NotifyBegin(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
	AActor* ownerActor = meshComp->GetOwner();
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

	HitCheckPair.FindOrAdd(meshComp) = FKMHitCheckData();
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
	
	FTransform socketTransform = meshComp->GetSocketTransform(SocketName);
	if (!FollowSocketRotation)
	{
		socketTransform.SetRotation(ownerActor->GetActorTransform().GetRotation());
	}
	socketTransform.SetRotation(socketTransform.GetRotation() * HitTransform.GetRotation());
	socketTransform.SetLocation(socketTransform.GetLocation() + (socketTransform.GetRotation().RotateVector(HitTransform.GetLocation())));
	socketTransform.SetScale3D(HitTransform.GetScale3D());
	ownerCharacterInstance->BoxHitImpact(socketTransform, ObjectTypeQuery, ActorClassFilter);
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

	DrawWireBox(pDI, socketTransform.ToMatrixWithScale(), FBox(HitTransform.GetScale3D() * -1.f, HitTransform.GetScale3D()), FColor::Red,SDPG_World);
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

	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor);
	if (IsValid(ownerCharacter))
	{
		if (UKMCharacterInstance* ownerCharacterInstance = ownerCharacter->GetCharacterInstance())
		{
			ownerCharacterInstance->HitCheckClear();
		}
	}
}
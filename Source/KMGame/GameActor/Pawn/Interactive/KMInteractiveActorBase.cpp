#include "KMInteractiveActorBase.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Component/KMMoveShapeComponent.h"
#include "Component/KMPawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"
#include "Skill/KMSkillTypes.h"
#include "Tables/Generated/KMTable_SkillEffect.h"

AKMInteractiveActorBase::AKMInteractiveActorBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	MoveShapeComponent = CreateDefaultSubobject<UKMMoveShapeComponent>(TEXT("Capsule"));
	SetRootComponent(MoveShapeComponent);

	MovementComponent = CreateDefaultSubobject<UKMPawnMovementComponent>(TEXT("Movement"));
	MovementComponent->SetUpdatedComponent(MoveShapeComponent);

	AttachedComponent = CreateDefaultSubobject<UKMAttachedBlendingComponent>(TEXT("AttachedBlending"));
	AttachedComponent->SetupAttachment(GetRootComponent());

	CurveWarpingComponent  = CreateDefaultSubobject<UKMCurveWarpingComponent>(TEXT("CurveWarping"));
	MartialArtsComponent = CreateDefaultSubobject<UKMMartialArtsComponent>(TEXT("MartialArts"));

	MovementComponent->CustomMovementDelegate.AddUObject(CurveWarpingComponent, &UEMCurveWarpingComponent::OnCustomMovement);
}

UKMMartialArtsComponent* AKMInteractiveActorBase::GetMartialArtsComponent() const
{
	return MartialArtsComponent;
}

UKMCurveWarpingComponent* AKMInteractiveActorBase::GetCurveWarpingComponent() const
{
	return CurveWarpingComponent;
}

UKMInteractiveInstance* AKMInteractiveActorBase::GetInteractiveInstance() const
{
	if (!InteractiveInstance.IsValid())
	{
		return nullptr;
	}
	return InteractiveInstance.Get();
}

UKMGameObjectInstance* AKMInteractiveActorBase::GetGameObjectInstance() const
{
	if (!InteractiveInstance.IsValid())
	{
		return nullptr;
	}
	return InteractiveInstance.Get();
};

void AKMInteractiveActorBase::PossessedByGameObjectInstance(UKMGameObjectInstance* newGameObjectInstance)
{
	InteractiveInstance = Cast<UKMInteractiveInstance>(newGameObjectInstance);
	check(InteractiveInstance.IsValid());
	InteractiveInstance->SetOwnerActor(this);
}

void AKMInteractiveActorBase::OnImpact(const TSharedPtr<FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	check(skillEffectInstance.IsValid() && skillEffectInstance->GetEffectTableRecord());
	Receive_OnImpact(skillEffectInstance->GetEffectTableRecord()->Id, hitClosestPoint, hitTag);
}

void AKMInteractiveActorBase::OnDeath()
{
	Receive_OnDeath();
}

FBoxSphereBounds AKMInteractiveActorBase::GetMasterBounds() const
{
	return FBoxSphereBounds();
}

UPrimitiveComponent* AKMInteractiveActorBase::GetMovementShapeComponent() const
{
	return 	MoveShapeComponent;
}

UKMAttachedBlendingComponent* AKMInteractiveActorBase::GetAttachedComponent() const
{
	return AttachedComponent;
}

UPawnMovementComponent* AKMInteractiveActorBase::GetMovementComponent() const
{
	return MovementComponent;
}
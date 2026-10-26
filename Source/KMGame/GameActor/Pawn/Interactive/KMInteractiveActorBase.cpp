#include "KMInteractiveActorBase.h"
#include "Component/KMMoveShapeComponent.h"
#include "Component/KMPawnMoverComponent.h"
#include "Component/KMWidgetComponent.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"
#include "Skill/KMSkillTypes.h"
#include "Tables/Generated/KMTable_SkillEffect.h"

AKMInteractiveActorBase::AKMInteractiveActorBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	WidgetComponent = CreateDefaultSubobject<UKMWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(GetRootComponent());
}

UKMWidgetComponent* AKMInteractiveActorBase::GetWidgetComponent() const
{
	return WidgetComponent;
}

UKMInteractiveInstance* AKMInteractiveActorBase::GetInteractiveInstance() const
{
	return Cast<UKMInteractiveInstance>(GetGameObjectInstance());
}

float AKMInteractiveActorBase::GetScaledRadius_Implementation() const
{
	return MoveShapeComponent->GetScaledCapsuleRadius();
}

float AKMInteractiveActorBase::GetScaledHalfHeight_Implementation() const
{
	return MoveShapeComponent->GetScaledCapsuleHalfHeight();
}

void AKMInteractiveActorBase::OnImpact(const TSharedPtr<FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	check(skillEffectInstance.IsValid() && skillEffectInstance->GetEffectTableRecord());
	Receive_OnImpact(skillEffectInstance->GetEffectTableRecord()->Id, hitClosestPoint, hitTag);
}

void AKMInteractiveActorBase::OnDeath()
{
	if (IsValid(MoveShapeComponent))
	{
		MoveShapeComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

	if (IsValid(MoverComponent))
	{
		MoverComponent->Deactivate();
	}
	Receive_OnDeath();
}

FBoxSphereBounds AKMInteractiveActorBase::GetMasterBounds() const
{
	return FBoxSphereBounds();
}

FTransform AKMInteractiveActorBase::GetApproachPullPoint(EKMApproachPullPointType approachPullPointType) const
{
	return GetActorTransform();
}
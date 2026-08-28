#include "KMInteractiveActorBase.h"

#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"
#include "Skill/KMSkillTypes.h"
#include "Tables/Generated/KMTable_SkillEffect.h"

AKMInteractiveActorBase::AKMInteractiveActorBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootScene")));
	
	CurveWarping  = CreateDefaultSubobject<UKMCurveWarpingComponent>(TEXT("CurveWarping"));
	MartialArtsComponent = CreateDefaultSubobject<UKMMartialArtsComponent>(TEXT("MartialArts"));
}

UKMMartialArtsComponent* AKMInteractiveActorBase::GetMartialArtsComponent() const
{
	return MartialArtsComponent;
}

UKMCurveWarpingComponent* AKMInteractiveActorBase::GetCurveWarping() const
{
	return CurveWarping;
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

UKMAttachedBlendingComponent* AKMInteractiveActorBase::GetAttachedComponent() const
{
	return nullptr;
}
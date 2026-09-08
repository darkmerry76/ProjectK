#include "KMInteractiveActorBase.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Component/KMMoveShapeComponent.h"
#include "Component/KMPawnMovementComponent.h"
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

UMeshComponent* AKMInteractiveActorBase::GetPlacementMeshComponent() const
{
	return PlacementMeshComponent;
}

UKMAttachedBlendingComponent* AKMInteractiveActorBase::GetAttachedBlendingComponent() const
{
	return AttachedComponent;
}

FTransform AKMInteractiveActorBase::GetCarryOffsetTransform_Implementation() const
{
	return AttachedComponent->GetOffsetTransform();
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
	if (IsValid(MoveShapeComponent))
	{
		MoveShapeComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

	if (IsValid(MovementComponent))
	{
		MovementComponent->Deactivate();
	}
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

void AKMInteractiveActorBase::StartCrarry(UKMGameObjectInstance* carriedGameObjectInstance)
{
	Receive_OnStartCarry(carriedGameObjectInstance);
}

void AKMInteractiveActorBase::StartCrarried(UKMGameObjectInstance* carryGameObjectInstance)
{
	PawnResponse = MoveShapeComponent->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn);
	
	MoveShapeComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Receive_OnStartCarried(carryGameObjectInstance);
}

void AKMInteractiveActorBase::ComplatePutdown(UKMGameObjectInstance* putDownedGameObjectInstance)
{
	Receive_OnComplatePutdown(putDownedGameObjectInstance);
}

void AKMInteractiveActorBase::ComplatePutdowned(UKMGameObjectInstance* putDownGameObjectInstance)
{
	MoveShapeComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, PawnResponse);
	Receive_OnComplatePutdowned(putDownGameObjectInstance);
}

void AKMInteractiveActorBase::LandHit(const FHitResult& hitResult)
{
}

void AKMInteractiveActorBase::WallHit(const FHitResult& hitResult)
{
}

void AKMInteractiveActorBase::CeilingHit(const FHitResult& hitResult)
{
}

const FHitResult AKMInteractiveActorBase::GetLatestWallHitResult_Implementation() const
{
	return LastWallHitResult;	
}
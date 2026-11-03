#include "KMPawnBase.h"
#include "Component/EMMoverCurveWarpingFallingMode.h"
#include "Component/KMAttachedBlendingComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMMartialArtsComponent.h"
#include "Component/KMMoveShapeComponent.h"
#include "Component/KMPawnMoverComponent.h"
#include "Component/KMWidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameObject/KMGameObjectInstance.h"
#include "GameObject/Interactive/KMInteractiveInstance.h"
#include "Skill/KMSkillHandler.h"
#include "Tables/Generated/KMTable_SkillEffect.h"

AKMPawnBase::AKMPawnBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	MoveShapeComponent = CreateDefaultSubobject<UKMMoveShapeComponent>(TEXT("Capsule"));
	SetRootComponent(MoveShapeComponent);

	MoverComponent = CreateDefaultSubobject<UKMPawnMoverComponent>(TEXT("PawnMover"));
	MoverComponent->SetUpdatedComponent(MoveShapeComponent);

	AttachedComponent = CreateDefaultSubobject<UKMAttachedBlendingComponent>(TEXT("AttachedBlending"));
	AttachedComponent->SetupAttachment(GetRootComponent());

	CurveWarpingComponent  = CreateDefaultSubobject<UKMCurveWarpingComponent>(TEXT("CurveWarping"));
	MartialArtsComponent = CreateDefaultSubobject<UKMMartialArtsComponent>(TEXT("MartialArts"));
}

void AKMPawnBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKMPawnBase::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void AKMPawnBase::PostActorCreated()
{
	Super::PostActorCreated();

	if (UEMMoverCurveWarpingFallingMode* moverCurveWarpingFalling =
		Cast<UEMMoverCurveWarpingFallingMode>(MoverComponent->FindMovementModeByName(UEMMoverCurveWarpingFallingMode::ModeName)))
	{
		moverCurveWarpingFalling->BindCurveWarpingComponent(CurveWarpingComponent);

		moverCurveWarpingFalling->OnLanded.AddDynamic(this, &AKMPawnBase::OnMoverLanded);
	}

	if (UFallingMode* moverFallingMode =
		Cast<UFallingMode>(MoverComponent->FindMovementModeByName(TEXT("Falling"))))
	{
		moverFallingMode->OnLanded.AddDynamic(this, &AKMPawnBase::OnMoverLanded);
	}
}

void AKMPawnBase::OnMoverLanded(const FName& nextMovementModeName, const FHitResult& hitResult)
{
	Landed(hitResult);
}

void AKMPawnBase::Landed(const FHitResult& hitResult)
{
	LandHit(hitResult);
}

void AKMPawnBase::LandHit(const FHitResult& hitResult)
{
	if (UKMGameObjectInstance* gameObjectInstance = GetGameObjectInstance())
	{
		if (UKMSkillHandler* skillHandler = gameObjectInstance->GetSkillHandler())
		{
			skillHandler->TriggerEvent(FKMGameplayTagName::Event_Move_Landing_Tag);
		}
	}
}

void AKMPawnBase::WallHit(const FHitResult& hitResult)
{
	if (UKMGameObjectInstance* gameObjectInstance = GetGameObjectInstance())
	{
		if (UKMSkillHandler* skillHandler = gameObjectInstance->GetSkillHandler())
		{
			skillHandler->TriggerEvent(FKMGameplayTagName::Event_Move_WallHit_Tag);
		}
	}
}

void AKMPawnBase::CeilingHit(const FHitResult& hitResult)
{
	if (UKMGameObjectInstance* gameObjectInstance = GetGameObjectInstance())
	{
		if (UKMSkillHandler* skillHandler = gameObjectInstance->GetSkillHandler())
		{
			skillHandler->TriggerEvent(FKMGameplayTagName::Event_Move_CeilingHit_Tag);
		}
	}
}

void AKMPawnBase::OnImpact(const TSharedPtr<FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	check(skillEffectInstance.IsValid() && skillEffectInstance->GetEffectTableRecord());
	Receive_OnImpact(skillEffectInstance->GetEffectTableRecord()->Id, hitClosestPoint, hitTag);	
}

void AKMPawnBase::OnDeath()
{
	Receive_OnDeath();
}

UKMMoveShapeComponent* AKMPawnBase::GetMovementShapeComponent() const
{
	return MoveShapeComponent;
}

UKMMartialArtsComponent* AKMPawnBase::GetMartialArtsComponent() const
{
	return MartialArtsComponent;
}

UKMWidgetComponent* AKMPawnBase::GetWidgetComponent() const
{
	return nullptr;
}

UKMPawnMoverComponent* AKMPawnBase::GetMoverComponent() const
{
	return MoverComponent;
}

UKMCurveWarpingComponent* AKMPawnBase::GetCurveWarpingComponent() const
{
	return CurveWarpingComponent;
}

void AKMPawnBase::SetMovementMode(EMovementMode newMovementMode, uint8 newCustomMode)
{
	if (UKMPawnMoverComponent* moverComponent = MoverComponent)
	{
		switch (newMovementMode)
		{
		case MOVE_Walking :
			moverComponent->QueueNextMode(TEXT("Walking"));
			break;
		case MOVE_Falling :
			moverComponent->QueueNextMode(TEXT("Falling"));
			break;
		case MOVE_Flying :
			moverComponent->QueueNextMode(TEXT("Flying"));
			break;
		case MOVE_Custom :
			moverComponent->QueueNextMode(UEMMoverCurveWarpingFallingMode::ModeName);
			break;
		default : break;
		}
	}
}

UMeshComponent* AKMPawnBase::GetPlacementMeshComponent() const
{
	return nullptr;
}

UKMAttachedBlendingComponent* AKMPawnBase::GetAttachedBlendingComponent() const
{
	return AttachedComponent;
}

bool AKMPawnBase::IsAir() const
{
	return MoverComponent->IsFalling();
}

bool AKMPawnBase::IsLand() const
{
	return MoverComponent->IsOnGround();
}

float AKMPawnBase::GetScaledRadius_Implementation() const
{
	return MoveShapeComponent->GetScaledCapsuleRadius();
}

float AKMPawnBase::GetScaledHalfHeight_Implementation() const
{
	return MoveShapeComponent->GetScaledCapsuleHalfHeight();
}

FTransform AKMPawnBase::GetCarryOffsetTransform_Implementation() const
{
	return FTransform::Identity;
}

const FHitResult AKMPawnBase::GetLatestWallHitResult_Implementation() const
{
	return LastWallHitResult;
}

UKMGameObjectInstance* AKMPawnBase::GetGameObjectInstance() const
{
	if (!GameObjectInstance.IsValid())
	{
		return nullptr;
	}
	return GameObjectInstance.Get();
}

void AKMPawnBase::PossessedByGameObjectInstance(UKMGameObjectInstance* newGameObjectInstance)
{
	GameObjectInstance = newGameObjectInstance;
	check(GameObjectInstance.IsValid());
	GameObjectInstance->SetOwnerActor(this);
}

void AKMPawnBase::StartCrarry(UKMGameObjectInstance* carriedGameObjectInstance)
{
	Receive_OnStartCarry(carriedGameObjectInstance);
}

void AKMPawnBase::StartCrarried(UKMGameObjectInstance* carryGameObjectInstance)
{
	if (IsValid(carryGameObjectInstance))
	{
		carryGameObjectInstance->AddGameplayTag(FKMGameplayTagName::State_Carry_Tag);
		carryGameObjectInstance->AddGameplayTag(TakeTag);
	}

	if (UKMGameObjectInstance* carriedGameObjectInstance = GetGameObjectInstance())
	{
		carriedGameObjectInstance->AddGameplayTag(FKMGameplayTagName::State_Carried_Tag);
		carriedGameObjectInstance->AddGameplayTag(TakenTag);
	}

	Receive_OnStartCarried(carryGameObjectInstance);
}

void AKMPawnBase::ComplatePutdown(UKMGameObjectInstance* putDownedGameObjectInstance)
{
	Receive_OnComplatePutdown(putDownedGameObjectInstance);
}

void AKMPawnBase::ComplatePutdowned(UKMGameObjectInstance* putDownGameObjectInstance)
{
	if (IsValid(putDownGameObjectInstance))
	{
		putDownGameObjectInstance->RemoveGameplayTag(FKMGameplayTagName::State_Carry_Tag);
		putDownGameObjectInstance->RemoveGameplayTag(TakeTag);
	}

	if (UKMGameObjectInstance* putDownedGameObjectInstance = GetGameObjectInstance())
	{
		putDownedGameObjectInstance->RemoveGameplayTag(FKMGameplayTagName::State_Carried_Tag);
		putDownedGameObjectInstance->RemoveGameplayTag(TakenTag);
	}
	
	Receive_OnComplatePutdowned(putDownGameObjectInstance);
}

FTransform AKMPawnBase::GetApproachPullPoint(EKMApproachPullPointType approachPullPointType) const
{
	return FTransform::Identity;
}

void AKMPawnBase::StartJump()
{
}

void AKMPawnBase::AddMovementInput(FVector worldDirection, float scaleValue, bool bForce)
{
	Super::AddMovementInput(worldDirection, scaleValue, bForce);
}


FVector AKMPawnBase::ConsumeInputVector()
{
	FVector inputVector = APawn::Internal_ConsumeMovementInputVector();

	FVector finalInputDirection =  GetControlRotation().RotateVector(inputVector);
	return finalInputDirection;
}

void AKMPawnBase::ProduceInput_Implementation(int32 simTimeMs, FMoverInputCmdContext& inputCmdResult)
{
	FVector inputDirection = ConsumeInputVector();
	
	UMoverDataModelBlueprintLibrary::SetDirectionalInput(DefaultInputs, inputDirection);

	DefaultInputs.OrientationIntent = GetActorForwardVector();

	FMoverDataCollection inputDataCollection;
	inputDataCollection.AddDataByCopy(&DefaultInputs);
	
	inputCmdResult.InputCollection = inputDataCollection;
}

void AKMPawnBase::ApplyCapsuleFootAlignment(float addFootHeight)
{
	if (UKMPawnMoverComponent* moverComponent = GetMoverComponent())
	{
		FTransform baseVisualRelativeTransform = moverComponent->GetBaseVisualComponentTransform();
		baseVisualRelativeTransform.SetLocation(FVector(baseVisualRelativeTransform.GetLocation().X, baseVisualRelativeTransform.GetLocation().Y, baseVisualRelativeTransform.GetLocation().Z - addFootHeight));
		moverComponent->SetBaseVisualComponentTransform(baseVisualRelativeTransform);
	}
}
	
void AKMPawnBase::RestoreCapsuleFootAlignment()
{
	if (UKMPawnMoverComponent* moverComponent = GetMoverComponent())
	{
		moverComponent->RestoreDefaultVisualTransform();
	}
	
	if (UKMMoveShapeComponent* moveShapeComponent = GetMovementShapeComponent())
	{
		moveShapeComponent->RevertOriginSize();
	}
}
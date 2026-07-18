#include "KMCharacterHero.h"
#include "EnhancedInputComponent.h"
#include "Animation/KMAnimInstance.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Core/KMGameplayTag.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "System/KMTargetSubsystem.h"
#include "Util/KMUtil.h"

AKMCharacterHero::AKMCharacterHero() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PrePhysics; 
}

void AKMCharacterHero::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (USpringArmComponent* springArmComponent = FindComponentByClass<USpringArmComponent>())
		{
			playerController->SetControlRotation(springArmComponent->GetComponentRotation());
		}
	}
}

void AKMCharacterHero::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(playerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AKMCharacterHero::StartCustomJump);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKMCharacterHero::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AKMCharacterHero::MovePressed);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AKMCharacterHero::MoveReleased);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AKMCharacterHero::RunPressed);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AKMCharacterHero::RunReleased);
		
		EnhancedInputComponent->BindAction(CombatSkillAction, ETriggerEvent::Started, this, &AKMCharacterHero::OnCombatSkillAction);
		EnhancedInputComponent->BindAction(TechniqueSkillAction, ETriggerEvent::Started, this, &AKMCharacterHero::OnTachniqueSkillAction);

		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Started, this, &AKMCharacterHero::OnGuardSkillAction);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this, &AKMCharacterHero::OnGuardSkillAction_Release);

		EnhancedInputComponent->BindAction(Debug1Action, ETriggerEvent::Started, this, &AKMCharacterHero::OnDebug1Action);
		EnhancedInputComponent->BindAction(Debug1Action, ETriggerEvent::Completed, this, &AKMCharacterHero::OnDebug1Action_Release);

		EnhancedInputComponent->BindAction(Debug2Action, ETriggerEvent::Started, this, &AKMCharacterHero::OnDebug2Action);
		EnhancedInputComponent->BindAction(Debug2Action, ETriggerEvent::Completed, this, &AKMCharacterHero::OnDebug2Action_Release);

		EnhancedInputComponent->BindAction(Debug3Action, ETriggerEvent::Started, this, &AKMCharacterHero::OnDebug3Action);
		EnhancedInputComponent->BindAction(Debug3Action, ETriggerEvent::Completed, this, &AKMCharacterHero::OnDebug3Action_Release);
	}
}

void AKMCharacterHero::StartCustomJump()
{
	if(GetCharacterInstance()->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		return;
	}

	UKMCharacterMovementComponent* characterMovementComponent = Cast<UKMCharacterMovementComponent>(GetCharacterMovement());
	if(IsValid(characterMovementComponent))
	{
		characterMovementComponent->CustomJump();
	}
}

void AKMCharacterHero::Move(const FInputActionValue& Value)
{
	if(GetCharacterInstance()->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		return;
	}

	//if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
	//	return;
	}

	FVector2D moveDirection = UKMUtil::GetCameraToDirection2D(Value.Get<FVector2D>(), GetController());
	InputVelocity = FVector(moveDirection, 0.f);
	if (moveDirection.IsNearlyZero())
	{
		return;
	}
	
	AddMovementInput(FVector(moveDirection, 0.f), 1.f);

	UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(animInstance))
	{
		float movementCircularDirection = UKMUtil::GetCircularAngle2D8Way(moveDirection);
		GetCharacterInstance()->SetCharacterDirection(movementCircularDirection);
	}	
}

void AKMCharacterHero::MovePressed(const FInputActionValue& Value)
{
	FVector2D moveDirection = UKMUtil::GetCameraToDirection2D(Value.Get<FVector2D>(), GetController());
	if (moveDirection.IsNearlyZero())
	{
		return;
	}

	float movementCircularDirection = UKMUtil::GetCircularAngle2D8Way(moveDirection);
	float currentTime = GetWorld()->GetTimeSeconds();
    
	float deltaTime = currentTime - LastInputTime;
    
	bool bSameDirection = (movementCircularDirection == LastMovementCircularDirection);
	bool bWithinTime = (deltaTime <= DoubleTapThreshold) && (deltaTime > 0.01f);

	if (bSameDirection && bWithinTime)
	{
		OnDoubleTap(movementCircularDirection);
		LastInputTime = 0.f; 
	}
	else
	{
		LastInputTime = currentTime;
		LastMovementCircularDirection = movementCircularDirection;
	}
}

void AKMCharacterHero::MoveReleased()
{
	InputVelocity = FVector::ZeroVector;
}

void AKMCharacterHero::RunPressed()
{
	if(GetCharacterInstance()->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		return;
	}

	GetCharacterInstance()->Run();
}

void AKMCharacterHero::RunReleased()
{
	GetCharacterInstance()->Walk();
}

void AKMCharacterHero::OnDoubleTap_Implementation(float direction)
{
}

void AKMCharacterHero::PendingJump()
{
	if (CanJump())
	{
		PressedJumpReady = true;
	}
}

void AKMCharacterHero::StopJumping()
{
	Super::StopJumping();
}

void AKMCharacterHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKMCharacterHero::OnCombatSkillAction()
{
	if (bTechniqueTriggered)
	{
		GetWorldTimerManager().ClearTimer(TechniqueSkillTimerHandle);

		bCombatTriggered = false;
		bTechniqueTriggered = false;

		ExecuteUltimate();
		return;
	}

	bCombatTriggered = true;
	GetWorldTimerManager().SetTimer(CombatSkillTimerHandle,this,
		&AKMCharacterHero::ExecuteCombatSkill,ComboInputWindow,false);
}

void AKMCharacterHero::OnTachniqueSkillAction()
{
	if (bCombatTriggered)
	{
		GetWorldTimerManager().ClearTimer(CombatSkillTimerHandle);

		bCombatTriggered = false;
		bTechniqueTriggered = false;

		ExecuteUltimate();
		return;
	}

	bTechniqueTriggered = true;
	GetWorldTimerManager().SetTimer(TechniqueSkillTimerHandle,this,
		&AKMCharacterHero::ExecuteTechniqueSkill,ComboInputWindow,false);
}

void AKMCharacterHero::OnGuardSkillAction()
{
	GetCharacterInstance()->UseGuardSkill();
}

void AKMCharacterHero::OnGuardSkillAction_Release()
{
	GetCharacterInstance()->UseGuardSkill_Release();
}

void AKMCharacterHero::OnDebug1Action()
{
	GetCharacterInstance()->ToggleBeast();
}

void AKMCharacterHero::OnDebug1Action_Release()
{
}

void AKMCharacterHero::OnDebug2Action()
{
}

void AKMCharacterHero::OnDebug2Action_Release()
{
}

void AKMCharacterHero::OnDebug3Action()
{
}

void AKMCharacterHero::OnDebug3Action_Release()
{
}

void AKMCharacterHero::ExecuteCombatSkill()
{
	bCombatTriggered = false;

	UKMTargetSubsystem* targetSystem =
		UKMTargetSubsystem::GetTargetSubsystem(this);

	check(IsValid(targetSystem));

	GetCharacterInstance()->UseCombatSkill();
}

void AKMCharacterHero::ExecuteTechniqueSkill()
{
	bTechniqueTriggered = false;

	UKMTargetSubsystem* targetSystem =
		UKMTargetSubsystem::GetTargetSubsystem(this);

	check(IsValid(targetSystem));

	GetCharacterInstance()->UseTechniqueSkill();
}

void AKMCharacterHero::ExecuteUltimate()
{
	UKMTargetSubsystem* targetSystem =
		UKMTargetSubsystem::GetTargetSubsystem(this);

	check(IsValid(targetSystem));

	GetCharacterInstance()->UseUltimateSkill();
}

void AKMCharacterHero::OnEnterGame_Implementation()
{
}

void AKMCharacterHero::OnHeroSelected_Implementation()
{
	
}
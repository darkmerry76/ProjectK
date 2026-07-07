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
		
		EnhancedInputComponent->BindAction(SkillAction0, ETriggerEvent::Started, this, &AKMCharacterHero::OnSkillAction0);
		EnhancedInputComponent->BindAction(SkillAction1, ETriggerEvent::Started, this, &AKMCharacterHero::OnSkillAction1);

		EnhancedInputComponent->BindAction(SkillDebug0, ETriggerEvent::Started, this, &AKMCharacterHero::OnSkillDebug0);
		EnhancedInputComponent->BindAction(SkillDebug0, ETriggerEvent::Completed, this, &AKMCharacterHero::OnSkillDebug0_Release);
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

	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		return;
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
	//PressedJumpReady = false;
}

void AKMCharacterHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

/*	if (PressedJumpReady == true && ReadyJump == true)
	{
		Jump();
		PressedJumpReady = false;
		ReadyJump = false;
	}*/
}

void AKMCharacterHero::OnSkillAction0()
{
	if (bSkill1Triggered == true)
	{
		GetWorldTimerManager().ClearTimer(Skill1TimerHandle);

		bSkill0Triggered = false;
		bSkill1Triggered = false;

		ExecuteSkillCombo();
		return;
	}

	bSkill0Triggered = true;

	GetWorldTimerManager().SetTimer(
		Skill0TimerHandle,
		this,
		&AKMCharacterHero::ExecuteSkill0,
		ComboInputWindow,
		false);
}

void AKMCharacterHero::OnSkillAction1()
{
	if (bSkill0Triggered == true)
	{
		GetWorldTimerManager().ClearTimer(Skill0TimerHandle);

		bSkill0Triggered = false;
		bSkill1Triggered = false;

		ExecuteSkillCombo();
		return;
	}

	bSkill1Triggered = true;

	GetWorldTimerManager().SetTimer(
		Skill1TimerHandle,
		this,
		&AKMCharacterHero::ExecuteSkill1,
		ComboInputWindow,
		false);
}

void AKMCharacterHero::ExecuteSkill0()
{
	bSkill0Triggered = false;

	UKMTargetSubsystem* targetSystem =
		UKMTargetSubsystem::GetTargetSubsystem(this);

	check(IsValid(targetSystem));

	GetCharacterInstance()->UseSkillNormal();
}

void AKMCharacterHero::ExecuteSkill1()
{
	bSkill1Triggered = false;

	UKMTargetSubsystem* targetSystem =
		UKMTargetSubsystem::GetTargetSubsystem(this);

	check(IsValid(targetSystem));

	GetCharacterInstance()->UseSkillGrab();
}

void AKMCharacterHero::ExecuteSkillCombo()
{
	UKMTargetSubsystem* targetSystem =
		UKMTargetSubsystem::GetTargetSubsystem(this);

	check(IsValid(targetSystem));

	GetCharacterInstance()->UseSkillSpecial();
}

void AKMCharacterHero::OnSkillDebug0()
{
	GetCharacterInstance()->UseSkillGuard();
}

void AKMCharacterHero::OnSkillDebug0_Release()
{
	GetCharacterInstance()->UseSkillGuard_Release();
}

void AKMCharacterHero::OnEnterGame_Implementation()
{
}

void AKMCharacterHero::OnHeroSelected_Implementation()
{
	
}
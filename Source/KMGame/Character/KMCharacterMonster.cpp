#include "KMCharacterMonster.h"

#include "AI/KMAIController.h"
#include "Animation/KMAnimInstance.h"
#include "Util/KMUtil.h"

void AKMCharacterMonster::BeginPlay()
{
	Super::BeginPlay();
}

void AKMCharacterMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FMath::Abs(GetCharacterInstance()->GetMoveAccelate()) > 0.001f)
	{
		if (!GetCharacterInstance()->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
		{
			//AddMovementInput(GetActorForwardVector(), GetCharacterInstance()->GetMoveAccelate());
		}
	}
	if (GetCharacterInstance()->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		AKMAIController* aIController = Cast<AKMAIController>(GetController());
		if (IsValid(aIController))
		{
			aIController->StopMovement();
		}
	}
	if (!GetCharacterInstance()->HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(GetMesh()->GetAnimInstance());
		if (IsValid(animInstance))
		{
			FVector2D latestVelocity = FVector2D(GetVelocity());
			if (!latestVelocity.IsZero())
			{
				latestVelocity.Normalize();
				float circularAngle = UKMUtil::GetCircularAngle2D(latestVelocity);
				GetCharacterInstance()->SetCharacterDirection(circularAngle);
			}
			
			//animInstance->NextDirection = 0.5f + (GetActorForwardVector().X * 0.5f);			
			//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("NextDirect=%.3f"), animInstance->NextDirection));
			//SetActorRotation(FRotator(0.f,  ((ForceMoveDirection * -1.f) * 0.5f + 0.5f) * 180.f, 0.f));
		}
	}
}

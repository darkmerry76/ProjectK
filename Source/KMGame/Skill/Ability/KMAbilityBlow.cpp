#include "KMAbilityBlow.h"
#include "EMCurveWarpingComponent.h"
#include "Animation/AnimSet/KMAnimationSetEffect.h"
#include "Character/KMCharacter.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Skill/KMSkillHandler.h"
#include "Util/KMUtil.h"

UKMAbilityBlow::UKMAbilityBlow(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	animEffectType = EKMAnimSetEffectType::Stand_Blow_Front_0;
}

void UKMAbilityBlow::Activate()
{
	Super::Activate();

	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	check(IsValid(curveWarping));
	
	float minValue = 0.f, maxValue = 1.f;
	BlowCurve->GetValueRange(minValue, maxValue);

	FVector actorForwardVector = ownerCharacter->GetActorForwardVector();
	actorForwardVector.Z = 0.f;
	actorForwardVector.Normalize();

	FVector targetLocation = ownerCharacter->GetActorLocation() + (HorizontalPower * ownerCharacter->GetActorForwardVector() * -1.f);

	float zScale = VerticalPower / maxValue;

	if (UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement()))
	{
		characterMovement->SetCustomMovementMode(EKMCustomMovementMode::CMODE_Jump);
	}

	if (!curveWarping->GetInteruptDelegate().IsAlreadyBound(this, &UKMAbilityBlow::OnCurveWarpingInterrupt))
	{
		curveWarping->GetInteruptDelegate().AddDynamic(this, &UKMAbilityBlow::OnCurveWarpingInterrupt);
	}
	curveWarping->PlayCurveWarpjng(BlowCurve, targetLocation, Duration, zScale, false, false);
}

void UKMAbilityBlow::OnCurveWarpingInterrupt_Implementation(const FVector& moveDelta, EEMCurveWarpingInteruptType type)
{
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	UKMSkillHandler* skillHandler = ownerCharacter->GetCharacterInstance()->GetSkillHandler();
	switch(type)
	{
		case EEMCurveWarpingInteruptType::Landing:
			skillHandler->TriggerTransitionSkillEffect(FGameplayTag::RequestGameplayTag(TEXT("event.blow.landing"))); break; 
		default:break;
	}
}

void UKMAbilityBlow::Impact(const FTransform& newImpactTransform)
{
	Super::Impact(newImpactTransform);
	
	OnImpact(newImpactTransform);
}

void UKMAbilityBlow::Deactivate()
{
	Super::Deactivate();
	
	AKMCharacter* ownerCharacter = GetOwnerCharacter();
	check(IsValid(ownerCharacter));

	UEMCurveWarpingComponent* curveWarping = ownerCharacter->GetCurveWarping();
	check(IsValid(curveWarping));

	curveWarping->GetInteruptDelegate().RemoveAll(this);
}
#include "KMAbilitySkillDirectionTag.h"
#include "Animation/AnimSet/KMAnimationSetTag.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Util/KMUtil.h"

UKMAbilitySkillDirectionTag::UKMAbilitySkillDirectionTag(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilitySkillDirectionTag::Activate()
{
	UKMAbility::Activate();
}

void UKMAbilitySkillDirectionTag::Deactivate(bool bCancel)
{
	UKMAbility::Deactivate(bCancel);
}

FGameplayTag UKMAbilitySkillDirectionTag::Get8WayDirectionAnimTag(EKM8WayDirection direction) const
{
	switch (direction)
	{
	case EKM8WayDirection::Angle_0: return Angle0_AnimTag;
	case EKM8WayDirection::Angle_L45: return AngleL45_AnimTag;
	case EKM8WayDirection::Angle_L90: return AngleL90_AnimTag;
	case EKM8WayDirection::Angle_L135: return AngleL135_AnimTag;
	case EKM8WayDirection::Angle_180: return Angle180_AnimTag;
	case EKM8WayDirection::Angle_R45: return AngleR45_AnimTag;
	case EKM8WayDirection::Angle_R90: return AngleR90_AnimTag;
	case EKM8WayDirection::Angle_R135: return AngleR135_AnimTag;
	default : break;
	}
	return DefaultAnimTag;
}

float UKMAbilitySkillDirectionTag::Get8WayDirectionWeight(EKM8WayDirection direction) const
{
	switch (direction)
	{
	case EKM8WayDirection::Angle_0: return Angle0_DirectionWeight;
	case EKM8WayDirection::Angle_L45: return AngleL45_DirectionWeight;
	case EKM8WayDirection::Angle_L90: return AngleL90_DirectionWeight;
	case EKM8WayDirection::Angle_L135: return AngleL135_DirectionWeight;
	case EKM8WayDirection::Angle_180: return Angle180_DirectionWeight;
	case EKM8WayDirection::Angle_R45: return AngleR45_DirectionWeight;
	case EKM8WayDirection::Angle_R90: return AngleR90_DirectionWeight;
	case EKM8WayDirection::Angle_R135: return AngleR135_DirectionWeight;
	default : break;
	}
	return Angle0_DirectionWeight;
}

void UKMAbilitySkillDirectionTag::ApplyAngle(EKM8WayDirection direction, float moveInstance, float moveDuration, bool bIngnoreZ)
{
	FGameplayTag finalAnimTag = Get8WayDirectionAnimTag(direction);

	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UKMCharacterInstance* ownerCharacterInstance = character->GetCharacterInstance();
	check(IsValid(ownerCharacterInstance));

	if (UAnimMontage* existMontage = character->GetAnimationTag(finalAnimTag))
	{
		Montage = existMontage;
	}
	
	MontageInstance = PlayMontage(Montage, Rate, NAME_None, EKMAnimSlotType::DefaultSlot, 0.1f);

	float yawAngle = UKMUtil::Get8WayDirectionYaw(direction);
	float directionWeight = Get8WayDirectionWeight(direction);
	FVector newDirection = UKMUtil::GetYawRotation(character->GetActorForwardVector(), yawAngle).Vector();
	
	ownerCharacterInstance->SetDirection(UKMUtil::GetCircularAngle2D(FVector2D(newDirection.X, newDirection.Y) * directionWeight));
	FVector newTargetLocation = GetOffsetAlongOwnerDirection(newDirection, moveInstance, 1.f, bIngnoreZ);

	AddOwnerMotionWarpingLocation(MotionWarpingTarget, newTargetLocation);

	OnApplyAngle(direction, moveInstance, moveDuration);
}
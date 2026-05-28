#include "KMAbilitySkillDirection.h"
#include "Animation/AnimSet/KMAnimationSetSkill.h"
#include "Character/KMCharacter.h"
#include "Util/KMUtil.h"

UKMAbilitySkillDirection::UKMAbilitySkillDirection(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMAbilitySkillDirection::Activate()
{
	Super::Activate();
}

void UKMAbilitySkillDirection::Deactivate()
{
	Super::Deactivate();

	AKMCharacter* character = GetOwnerCharacter();
	if (IsValid(character))
	{
		if (IsValid(Montage))
		{
			StopMontage(Montage);
		}
	}
}

EKMAnimSetSkillType UKMAbilitySkillDirection::Get8WayDirectionAnimSet(EKM8WayDirection direction) const
{
	switch (direction)
	{
	case EKM8WayDirection::Angle_0: return Angle0_CancelAnimType;
	case EKM8WayDirection::Angle_L45: return AngleL45_CancelAnimType;
	case EKM8WayDirection::Angle_L90: return AngleL90_CancelAnimType;
	case EKM8WayDirection::Angle_L135: return AngleL135_CancelAnimType;
	case EKM8WayDirection::Angle_180: return Angle180_CancelAnimType;
	case EKM8WayDirection::Angle_R45: return AngleR45_CancelAnimType;
	case EKM8WayDirection::Angle_R90: return AngleR90_CancelAnimType;
	case EKM8WayDirection::Angle_R135: return AngleR135_CancelAnimType;
	default : break;
	}
	return DefaultCancelAnimType;
}

float UKMAbilitySkillDirection::Get8WayDirectionWeight(EKM8WayDirection direction) const
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


void UKMAbilitySkillDirection::ApplyAngle(EKM8WayDirection direction, float moveInstance, float moveDuration, bool bIngnoreZ)
{
	EKMAnimSetSkillType finalCancelAnimset = Get8WayDirectionAnimSet(direction);

	AKMCharacter* character = GetOwnerCharacter();
	check(IsValid(character));

	UKMCharacterInstance* ownerCharacterInstance = character->GetCharacterInstance();
	check(IsValid(ownerCharacterInstance));

	UKMAnimationSetSkill* animSetSkill = character->SkillAnimset;
	check(IsValid(animSetSkill));

	TObjectPtr<UAnimMontage>* existMontage = animSetSkill->AnimMontageMap.Find(finalCancelAnimset);
	if (existMontage && IsValid(*existMontage))
	{
		Montage = *existMontage;
	}

	MontageInstance = PlayerMontage(Montage, Rate);

	float yawAngle = UKMUtil::Get8WayDirectionYaw(direction);
	float directionWeight = Get8WayDirectionWeight(direction);
	FVector newDirection = UKMUtil::GetYawRotation(character->GetActorForwardVector(), yawAngle).Vector();
	
	ownerCharacterInstance->SetCharacterDirection(UKMUtil::GetCircularAngle2D(FVector2D(newDirection.X, newDirection.Y) * directionWeight));
	FVector newTargetLocation = GetOffsetAlongOwnerDirection(newDirection, moveInstance, 1.f, bIngnoreZ);

	newTargetLocation.Z = character->GetActorLocation().Z;

	PlayOwnerCurveWarping(Curve, newTargetLocation, moveDuration, 0.f, bIngnoreZ);

	OnApplyAngle(direction, moveInstance, moveDuration);
}
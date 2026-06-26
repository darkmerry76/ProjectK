#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class KMGAME_API FKMGameplayTagName 
{
public:
	static const FGameplayTag Block_Tag;
	static const FGameplayTag Block_Control_Tag;
	static const FGameplayTag Block_Control_Move_Tag;

	static const FGameplayTag Event_Hit_Tag;
	static const FGameplayTag Event_Hit_Normal_Tag;
	static const FGameplayTag Event_Hit_Projectile_Tag;
	static const FGameplayTag Event_Throw_Tag;
	static const FGameplayTag Event_Item_Launch;
	static const FGameplayTag Event_Grab_Tag;
	static const FGameplayTag Event_Bound_Wall_Tag;
	static const FGameplayTag Event_Trigger_Tag;
	static const FGameplayTag Event_Skill_Start_Tag;
	static const FGameplayTag Event_Skill_End_Tag;
	static const FGameplayTag Event_Cancel_Tag;
	static const FGameplayTag Event_Cancel_Combo_Available_Tag;
	static const FGameplayTag Event_Cancel_Available_Tag;
	static const FGameplayTag Event_Cancel_Available_Perfect_Tag;
	
	static const FGameplayTag State_Tag;
	static const FGameplayTag State_Dead_Tag;
	static const FGameplayTag State_Blow_Tag;
	static const FGameplayTag State_Blow_Bound_Tag;
	static const FGameplayTag State_Blow_Down_Tag;
	static const FGameplayTag State_Blow_Getup_Tag;
	static const FGameplayTag State_Move_Tag;
	static const FGameplayTag State_Move_Walk_Tag;
	static const FGameplayTag State_Move_Run_Tag;
	static const FGameplayTag State_Cancel_Tag;
	static const FGameplayTag State_Cancel_Good_Tag;
	static const FGameplayTag State_Cancel_Great_Tag;
	static const FGameplayTag State_Cancel_Perfect_Tag;
	static const FGameplayTag State_Parry_Tag;
	static const FGameplayTag State_Parry_Good_Tag;
	static const FGameplayTag State_Parry_Great_Tag;
	static const FGameplayTag State_Parry_Perfect_Tag;
	static const FGameplayTag State_Invincible_Tag;
	static const FGameplayTag State_Intangible_Tag;
	static const FGameplayTag State_Unselectable_Tag;
	static const FGameplayTag State_Thrown_OverlapDamage_Tag;
	
	static const FGameplayTag State_Immune_Tag;
	static const FGameplayTag State_Immune_Abnormal_Hit_Tag;
	static const FGameplayTag State_Immune_Abnormal_Nuckback_Tag;
	static const FGameplayTag State_Immune_Abnormal_Nuckdown_Tag;
	static const FGameplayTag State_Immune_Abnormal_Grabed_Tag;
	static const FGameplayTag State_Immune_Damage_Tag;
	
};

class KMGAME_API FKMGameplayTagContainer
{
public:
	void AddTag(const FGameplayTag& tagToAdd);
	bool HasTag(const FGameplayTag& tag) const;
	void RemoveTag(const FGameplayTag& tag);

	FGameplayTagContainer& GetOwnerTagContainer() { return OwnerTagContainer; };

	int32 GetRefCount(const FGameplayTag& tag) const;
	
protected:
	FGameplayTagContainer OwnerTagContainer;
	TMap<FGameplayTag, int32> TagRefCounts;
};

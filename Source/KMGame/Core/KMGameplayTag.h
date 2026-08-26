#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class KMGAME_API FKMGameplayTagName 
{
public:
	static const FGameplayTag Game_Event_Stage_Begin;
	
	static const FGameplayTag Block_Tag;
	static const FGameplayTag Block_Control_Tag;
	static const FGameplayTag Block_Control_Move_Tag;
	static const FGameplayTag Block_Control_Walk_Tag;
	static const FGameplayTag Block_Control_Run_Tag;
	static const FGameplayTag Block_Control_Jump_Tag;
	static const FGameplayTag Block_Control_Dash_Tag;
	static const FGameplayTag Block_Control_Rotation_Tag;

	static const FGameplayTag Event_Hit_Tag;
	static const FGameplayTag Event_Hit_Normal_Tag;
	static const FGameplayTag Event_Hit_Projectile_Tag;
	static const FGameplayTag Event_Throw_Tag;
	static const FGameplayTag Event_Item_Launch;
	static const FGameplayTag Event_Grab_Tag;
	static const FGameplayTag Event_Grab_Begin_End_Tag;
	static const FGameplayTag Event_Bound_Wall_Tag;
	static const FGameplayTag Event_Trigger_Tag;
	static const FGameplayTag Event_Skill_Start_Tag;
	static const FGameplayTag Event_Skill_End_Tag;
	static const FGameplayTag Event_Cancel_Tag;
	static const FGameplayTag Event_Cancel_Combo_Available_Tag;
	static const FGameplayTag Event_Cancel_Available_Tag;
	static const FGameplayTag Event_Cancel_Available_Perfect_Tag;
	static const FGameplayTag Event_Move_Landing_Tag;
	static const FGameplayTag Event_Blow_IgnoreLanded_Tag;
	
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

	static const FGameplayTag Anim_Guard_Stand_Front_0;
	static const FGameplayTag Anim_Guard_Stand_Front_1;
	static const FGameplayTag Anim_Guard_Stand_Front_2;
	static const FGameplayTag Anim_Guard_Stand_Back_0;
    static const FGameplayTag Anim_Guard_Stand_Back_1;
    static const FGameplayTag Anim_Guard_Stand_Back_2;
	static const FGameplayTag Anim_Jump_0;
	static const FGameplayTag Anim_Jump_1;
	static const FGameplayTag Anim_Jump_2;
	static const FGameplayTag Anim_Jump_Front_0;
	static const FGameplayTag Anim_Jump_Front_1;
	static const FGameplayTag Anim_Jump_Front_2;
	static const FGameplayTag Anim_Jump_Back_0;
	static const FGameplayTag Anim_Jump_Back_1;
	static const FGameplayTag Anim_Jump_Back_2;
	static const FGameplayTag Anim_Landing_0;
	static const FGameplayTag Anim_Landing_1;
	static const FGameplayTag Anim_Landing_2;
	static const FGameplayTag Anim_Landing_Front_0;
	static const FGameplayTag Anim_Landing_Front_1;
	static const FGameplayTag Anim_Landing_Front_2;
	static const FGameplayTag Anim_Landing_Back_0;
	static const FGameplayTag Anim_Landing_Back_1;
	static const FGameplayTag Anim_Landing_Back_2;
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

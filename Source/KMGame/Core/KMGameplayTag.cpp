#include "KMGameplayTag.h"

const FGameplayTag FKMGameplayTagName::Game_Event_Stage_Begin = FGameplayTag::RequestGameplayTag(TEXT("Game.Event.Stage.Begin"));

const FGameplayTag FKMGameplayTagName::Block_Tag = FGameplayTag::RequestGameplayTag(TEXT("Block"));
const FGameplayTag FKMGameplayTagName::Block_Control_Tag = FGameplayTag::RequestGameplayTag(TEXT("Block.Control"));
const FGameplayTag FKMGameplayTagName::Block_Control_Move_Tag = FGameplayTag::RequestGameplayTag(TEXT("Block.Control.Move"));

const FGameplayTag FKMGameplayTagName::Event_Hit_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Hit"));
const FGameplayTag FKMGameplayTagName::Event_Hit_Normal_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Hit.Normal"));
const FGameplayTag FKMGameplayTagName::Event_Hit_Projectile_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Hit.Projectile"));
const FGameplayTag FKMGameplayTagName::Event_Throw_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Throw"));
const FGameplayTag FKMGameplayTagName::Event_Item_Launch = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Launch"));
const FGameplayTag FKMGameplayTagName::Event_Grab_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Grab"));
const FGameplayTag FKMGameplayTagName::Event_Bound_Wall_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Bounding.Wall"));
const FGameplayTag FKMGameplayTagName::Event_Trigger_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Trigger"));
const FGameplayTag FKMGameplayTagName::Event_Skill_Start_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Skill.Start"));
const FGameplayTag FKMGameplayTagName::Event_Skill_End_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Skill.End"));
const FGameplayTag FKMGameplayTagName::Event_Cancel_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Cancel"));
const FGameplayTag FKMGameplayTagName::Event_Cancel_Combo_Available_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Cancel.Combo.Available"));
const FGameplayTag FKMGameplayTagName::Event_Cancel_Available_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Cancel.Available"));
const FGameplayTag FKMGameplayTagName::Event_Cancel_Available_Perfect_Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Cancel.Available.Perfect"));

const FGameplayTag FKMGameplayTagName::State_Tag = FGameplayTag::RequestGameplayTag(TEXT("State"));
const FGameplayTag FKMGameplayTagName::State_Dead_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"));
const FGameplayTag FKMGameplayTagName::State_Blow_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Blow"));
const FGameplayTag FKMGameplayTagName::State_Blow_Bound_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Blow.Bound"));
const FGameplayTag FKMGameplayTagName::State_Blow_Down_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Blow.Down"));
const FGameplayTag FKMGameplayTagName::State_Blow_Getup_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Blow.Getup"));
const FGameplayTag FKMGameplayTagName::State_Move_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Move"));
const FGameplayTag FKMGameplayTagName::State_Move_Walk_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Move.Walk"));
const FGameplayTag FKMGameplayTagName::State_Move_Run_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Move.Run"));
const FGameplayTag FKMGameplayTagName::State_Cancel_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Cancel"));
const FGameplayTag FKMGameplayTagName::State_Cancel_Good_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Cancel.Good"));
const FGameplayTag FKMGameplayTagName::State_Cancel_Great_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Cancel.Great"));
const FGameplayTag FKMGameplayTagName::State_Cancel_Perfect_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Cancel.Perfect"));
const FGameplayTag FKMGameplayTagName::State_Parry_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Parry"));
const FGameplayTag FKMGameplayTagName::State_Parry_Good_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Parry.Good"));
const FGameplayTag FKMGameplayTagName::State_Parry_Great_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Parry.Great"));
const FGameplayTag FKMGameplayTagName::State_Parry_Perfect_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Parry.Perfect"));
const FGameplayTag FKMGameplayTagName::State_Invincible_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Invincible"));
const FGameplayTag FKMGameplayTagName::State_Intangible_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Intangible"));
const FGameplayTag FKMGameplayTagName::State_Unselectable_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Unselectable"));
const FGameplayTag FKMGameplayTagName::State_Thrown_OverlapDamage_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Thrown.OverlapDamage"));

const FGameplayTag FKMGameplayTagName::State_Immune_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Immune"));
const FGameplayTag FKMGameplayTagName::State_Immune_Abnormal_Hit_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Immune.Abnormal"));
const FGameplayTag FKMGameplayTagName::State_Immune_Abnormal_Nuckback_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Immune.Abnormal.Nuckback"));
const FGameplayTag FKMGameplayTagName::State_Immune_Abnormal_Nuckdown_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Immune.Abnormal.Nuckdown"));
const FGameplayTag FKMGameplayTagName::State_Immune_Abnormal_Grabed_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Immune.Abnormal.Grabed"));
const FGameplayTag FKMGameplayTagName::State_Immune_Damage_Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Immune.Damage"));

const FGameplayTag FKMGameplayTagName::Anim_Guard_Stand_Front_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Guard.Stand.Front.0"));
const FGameplayTag FKMGameplayTagName::Anim_Guard_Stand_Front_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Guard.Stand.Front.1"));
const FGameplayTag FKMGameplayTagName::Anim_Guard_Stand_Front_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Guard.Stand.Front.2"));
const FGameplayTag FKMGameplayTagName::Anim_Guard_Stand_Back_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Guard.Stand.Back.0"));
const FGameplayTag FKMGameplayTagName::Anim_Guard_Stand_Back_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Guard.Stand.Back.1"));
const FGameplayTag FKMGameplayTagName::Anim_Guard_Stand_Back_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Guard.Stand.Back.2"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.0"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.1"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.2"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_Front_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.Front.0"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_Front_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.Front.1"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_Front_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.Front.2"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_Back_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.Back.0"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_Back_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.Back.1"));
const FGameplayTag FKMGameplayTagName::Anim_Jump_Back_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Jump.Back.2"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.0"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.1"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.2"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_Front_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.Front.0"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_Front_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.Front.1"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_Front_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.Front.2"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_Back_0 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.Back.0"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_Back_1 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.Back.1"));
const FGameplayTag FKMGameplayTagName::Anim_Landing_Back_2 = FGameplayTag::RequestGameplayTag(TEXT("Anim.Landing.Back.2"));

void FKMGameplayTagContainer::AddTag(const FGameplayTag& tagToAdd)
{
	FGameplayTag currentTag = tagToAdd;
    
	while (currentTag.IsValid())
	{
		OwnerTagContainer.AddTag(currentTag);

		if (!TagRefCounts.Contains(currentTag))
		{
			TagRefCounts.Add(currentTag, 1);
		}
		else
		{
			TagRefCounts[currentTag]++;
		}
        
		currentTag = currentTag.RequestDirectParent();
	}
}

void FKMGameplayTagContainer::RemoveTag(const FGameplayTag& tagToRemove)
{
	FGameplayTag currentTag = tagToRemove;

	while (currentTag.IsValid())
	{
		int32* countPtr = TagRefCounts.Find(currentTag);

		ensureMsgf(countPtr != nullptr, TEXT("RemoveTag called without AddTag: %s"), *currentTag.ToString());

		if (countPtr != nullptr)
		{
			(*countPtr)--;

			if (*countPtr <= 0)
			{
				TagRefCounts.Remove(currentTag);
				OwnerTagContainer.RemoveTag(currentTag);
			}
		}

		currentTag = currentTag.RequestDirectParent();
	}
}

bool FKMGameplayTagContainer::HasTag(const FGameplayTag& tag) const
{
	return OwnerTagContainer.HasTag(tag);
}

int32 FKMGameplayTagContainer::GetRefCount(const FGameplayTag& tag) const
{
	const int32* countPtr = TagRefCounts.Find(tag);
	if (!countPtr)
	{
		return 0;
	}
	return (*countPtr);
}
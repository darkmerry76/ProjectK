#include "KMMartialArtsComponent.h"
#include "EMMartialArts.h"

FKMMartialArtsSkillContextData::FKMMartialArtsSkillContextData(UEMAbility* ability)
{
	Ability = ability;
}

UEMAbility* FKMMartialArtsSkillContextData::GetAbility() const
{
	if (!Ability.IsValid())
	{
		return nullptr;
	}
	return Ability.Pin().Get();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMMartialArtsComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMMartialArtsComponent::UKMMartialArtsComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}
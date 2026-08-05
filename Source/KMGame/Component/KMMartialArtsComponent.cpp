#include "KMMartialArtsComponent.h"
#include "EMMartialArts.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMMartialArtsSkillContextData
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FKMMartialArtsSkillContextData::FKMMartialArtsSkillContextData(UEMAbility* ability) : Ability(ability)
{
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
// FKMMartialArtsInstance
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FKMMartialArtsInstance::FKMMartialArtsInstance(UEMMartialArtsComponent* ownerComponent, int32 id, UEMMartialArts* newMartialArts) :
	FEMMartialArtsInstance(ownerComponent, id, newMartialArts)
{
}

void FKMMartialArtsInstance::OnAddContextData(FAnimNotifyEventReference& eventReference, const TSharedPtr<const UE::Anim::IAnimNotifyEventContextDataInterface>& newContextData)
{
	if (newContextData->Is<FKMMartialArtsSkillContextData>())
	{
		TSharedPtr<const FKMMartialArtsSkillContextData> castSkillContextData = StaticCastSharedPtr<const FKMMartialArtsSkillContextData>(newContextData);
		
		eventReference.AddContextData<FKMMartialArtsSkillContextData>(castSkillContextData->GetAbility());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMMartialArtsComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UKMMartialArtsComponent::UKMMartialArtsComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

TSharedPtr<FEMMartialArtsInstance> UKMMartialArtsComponent::CreateInstance(class UEMMartialArts* martialArts, int32 newId)
{
	return MakeShared<FKMMartialArtsInstance>(this, newId, martialArts);
}
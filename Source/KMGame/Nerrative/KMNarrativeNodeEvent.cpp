#include "KMNarrativeNodeEvent.h"
#include "EMDataTable.h"
#include "GameplayTagContainer.h"
#include "System/KMNarrativeSubsystem.h"
#include "Tables/Generated/KMTable_Narrative_Event.h"

UKMNarrativeNodeEvent::UKMNarrativeNodeEvent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMNarrativeNodeEvent::Activate()
{
	Super::Activate();
	
	check(TableGroup.Num() == 1);

	EventTableRow = CastRow<FKMTable_Narrative_EventRow>(TableGroup[0]);
	check(EventTableRow);

	UKMNarrativeSubsystem* narrativeSubsystem = UKMNarrativeSubsystem::GetNarrativeSubsystem(this);
	check(IsValid(narrativeSubsystem));

	narrativeSubsystem->BroadcastEvent(FGameplayTag::RequestGameplayTag(EventTableRow->EventTag));
}

void UKMNarrativeNodeEvent::Deactivate()
{
	Super::Deactivate();
}

void UKMNarrativeNodeEvent::CopyFrom(class UKMNarrativeNode* source)
{
	Super::CopyFrom(source);
}

bool UKMNarrativeNodeEvent::IsEnd() const
{
	return true;
}

void UKMNarrativeNodeEvent::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsEnd())
	{
		BranchDelegate.Execute(this, EventTableRow->Branch);
	}
}
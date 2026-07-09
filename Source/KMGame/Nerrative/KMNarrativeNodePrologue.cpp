#include "KMNarrativeNodePrologue.h"
#include "System/KMUiSubsystem.h"
#include "Tables/Generated/KMTable_Narrative_Prologue.h"
#include "Ui/Window/Prologue/KMPrologueWindow.h"

UKMNarrativeNodePrologue::UKMNarrativeNodePrologue(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMNarrativeNodePrologue::Activate()
{
	Super::Activate();
	
	check(TableGroup.Num() == 1);
	
	PrologueTableRow = CastRow<FKMTable_Narrative_PrologueRow>(TableGroup[0]);
	check(PrologueTableRow);

	BeginTime = GetWorld()->GetTimeSeconds();

	UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this);
	check(IsValid(uiSubsystem));

	PrologueWidget = uiSubsystem->CreatePrologue(PrologueTableRow->Id);
}

void UKMNarrativeNodePrologue::Deactivate()
{
	Super::Deactivate();

	PrologueWidget->RemoveFromParent();
}

void UKMNarrativeNodePrologue::CopyFrom(class UKMNarrativeNode* source)
{
	UKMNarrativeNodePrologue* sourceNoedSequence = Cast<UKMNarrativeNodePrologue>(source);
	Super::CopyFrom(source);
}

bool UKMNarrativeNodePrologue::IsEnd() const
{
	if (GetWorld()->GetTimeSeconds() - BeginTime >= PrologueTableRow->Duration + PrologueTableRow->StartDelay + PrologueTableRow->FadeOutTime + PrologueTableRow->EndDelay)
	{
		return true;
	}
	
	return false;
}

void UKMNarrativeNodePrologue::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsEnd())
	{
		BranchDelegate.Execute(this, PrologueTableRow->Branch);
	}
}
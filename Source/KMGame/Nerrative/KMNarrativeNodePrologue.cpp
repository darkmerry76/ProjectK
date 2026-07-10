#include "KMNarrativeNodePrologue.h"
#include "System/KMUiSubsystem.h"
#include "Tables/Generated/KMTable_Narrative_Prologue.h"
#include "Ui/Window/Prologue/KMPrologueWindow.h"
#include "Util/KMUtil.h"

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

	if (IsValid(PrologueWidget))
	{
		PrologueWidget->RemoveFromParent();
	}
}

void UKMNarrativeNodePrologue::CopyFrom(class UKMNarrativeNode* source)
{
	UKMNarrativeNodePrologue* sourceNoedSequence = Cast<UKMNarrativeNodePrologue>(source);
	Super::CopyFrom(source);
}

bool UKMNarrativeNodePrologue::IsEnd() const
{
	if (GetWorld()->GetTimeSeconds() >= PrologueTableRow->Duration + PrologueTableRow->StartDelay + PrologueTableRow->EndDelay + PrologueTableRow->FadeOutTime)
	{
		return true;
	}
	
	return false;
}

void UKMNarrativeNodePrologue::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (!bIsFadeOut)
	{
		if (GetWorld()->GetTimeSeconds() >= PrologueTableRow->Duration + PrologueTableRow->StartDelay + PrologueTableRow->EndDelay)
		{
			UKMUtil::PlaySlateFade(this, 0.f, 1.f,PrologueTableRow->FadeOutTime);
			bIsFadeOut = true;
		}
	}
	
	if (IsEnd())
	{
		BranchDelegate.Execute(this, PrologueTableRow->Branch);
	}
}
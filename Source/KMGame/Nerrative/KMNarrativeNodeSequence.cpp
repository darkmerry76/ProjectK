#include "KMNarrativeNodeSequence.h"

#include "Hash/BuzHash.h"
#include "Tables/Generated/KMTable_Narrative_Sequence.h"

UKMNarrativeNodeSequence::UKMNarrativeNodeSequence(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMNarrativeNodeSequence::Activate()
{
	Super::Activate();

	for (auto tableItr = TableGroup.CreateConstIterator(); tableItr; ++tableItr)
	{
		const FKMTable_Narrative_SequenceRow* narrativeSequenceTable = CastRow<FKMTable_Narrative_SequenceRow>(*tableItr);
		check(narrativeSequenceTable);

		if (narrativeSequenceTable->task.IsEmpty())
		{
			continue;
		}
		for (auto taskItr = narrativeSequenceTable->task.CreateConstIterator(); taskItr; ++taskItr)
		{
			BranchDelegate.Execute(this, *taskItr);
		}
		break;
	}
}

void UKMNarrativeNodeSequence::Deactivate()
{
	Super::Deactivate();
}

void UKMNarrativeNodeSequence::CopyFrom(class UKMNarrativeNode* source)
{
	UKMNarrativeNodeSequence* sourceNoedSequence = Cast<UKMNarrativeNodeSequence>(source);
	
	Super::CopyFrom(source);
}

bool UKMNarrativeNodeSequence::IsEnd() const
{
	bool bEnd =  ChildNodes.IsEmpty();
	if (bEnd)
	{
		return true;
	}
	return false;
}
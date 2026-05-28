#include "KMNarrativeNodeDirector.h"
#include "Tables/Generated/KMTable_Narrative_Director.h"

UKMNarrativeNodeDirector::UKMNarrativeNodeDirector(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMNarrativeNodeDirector::Activate()
{
	Super::Activate();

	for (auto tableItr = TableGroup.CreateConstIterator(); tableItr; ++tableItr)
	{
		const FKMTable_Narrative_DirectorRow* narrativeDirectorTable = CastRow<FKMTable_Narrative_DirectorRow>(*tableItr);
		check(narrativeDirectorTable);

		if (narrativeDirectorTable->Sequence == NAME_None)
		{
			continue;
		}
		BranchDelegate.Execute(this, narrativeDirectorTable->Sequence);
		break;
	}
}

void UKMNarrativeNodeDirector::Deactivate()
{
	Super::Deactivate();
}

void UKMNarrativeNodeDirector::CopyFrom(class UKMNarrativeNode* source)
{
	UKMNarrativeNodeDirector* sourceNoedDirector = Cast<UKMNarrativeNodeDirector>(source);
	
	Super::CopyFrom(source);
}
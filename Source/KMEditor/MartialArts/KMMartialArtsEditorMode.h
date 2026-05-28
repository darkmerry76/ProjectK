#pragma once

#include "CoreMinimal.h"
#include "EMMartialArtsEditorMode.h"

class FKMMartialArtsEditorMode : public FEMMartialArtsEditorMode
{
public:
	FKMMartialArtsEditorMode(TSharedRef<class FWorkflowCentricApplication> hostingApp, TSharedRef<class ISkeletonTree> skeletonTree);

protected:
	virtual TSharedPtr<FEMSequencerTabSummoner> CreateSequencerTabSummoner() override;
	virtual TSharedPtr<FEMCharacterTabSummoner> CreateCharacterTabSummoner() override;
};

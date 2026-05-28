#pragma once

#include "CoreMinimal.h"
#include "TabSummoner/EMSequencerTabSummoner.h"

class FKMSequencerTabSummoner : public FEMSequencerTabSummoner
{
public:
	FKMSequencerTabSummoner(TSharedPtr<class FAssetEditorToolkit> hostingApp, const TSharedRef<class IPersonaPreviewScene>& previewScene);
};

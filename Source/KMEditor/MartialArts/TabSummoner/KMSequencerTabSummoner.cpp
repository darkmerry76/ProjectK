#include "KMSequencerTabSummoner.h"

#define LOCTEXT_NAMESPACE "KMSequencerTabSummoner"

FKMSequencerTabSummoner::FKMSequencerTabSummoner(TSharedPtr<class FAssetEditorToolkit> hostingApp, const TSharedRef<IPersonaPreviewScene>& previewScene) :
	FEMSequencerTabSummoner(hostingApp, previewScene)
{
}

#undef LOCTEXT_NAMESPACE
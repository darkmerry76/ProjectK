#pragma once

#include "CoreMinimal.h"
#include "TabSummoner/EMCharacterTabSummoner.h"

class FKMCharacterTabSummoner : public FEMCharacterTabSummoner
{
public:
	FKMCharacterTabSummoner(TSharedPtr<class FAssetEditorToolkit> hostingApp, const TSharedRef<class IPersonaPreviewScene>& previewScene);

protected:
	virtual TSharedRef<SWidget> CreateOutliner() const;
};

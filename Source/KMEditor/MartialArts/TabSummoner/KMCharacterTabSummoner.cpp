#include "KMCharacterTabSummoner.h"
#include "EMOutlinerPublicTypes.h"
#include "KMEditor/MartialArts/CharacterOutliner/KMCharacterOutlinerMode.h"
#include "KMEditor/MartialArts/CharacterOutliner/SKMCharacterOutliner.h"

#define LOCTEXT_NAMESPACE "KMCharacterTabSummoner"

FKMCharacterTabSummoner::FKMCharacterTabSummoner(TSharedPtr<FAssetEditorToolkit> hostingApp, const TSharedRef<IPersonaPreviewScene>& previewScene) :
	FEMCharacterTabSummoner(hostingApp, previewScene)
{
}

TSharedRef<SWidget> FKMCharacterTabSummoner::CreateOutliner() const
{
	FEMCreateOutlinerMode modeFactory = FEMCreateOutlinerMode::CreateLambda([](SEMOutliner* outliner)
	{
		return new FKMCharacterOutlinerMode(outliner);
	});
	
	FEMOutlinerInitializationOptions outlinerOptions;
	outlinerOptions.ModeFactory = modeFactory;
	
	TSharedPtr<SKMCharacterOutliner> newOutliner;
	SAssignNew(newOutliner, SKMCharacterOutliner, outlinerOptions);
	
	return newOutliner.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE
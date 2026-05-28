#include "KMMartialArtsEditorMode.h"
#include "EMMartialArtsEditor.h"
#include "IPersonaToolkit.h"
#include "TabSummoner/KMCharacterTabSummoner.h"
#include "TabSummoner/KMSequencerTabSummoner.h"

FKMMartialArtsEditorMode::FKMMartialArtsEditorMode(TSharedRef<FWorkflowCentricApplication> hostingApp, TSharedRef<ISkeletonTree> skeletonTree)
	: FEMMartialArtsEditorMode(hostingApp, skeletonTree)
{
}

TSharedPtr<FEMSequencerTabSummoner> FKMMartialArtsEditorMode::CreateSequencerTabSummoner()
{
	if (!HostingAppPtr.IsValid())
	{
		return nullptr;
	}
	
	TSharedPtr<FEMMartialArtsEditor> animationEditor = StaticCastSharedPtr<FEMMartialArtsEditor>(HostingAppPtr.Pin());
	return MakeShared<FKMSequencerTabSummoner>(HostingAppPtr.Pin(), animationEditor->GetPersonaToolkit()->GetPreviewScene());	
}

TSharedPtr<FEMCharacterTabSummoner> FKMMartialArtsEditorMode::CreateCharacterTabSummoner()
{
	if (!HostingAppPtr.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<FEMMartialArtsEditor> animationEditor = StaticCastSharedPtr<FEMMartialArtsEditor>(HostingAppPtr.Pin());
	return MakeShared<FKMCharacterTabSummoner>(HostingAppPtr.Pin(), animationEditor->GetPersonaToolkit()->GetPreviewScene());
}

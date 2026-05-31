#include "KMMartialArtsEditor.h"
#include "KMMartialArtsEditorMode.h"
#include "CharacterOutliner/KMCharacterOutlinerHierarchy.h"

#define LOCTEXT_NAMESPACE "KMMartialArtsEditor"

FKMMartialArtsEditor::~FKMMartialArtsEditor()
{
	if (FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.IsBoundToObject(this))
	{
		FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.RemoveAll(this);
	}
}

void FKMMartialArtsEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr<class IToolkitHost>& initToolkitHost, class UAnimationAsset* animationAsset)
{
	FEMMartialArtsEditor::InitEditor(mode, initToolkitHost, animationAsset);

	if (!FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.IsBoundToObject(this))
	{
		FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.AddSP(this, &FKMMartialArtsEditor::OnCharacterelected);
	}
}

TSharedPtr<FEMMartialArtsEditorMode> FKMMartialArtsEditor::CreateEditorMode()
{
	return MakeShared<FKMMartialArtsEditorMode>(SharedThis(this), SkeletonTree.ToSharedRef());
}

void FKMMartialArtsEditor::OnCharacterelected(const FKMTable_CharacterRow* newCharacterTable)
{
}

#undef LOCTEXT_NAMESPACE

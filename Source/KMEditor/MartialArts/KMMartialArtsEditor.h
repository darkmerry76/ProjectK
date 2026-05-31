#pragma once

#include "CoreMinimal.h"
#include "EMMartialArtsEditor.h"

class FKMMartialArtsEditor : public FEMMartialArtsEditor
{
public:
	virtual ~FKMMartialArtsEditor() override;
	
protected:
	virtual void InitEditor(const EToolkitMode::Type mode, const TSharedPtr<class IToolkitHost>& initToolkitHost, class UAnimationAsset* animationAsset) override;
	virtual TSharedPtr<class FEMMartialArtsEditorMode> CreateEditorMode() override;

	void OnCharacterelected(const struct FKMTable_CharacterRow* newCharacterTable);
};

#pragma once

#include "EMOutlinerTextTreeItem.h"
#include "Widgets/SWidget.h"
#include "Folder.h"

DECLARE_DELEGATE_ThreeParams(FKMCharacterLabelCommttedDelgate, TWeakPtr<struct FKMCharacterOutlinerTreeItem> treeItem, const FText& label, ETextCommit::Type commitInfo);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FKMCharacterVerifyItemLabelChangedDelgate, TWeakPtr<struct FKMCharacterOutlinerTreeItem> treeItem, const FText& label, FText& outErrorMessage);

struct FKMCharacterOutlinerTreeItem : public FEMOutlinerTextTreeItem
{
public:
	static const FEMOutlinerTreeItemType Type;

	FKMCharacterOutlinerTreeItem(const FText& text);

	void SetCharacterTable(const struct FKMTable_CharacterRow* newCharacterTable);
	const struct FKMTable_CharacterRow* GetCharacterTable() const;

	static FKMCharacterLabelCommttedDelgate& GetCharacterLabelCommttedDelgate();
	static FKMCharacterVerifyItemLabelChangedDelgate& GetCharacterVerifyItemLabelChangedDelgate();

protected:
	virtual TSharedRef<SWidget> GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row) override;

protected:
	const struct FKMTable_CharacterRow* CharacterTable = nullptr;;
};

struct SKMCharacterOutlinerTreeItem : public SEMOutlinerTextTreeItem
{
protected:
	virtual bool OnVerifyItemLabelChanged(const FText& label, FText& outErrorMessage) override;
	virtual void OnLabelCommitted(const FText& label, ETextCommit::Type commitInfo) override;
	virtual FSlateColor GetForegroundColor() const override;
	virtual FText GetDisplayText() const override;
	virtual const struct FSlateBrush* GetIcon() const override;

private:
	bool bIsValidLabel = true;
};

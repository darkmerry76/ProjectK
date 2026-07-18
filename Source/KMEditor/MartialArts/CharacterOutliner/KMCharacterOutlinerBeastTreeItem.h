#pragma once

#include "EMOutlinerTextTreeItem.h"
#include "Widgets/SWidget.h"
#include "Folder.h"

DECLARE_DELEGATE_ThreeParams(FKMBeastLabelCommttedDelgate, TWeakPtr<struct FKMCharacterOutlinerBeastTreeItem> treeItem, const FText& label, ETextCommit::Type commitInfo);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FKMBeastVerifyItemLabelChangedDelgate, TWeakPtr<struct FKMCharacterOutlinerBeastTreeItem> treeItem, const FText& label, FText& outErrorMessage);

struct FKMCharacterOutlinerBeastTreeItem : public FEMOutlinerTextTreeItem
{
public:
	static const FEMOutlinerTreeItemType Type;

	FKMCharacterOutlinerBeastTreeItem(const FText& text);

	void SetBeastTable(const struct FKMTable_BeastRow* newBeastTable);
	const struct FKMTable_BeastRow* GetBeastTable() const;

	static FKMBeastLabelCommttedDelgate& GetBeastLabelCommttedDelgate();
	static FKMBeastVerifyItemLabelChangedDelgate& GetBeastVerifyItemLabelChangedDelgate();

protected:
	virtual TSharedRef<SWidget> GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row) override;

protected:
	const struct FKMTable_BeastRow* BeastTable = nullptr;;
};

struct SKMCharacterOutlinerBeastTreeItem : public SEMOutlinerTextTreeItem
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

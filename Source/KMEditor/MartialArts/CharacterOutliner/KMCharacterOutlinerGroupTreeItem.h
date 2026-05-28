#pragma once

#include "EMOutlinerFolderTreeItem.h"
#include "Widgets/SWidget.h"
#include "Folder.h"

DECLARE_DELEGATE_ThreeParams(FEMCharacterGroupLabelCommttedDelgate, TWeakPtr<struct FKMCharacterOutlinerGroupTreeItem> treeItem, const FText& label, ETextCommit::Type commitInfo);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FKMCharacterGroupVerifyItemLabelChangedDelgate, TWeakPtr<struct FKMCharacterOutlinerGroupTreeItem> treeItem, const FText& label, FText& outErrorMessage);

struct FKMCharacterOutlinerGroupTreeItem : public FEMOutlinerFolderTreeItem
{
public:
	static const FEMOutlinerTreeItemType Type;

	FKMCharacterOutlinerGroupTreeItem(const FFolder& folder, FEMOutlinerTreeItemType type);
	FKMCharacterOutlinerGroupTreeItem(const FFolder& folder);
	FKMCharacterOutlinerGroupTreeItem(FName path);
	FKMCharacterOutlinerGroupTreeItem(FName InPath, FEMOutlinerTreeItemType Type);

public:
	virtual FEMOutlinerTreeItemType GetType() const override { return FKMCharacterOutlinerGroupTreeItem::Type; }

protected:
	virtual TSharedRef<SWidget> GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row) override;

public:
	static FEMCharacterGroupLabelCommttedDelgate& GetCharacterGroupLabelCommttedDelgate();
	static FKMCharacterGroupVerifyItemLabelChangedDelgate& GetCharacterGroupVerifyItemLabelChangedDelgate();
};

struct SKMCharacterOutlinerGroupTreeItem : public SEMOutlinerFolderTreeItem
{
protected:
	virtual bool OnVerifyItemLabelChanged(const FText& label, FText& outErrorMessage) override;
	virtual void OnLabelCommitted(const FText& label, ETextCommit::Type commitInfo) override;
	virtual FSlateColor GetForegroundColor() const override;

	virtual FText GetDisplayText() const override;

protected:
	bool bIsValidLabel = true;
};
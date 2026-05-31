#include "KMTagChooserOutlinerTreeItem.h"
#include "Styling/SlateIconFinder.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const FEMOutlinerTreeItemType FKMTagChooserOutlinerTreeItem::Type(&IEMOutlinerTreeItem::Type);
FKMTagChooserOutlinerTreeItem::FKMTagChooserOutlinerTreeItem(const FText& text) : FEMOutlinerTextTreeItem(text, FKMTagChooserOutlinerTreeItem::Type)
{
}

TSharedRef<SWidget> FKMTagChooserOutlinerTreeItem::GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row)
{
	return SNew(SKMTagChooserOutlinerTreeItem, *this, outliner, row);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SKMTagChooserOutlinerTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FSlateColor SKMTagChooserOutlinerTreeItem::GetForegroundColor() const
{
	return SEMOutlinerTextTreeItem::GetForegroundColor();
}

FText SKMTagChooserOutlinerTreeItem::GetDisplayText() const
{
	FText displayText = SEMOutlinerTextTreeItem::GetDisplayText();
	
	return displayText;
}

const FSlateBrush* SKMTagChooserOutlinerTreeItem::GetIcon() const
{
	return FSlateIconFinder::FindIconForClass(UAnimMontage::StaticClass()).GetOptionalIcon();
}

FText SKMTagChooserOutlinerTreeItem::GetTooltipText() const
{
	if (const TSharedPtr<FKMTagChooserOutlinerTreeItem> treeItem = StaticCastSharedPtr<FKMTagChooserOutlinerTreeItem>(TreeItemPtr.Pin()))
	{
		return FText::FromString(treeItem->GetTag().ToString());
	}

	return SEMOutlinerTextTreeItem::GetTooltipText();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerGroupTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const FEMOutlinerTreeItemType FKMTagChooserOutlinerGroupTreeItem::Type(&IEMOutlinerTreeItem::Type);
FKMTagChooserOutlinerGroupTreeItem::FKMTagChooserOutlinerGroupTreeItem(const FFolder& folder, FEMOutlinerTreeItemType type) : FEMOutlinerFolderTreeItem(folder, type)
{
}

FKMTagChooserOutlinerGroupTreeItem::FKMTagChooserOutlinerGroupTreeItem(const FFolder& folder) : FEMOutlinerFolderTreeItem(folder, FKMTagChooserOutlinerGroupTreeItem::Type)
{
}

FKMTagChooserOutlinerGroupTreeItem::FKMTagChooserOutlinerGroupTreeItem(FName path) : FEMOutlinerFolderTreeItem(path, FKMTagChooserOutlinerGroupTreeItem::Type)
{
}

FKMTagChooserOutlinerGroupTreeItem::FKMTagChooserOutlinerGroupTreeItem(FName path, FEMOutlinerTreeItemType type) : FEMOutlinerFolderTreeItem(path, type)
{
}

TSharedRef<SWidget> FKMTagChooserOutlinerGroupTreeItem::GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row)
{
	return SNew(SKMTagChooserOutlinerGroupTreeItem, *this, outliner, row);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SKMTagChooserOutlinerGroupTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FSlateColor SKMTagChooserOutlinerGroupTreeItem::GetForegroundColor() const
{
	return SEMOutlinerFolderTreeItem::GetForegroundColor();
}

FText SKMTagChooserOutlinerGroupTreeItem::GetDisplayText() const
{
	FText displayText = SEMOutlinerFolderTreeItem::GetDisplayText();
	return displayText;
}

FText SKMTagChooserOutlinerGroupTreeItem::GetTooltipText() const
{
	if (const TSharedPtr<FKMTagChooserOutlinerGroupTreeItem> treeItem = StaticCastSharedPtr<FKMTagChooserOutlinerGroupTreeItem>(TreeItemPtr.Pin()))
	{
		return FText::FromString(treeItem->GetTag().ToString());
	}

	return SEMOutlinerFolderTreeItem::GetTooltipText();
}

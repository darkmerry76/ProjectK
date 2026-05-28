#include "KMCharacterOutlinerGroupTreeItem.h"
#include "EMMartialArtsUtil.h"

const FEMOutlinerTreeItemType FKMCharacterOutlinerGroupTreeItem::Type(&IEMOutlinerTreeItem::Type);
FKMCharacterOutlinerGroupTreeItem::FKMCharacterOutlinerGroupTreeItem(const FFolder& folder, FEMOutlinerTreeItemType type) : FEMOutlinerFolderTreeItem(folder, type)
{
}

FKMCharacterOutlinerGroupTreeItem::FKMCharacterOutlinerGroupTreeItem(const FFolder& folder) : FEMOutlinerFolderTreeItem(folder, FKMCharacterOutlinerGroupTreeItem::Type)
{
}

FKMCharacterOutlinerGroupTreeItem::FKMCharacterOutlinerGroupTreeItem(FName path) : FEMOutlinerFolderTreeItem(path, FKMCharacterOutlinerGroupTreeItem::Type)
{
}

FKMCharacterOutlinerGroupTreeItem::FKMCharacterOutlinerGroupTreeItem(FName path, FEMOutlinerTreeItemType type) : FEMOutlinerFolderTreeItem(path, type)
{
}

TSharedRef<SWidget> FKMCharacterOutlinerGroupTreeItem::GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row)
{
	return SNew(SKMCharacterOutlinerGroupTreeItem, *this, outliner, row);
}

FEMCharacterGroupLabelCommttedDelgate& FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupLabelCommttedDelgate()
{
	static FEMCharacterGroupLabelCommttedDelgate groupLabelCommttedDelgate;
	return groupLabelCommttedDelgate;
}

FKMCharacterGroupVerifyItemLabelChangedDelgate& FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupVerifyItemLabelChangedDelgate()
{
	static FKMCharacterGroupVerifyItemLabelChangedDelgate groupVerifyItemLabelChangedDelgate;
	return groupVerifyItemLabelChangedDelgate;
}

bool SKMCharacterOutlinerGroupTreeItem::OnVerifyItemLabelChanged(const FText& label, FText& outErrorMessage)
{
	if (FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupVerifyItemLabelChangedDelgate().IsBound())
	{
		bIsValidLabel = FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupVerifyItemLabelChangedDelgate().Execute(StaticCastWeakPtr<FKMCharacterOutlinerGroupTreeItem>(TreeItemPtr), label, outErrorMessage);
	}
	else
	{
		bIsValidLabel = UEMMartialArtsUtil::IsValidAssetName(label.ToString());
	}
	bIsValidLabel &= !label.IsEmpty();
	if (!bIsValidLabel)
	{
		outErrorMessage = FText::FromString(TEXT("Invalid CameraGroup Name"));
	}
	return bIsValidLabel;
}

void SKMCharacterOutlinerGroupTreeItem::OnLabelCommitted(const FText& label, ETextCommit::Type commitInfo)
{
	bIsValidLabel = true;
	FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupLabelCommttedDelgate().ExecuteIfBound(StaticCastWeakPtr<FKMCharacterOutlinerGroupTreeItem>(TreeItemPtr), label, commitInfo);
}

FSlateColor SKMCharacterOutlinerGroupTreeItem::GetForegroundColor() const
{
	if (!bIsValidLabel)
	{
		return FLinearColor::Red;
	}
	return SEMOutlinerFolderTreeItem::GetForegroundColor();
}

FText SKMCharacterOutlinerGroupTreeItem::GetDisplayText() const
{
	FText displayText = SEMOutlinerFolderTreeItem::GetDisplayText();

	if (TreeItemPtr.IsValid())
	{
		TSharedPtr<FKMCharacterOutlinerGroupTreeItem> groupItem =StaticCastSharedPtr<FKMCharacterOutlinerGroupTreeItem>(TreeItemPtr.Pin());
		if (groupItem->GetAsset() && groupItem->GetAsset()->GetOutermost()->IsDirty())
		{
			displayText = FText::Format(FText::FromString(TEXT("{0}*")),displayText);
		}
	}
	return displayText;
}
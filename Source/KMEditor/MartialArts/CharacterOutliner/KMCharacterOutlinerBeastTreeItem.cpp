#include "KMCharacterOutlinerBeastTreeItem.h"
#include "EMMartialArtsEditorUtil.h"
#include "Animation/SkeletalMeshActor.h"
#include "Styling/SlateIconFinder.h"

const FEMOutlinerTreeItemType FKMCharacterOutlinerBeastTreeItem::Type(&IEMOutlinerTreeItem::Type);
FKMCharacterOutlinerBeastTreeItem::FKMCharacterOutlinerBeastTreeItem(const FText& text) : FEMOutlinerTextTreeItem(text, FKMCharacterOutlinerBeastTreeItem::Type)
{
}

TSharedRef<SWidget> FKMCharacterOutlinerBeastTreeItem::GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row)
{
	return SNew(SKMCharacterOutlinerBeastTreeItem, *this, outliner, row);
}

void FKMCharacterOutlinerBeastTreeItem::SetBeastTable(const FKMTable_BeastRow* newBeastTable)
{
	BeastTable = newBeastTable;
}

const FKMTable_BeastRow* FKMCharacterOutlinerBeastTreeItem::GetBeastTable() const
{
	return BeastTable;
}

FKMBeastLabelCommttedDelgate& FKMCharacterOutlinerBeastTreeItem::GetBeastLabelCommttedDelgate()
{
	static FKMBeastLabelCommttedDelgate beastLabelCommttedDelgate;
	return beastLabelCommttedDelgate;
}

FKMBeastVerifyItemLabelChangedDelgate& FKMCharacterOutlinerBeastTreeItem::GetBeastVerifyItemLabelChangedDelgate()
{
	static FKMBeastVerifyItemLabelChangedDelgate beastVerifyItemLabelChangedDelgate;
	return beastVerifyItemLabelChangedDelgate;
}

bool SKMCharacterOutlinerBeastTreeItem::OnVerifyItemLabelChanged(const FText& label, FText& outErrorMessage)
{
	if (FKMCharacterOutlinerBeastTreeItem::GetBeastVerifyItemLabelChangedDelgate().IsBound())
	{
		bIsValidLabel = FKMCharacterOutlinerBeastTreeItem::GetBeastVerifyItemLabelChangedDelgate().Execute(StaticCastWeakPtr<FKMCharacterOutlinerBeastTreeItem>(TreeItemPtr), label, outErrorMessage);
	}
	else
	{
		bIsValidLabel = UEMMartialArtsEditorUtil::IsValidAssetName(label.ToString());
	}

	bIsValidLabel &= !label.IsEmpty();
	if (!bIsValidLabel)
	{
		outErrorMessage = FText::FromString(TEXT("Invalid Beast Name"));
	}
	return bIsValidLabel;
}

void SKMCharacterOutlinerBeastTreeItem::OnLabelCommitted(const FText& label, ETextCommit::Type commitInfo)
{
	bIsValidLabel = true;
	FKMCharacterOutlinerBeastTreeItem::GetBeastLabelCommttedDelgate().ExecuteIfBound(StaticCastWeakPtr<FKMCharacterOutlinerBeastTreeItem>(TreeItemPtr), label, commitInfo);
}

FSlateColor SKMCharacterOutlinerBeastTreeItem::GetForegroundColor() const
{
	if (!bIsValidLabel)
	{
		return FLinearColor::Red;
	}
	return SEMOutlinerTextTreeItem::GetForegroundColor();
}

FText SKMCharacterOutlinerBeastTreeItem::GetDisplayText() const
{
	FText displayText = SEMOutlinerTextTreeItem::GetDisplayText();

	if (TreeItemPtr.IsValid())
	{
		TSharedPtr<FKMCharacterOutlinerBeastTreeItem> beastItem =StaticCastSharedPtr<FKMCharacterOutlinerBeastTreeItem>(TreeItemPtr.Pin());
		if (beastItem->GetAsset() && beastItem->GetAsset()->GetOutermost()->IsDirty())
		{
			displayText = FText::Format(FText::FromString(TEXT("{0}*")),displayText);
		}
	}
	return displayText;
}

const FSlateBrush* SKMCharacterOutlinerBeastTreeItem::GetIcon() const
{
	return FSlateIconFinder::FindIconForClass(ASkeletalMeshActor::StaticClass()).GetOptionalIcon();
}
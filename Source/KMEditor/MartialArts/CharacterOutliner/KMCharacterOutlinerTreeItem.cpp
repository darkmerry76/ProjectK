#include "KMCharacterOutlinerTreeItem.h"
#include "EMMartialArtsUtil.h"
#include "Animation/SkeletalMeshActor.h"
#include "Styling/SlateIconFinder.h"

const FEMOutlinerTreeItemType FKMCharacterOutlinerTreeItem::Type(&IEMOutlinerTreeItem::Type);
FKMCharacterOutlinerTreeItem::FKMCharacterOutlinerTreeItem(const FText& text) : FEMOutlinerTextTreeItem(text, FKMCharacterOutlinerTreeItem::Type)
{
}

TSharedRef<SWidget> FKMCharacterOutlinerTreeItem::GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row)
{
	return SNew(SKMCharacterOutlinerTreeItem, *this, outliner, row);
}

void FKMCharacterOutlinerTreeItem::SetCharacterTable(const FKMTable_CharacterRow* newCharacterTable)
{
	CharacterTable = newCharacterTable;
}

const FKMTable_CharacterRow* FKMCharacterOutlinerTreeItem::GetCharacterTable() const
{
	return CharacterTable;
}

FKMCharacterLabelCommttedDelgate& FKMCharacterOutlinerTreeItem::GetCharacterLabelCommttedDelgate()
{
	static FKMCharacterLabelCommttedDelgate characterLabelCommttedDelgate;
	return characterLabelCommttedDelgate;
}

FKMCharacterVerifyItemLabelChangedDelgate& FKMCharacterOutlinerTreeItem::GetCharacterVerifyItemLabelChangedDelgate()
{
	static FKMCharacterVerifyItemLabelChangedDelgate cameraVerifyItemLabelChangedDelgate;
	return cameraVerifyItemLabelChangedDelgate;
}

bool SKMCharacterOutlinerTreeItem::OnVerifyItemLabelChanged(const FText& label, FText& outErrorMessage)
{
	if (FKMCharacterOutlinerTreeItem::GetCharacterVerifyItemLabelChangedDelgate().IsBound())
	{
		bIsValidLabel = FKMCharacterOutlinerTreeItem::GetCharacterVerifyItemLabelChangedDelgate().Execute(StaticCastWeakPtr<FKMCharacterOutlinerTreeItem>(TreeItemPtr), label, outErrorMessage);
	}
	else
	{
		bIsValidLabel = UEMMartialArtsUtil::IsValidAssetName(label.ToString());
	}

	bIsValidLabel &= !label.IsEmpty();
	if (!bIsValidLabel)
	{
		outErrorMessage = FText::FromString(TEXT("Invalid Camera Name"));
	}
	return bIsValidLabel;
}

void SKMCharacterOutlinerTreeItem::OnLabelCommitted(const FText& label, ETextCommit::Type commitInfo)
{
	bIsValidLabel = true;
	FKMCharacterOutlinerTreeItem::GetCharacterLabelCommttedDelgate().ExecuteIfBound(StaticCastWeakPtr<FKMCharacterOutlinerTreeItem>(TreeItemPtr), label, commitInfo);
}

FSlateColor SKMCharacterOutlinerTreeItem::GetForegroundColor() const
{
	if (!bIsValidLabel)
	{
		return FLinearColor::Red;
	}
	return SEMOutlinerTextTreeItem::GetForegroundColor();
}

FText SKMCharacterOutlinerTreeItem::GetDisplayText() const
{
	FText displayText = SEMOutlinerTextTreeItem::GetDisplayText();

	if (TreeItemPtr.IsValid())
	{
		TSharedPtr<FKMCharacterOutlinerTreeItem> cameraItem =StaticCastSharedPtr<FKMCharacterOutlinerTreeItem>(TreeItemPtr.Pin());
		if (cameraItem->GetAsset() && cameraItem->GetAsset()->GetOutermost()->IsDirty())
		{
			displayText = FText::Format(FText::FromString(TEXT("{0}*")),displayText);
		}
	}
	return displayText;
}

const FSlateBrush* SKMCharacterOutlinerTreeItem::GetIcon() const
{
	return FSlateIconFinder::FindIconForClass(ASkeletalMeshActor::StaticClass()).GetOptionalIcon();
}
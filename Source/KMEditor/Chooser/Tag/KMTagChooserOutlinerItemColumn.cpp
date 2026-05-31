#include "KMTagChooserOutlinerItemColumn.h"
#include "EMOutlinerHelpers.h"
#include "EMOutlinerSortHelper.h"
#include "KMTagChooserOutlinerTreeItem.h"

#define LOCTEXT_NAMESPACE "KMTagChooserOutlinerItemColumn"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerItemColumn
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FName FKMTagChooserOutlinerItemColumn::GetID()
{
	return TEXT("Tag");
}

FText FKMTagChooserOutlinerItemColumn::GetLabel()
{
	return LOCTEXT("Tag", "Tag");
}

FName FKMTagChooserOutlinerItemColumn::GetColumnID()
{
	return GetID();
}

SHeaderRow::FColumn::FArguments FKMTagChooserOutlinerItemColumn::ConstructHeaderRowColumn()
{
	return SHeaderRow::Column(GetColumnID())
		.FillWidth( 5.0f );
}
const TSharedRef<SWidget> FKMTagChooserOutlinerItemColumn::ConstructRowWidget(FEMOutlinerTreeItemRef treeItem, const STableRow<FEMOutlinerTreeItemPtr>& row)
{
	IEMOutliner* outliner = WeakOutliner.Pin().Get();
	check(outliner);
	return treeItem->GenerateLabelWidget(*outliner, row);
}

void FKMTagChooserOutlinerItemColumn::PopulateSearchStrings(const IEMOutlinerTreeItem& item, TArray<FString>& outSearchStrings) const
{
	outSearchStrings.Add(item.GetDisplayString());
	EMOutliner::FEMOutlinerHelpers::PopulateExtraSearchStrings(item, outSearchStrings);
}

void FKMTagChooserOutlinerItemColumn::SortItems(TArray<FEMOutlinerTreeItemPtr>& outItems, const EColumnSortMode::Type sortMode) const
{
	typedef FEMOutlinerSortHelper<int32, EMOutliner::FEMNumericStringWrapper> FSort;

	FSort()
		.Primary([this](const IEMOutlinerTreeItem& item){ return WeakOutliner.Pin()->GetTypeSortPriority(item); }, sortMode)
		.Secondary([](const IEMOutlinerTreeItem& item){ return EMOutliner::FEMNumericStringWrapper(item.GetDisplayString()); }, sortMode)
		.Sort(outItems);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserThumbnailColumn
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedRef<IEMOutlinerColumn> FKMTagChooserThumbnailColumn::CreateThumbnailColumn(IEMOutliner& outliner, const FName columnName, const FText columnToolTip)
{
	return TSharedRef<IEMOutlinerColumn>(MakeShareable(new FKMTagChooserThumbnailColumn(outliner, columnName, columnToolTip)));
}

FKMTagChooserThumbnailColumn::FKMTagChooserThumbnailColumn(IEMOutliner& outliner, const FName columnName, const FText columnToolTip)
	: FEMThumbnailColumn(outliner, columnName, columnToolTip)
{
}

TSharedPtr<FAssetThumbnail> FKMTagChooserThumbnailColumn::CreateAssetThumbnail(const IEMOutlinerTreeItem& item)
{
	if (OutlinerWeak.IsValid())
	{
		FAssetData assetData;

		if (item.IsA<FKMTagChooserOutlinerTreeItem>())
		{
			assetData = reinterpret_cast<const FKMTagChooserOutlinerTreeItem*>(&item)->GetAssetData();
		}
/*		else if (item.IsA<FKMTagChooserOutlinerGroupTreeItem>())
		{
			assetData = reinterpret_cast<const FKMTagChooserOutlinerGroupTreeItem*>(&item)->GetAssetData();
		}*/
		
		if (!assetData.IsValid())
		{
			return nullptr;
		}
		
		TSharedPtr<FAssetThumbnailPool> assetThumbnailPool = OutlinerWeak.Pin()->GetAssetThumbnailPool();
		if (assetThumbnailPool.IsValid())
		{
			return MakeShareable(new FAssetThumbnail(assetData, 40, 40, assetThumbnailPool));
		}
	}
	return nullptr;
}
#undef LOCTEXT_NAMESPACE

#include "KMCharacterOutlinerItemColumn.h"
#include "EMOutlinerHelpers.h"
#include "EMOutlinerSortHelper.h"

#define LOCTEXT_NAMESPACE "CharacterOutlinerItemLabelColumn"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMCharacterOutlinerItemLabelColumn
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FName FKMCharacterOutlinerItemLabelColumn::GetID()
{
	return TEXT("Id");
}

FText FKMCharacterOutlinerItemLabelColumn::GetLabel()
{
	return LOCTEXT("Id", "Id");
}

FName FKMCharacterOutlinerItemLabelColumn::GetColumnID()
{
	return GetID();
}

SHeaderRow::FColumn::FArguments FKMCharacterOutlinerItemLabelColumn::ConstructHeaderRowColumn()
{
	return SHeaderRow::Column(GetColumnID())
		.FillWidth( 5.0f );
}
const TSharedRef<SWidget> FKMCharacterOutlinerItemLabelColumn::ConstructRowWidget(FEMOutlinerTreeItemRef treeItem, const STableRow<FEMOutlinerTreeItemPtr>& row)
{
	IEMOutliner* outliner = WeakOutliner.Pin().Get();
	check(outliner);
	return treeItem->GenerateLabelWidget(*outliner, row);
}

void FKMCharacterOutlinerItemLabelColumn::PopulateSearchStrings(const IEMOutlinerTreeItem& item, TArray<FString>& outSearchStrings) const
{
	outSearchStrings.Add(item.GetDisplayString());
	EMOutliner::FEMOutlinerHelpers::PopulateExtraSearchStrings(item, outSearchStrings);
}

void FKMCharacterOutlinerItemLabelColumn::SortItems(TArray<FEMOutlinerTreeItemPtr>& outItems, const EColumnSortMode::Type sortMode) const
{
	typedef FEMOutlinerSortHelper<int32, EMOutliner::FEMNumericStringWrapper> FSort;

	FSort()
		.Primary([this](const IEMOutlinerTreeItem& item){ return WeakOutliner.Pin()->GetTypeSortPriority(item); }, sortMode)
		.Secondary([](const IEMOutlinerTreeItem& item){ return EMOutliner::FEMNumericStringWrapper(item.GetDisplayString()); }, sortMode)
		.Sort(outItems);
}

#undef LOCTEXT_NAMESPACE

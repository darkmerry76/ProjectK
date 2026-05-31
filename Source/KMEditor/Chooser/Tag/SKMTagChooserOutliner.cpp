#include "SKMTagChooserOutliner.h"

#include "AssetToolsModule.h"
#include "KMTagChooserOutlinerItemColumn.h"
#include "SKMTagChooserOutlinerTreeView.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SKMTagChooserOutliner
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SKMTagChooserOutliner::SKMTagChooserOutliner() : SEMOutliner()
{
}

SKMTagChooserOutliner::~SKMTagChooserOutliner()
{
}

void SKMTagChooserOutliner::Construct(const FArguments& args, const FEMOutlinerInitializationOptions& initOptions)
{
	SEMOutliner::Construct(args, initOptions);

	SortByColumn = FKMTagChooserOutlinerItemColumn::GetID();
	SortMode = EColumnSortMode::Ascending;

	TSharedPtr<FAssetThumbnail> AssetThumbnail;
}

void SKMTagChooserOutliner::RegisterColumnThumbnail(EEMOutlinerColumnVisibility visiblity, int32 priority, FName columnID, TAttribute<FText> columnName, const FText& ColumnTooltip)
{
	FEMCreateOutlinerColumn createColumn = FEMCreateOutlinerColumn::CreateStatic(&FKMTagChooserThumbnailColumn::CreateThumbnailColumn, columnID, ColumnTooltip);
	SharedData->ColumnMap.Add(columnID,FEMOutlinerColumnInfo(visiblity, priority, createColumn,true,TOptional<float>(), columnName));
};

void SKMTagChooserOutliner::InitHeaderColumn()
{
	RegisterColumnDefault<FKMTagChooserOutlinerItemColumn>(EEMOutlinerColumnVisibility::Visible, 0, FText::FromName(FKMTagChooserOutlinerItemColumn::GetID()));
	RegisterColumnThumbnail(EEMOutlinerColumnVisibility::Visible, 10, TEXT("Montage"), FText::FromName(TEXT("Montage")));
}

TSharedRef<ITableRow> SKMTagChooserOutliner::OnGenerateRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable)
{
	TSharedRef<SEMOutlinerTreeRow> outlinerTreeRow = SNew(SEMOutlinerTreeRow, OutlinerTreeView.ToSharedRef(), SharedThis(this)).Item(item);
	return outlinerTreeRow;
}

TSharedRef<ITableRow> SKMTagChooserOutliner::OnGeneratePinnedRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable)
{
	return SNew(SEMOutlinerPinnedTreeRow, OutlinerTreeView.ToSharedRef(), SharedThis(this)).Item(item);
}

void SKMTagChooserOutliner::Populate()
{
	SEMOutliner::Populate();
}

TSharedRef<SEMOutlinerTreeView> SKMTagChooserOutliner::CreateOutlinerTreeView()
{
	return MakeShared<SKMTagChooserOutlinerTreeView>();	
}

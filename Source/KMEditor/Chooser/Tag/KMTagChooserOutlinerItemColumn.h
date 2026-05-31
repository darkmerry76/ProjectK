#pragma once

#include "CoreMinimal.h"
#include "EMOutlinerTextInfoColumn.h"
#include "Widgets/SWidget.h"
#include "Widgets/Views/SHeaderRow.h"
#include "IEMOutliner.h"
#include "IEMOutlinerColumn.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerItemColumn
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FKMTagChooserOutlinerItemColumn : public IEMOutlinerColumn
{
public:
	FKMTagChooserOutlinerItemColumn(IEMOutliner& outliner) : WeakOutliner(StaticCastSharedRef<IEMOutliner>(outliner.AsShared())) {}
	virtual ~FKMTagChooserOutlinerItemColumn() override {}

	static FName GetID();
	static FText GetLabel();

	virtual FName GetColumnID() override;
	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;
	virtual const TSharedRef<SWidget> ConstructRowWidget(FEMOutlinerTreeItemRef treeItem, const STableRow<FEMOutlinerTreeItemPtr>& row) override;
	virtual void PopulateSearchStrings( const IEMOutlinerTreeItem& item, TArray<FString>& outSearchStrings) const override;
	virtual bool SupportsSorting() const override { return true; }
	virtual void SortItems(TArray<FEMOutlinerTreeItemPtr>& rootItems, const EColumnSortMode::Type sortMode) const override;

private:
	TWeakPtr<IEMOutliner> WeakOutliner;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserThumbnailColumn
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FKMTagChooserThumbnailColumn : public FEMThumbnailColumn
{
public:
	static TSharedRef<IEMOutlinerColumn> CreateThumbnailColumn(IEMOutliner& outliner, const FName columnName, const FText columnToolTip);
	
	FKMTagChooserThumbnailColumn(IEMOutliner& outliner, const FName columnName, const FText columnToolTip);
	virtual ~FKMTagChooserThumbnailColumn() override {}
	virtual TSharedPtr<class FAssetThumbnail> CreateAssetThumbnail(const IEMOutlinerTreeItem& item) override;
};
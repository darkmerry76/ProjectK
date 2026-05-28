#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Widgets/Views/SHeaderRow.h"
#include "IEMOutliner.h"
#include "IEMOutlinerColumn.h"

template<typename ItemType> class STableRow;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMCharacterOutlinerItemLabelColumn
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FKMCharacterOutlinerItemLabelColumn : public IEMOutlinerColumn
{
public:
	FKMCharacterOutlinerItemLabelColumn(IEMOutliner& outliner) : WeakOutliner(StaticCastSharedRef<IEMOutliner>(outliner.AsShared())) {}
	virtual ~FKMCharacterOutlinerItemLabelColumn() override {}

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
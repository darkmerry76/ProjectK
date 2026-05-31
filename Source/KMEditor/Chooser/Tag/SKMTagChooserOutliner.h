#pragma once

#include "CoreMinimal.h"
#include "SEMOutliner.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SKMTagChooserOutliner
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SKMTagChooserOutliner : public SEMOutliner
{
public:
	SKMTagChooserOutliner(); 
	virtual ~SKMTagChooserOutliner() override;

	virtual void Construct(const FArguments& args, const FEMOutlinerInitializationOptions& initOptions) override;
	
protected:
	virtual void InitHeaderColumn() override;
	virtual void Populate() override;

	virtual TSharedRef<SEMOutlinerTreeView> CreateOutlinerTreeView() override;
	virtual TSharedRef<ITableRow> OnGenerateRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable) override;
	virtual TSharedRef<ITableRow> OnGeneratePinnedRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable) override;

	void RegisterColumnThumbnail(EEMOutlinerColumnVisibility visiblity, int32 priority, FName columnID, TAttribute<FText> columnName, const FText& ColumnTooltip = FText::GetEmpty());


protected:
	const struct FSlateBrush* GetAssetThumbnail(const IEMOutlinerTreeItem& treeItem) const;
};
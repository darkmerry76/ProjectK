#pragma once

#include "CoreMinimal.h"
#include "SEMOutliner.h"

class SKMCharacterOutliner : public SEMOutliner
{
public:
	SKMCharacterOutliner(); 
	virtual ~SKMCharacterOutliner() override;

	virtual void Construct(const FArguments& args, const FEMOutlinerInitializationOptions& initOptions) override;
	
protected:
	virtual void InitHeaderColumn() override;
	virtual void Populate() override;

	virtual TSharedRef<SEMOutlinerTreeView> CreateOutlinerTreeView() override;
	virtual TSharedRef<ITableRow> OnGenerateRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable) override;
	virtual TSharedRef<ITableRow> OnGeneratePinnedRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable) override;

protected:
	FString GetTypeRowText(const IEMOutlinerTreeItem& treeItem) const;
	FString GetNameRowText(const IEMOutlinerTreeItem& treeItem) const;
};
#pragma once

#include "CoreMinimal.h"
#include "EMOutlinerMode.h"
#include "SEMOutliner.h"

class FKMCharacterOutlinerMode : public FEMOutlinerMode
{
public:
	FKMCharacterOutlinerMode(class SEMOutliner* sceneOutliner);
	virtual ~FKMCharacterOutlinerMode() override;

protected:
	virtual TSharedPtr<SWidget> CreateContextMenu() override;
	virtual TUniquePtr<IEMOutlinerHierarchy> CreateHierarchy() override;
	virtual void BindCommands(const TSharedRef<class FUICommandList>& outCommandList) override;
	virtual bool IsInteractive() const override { return true; }

	virtual TSharedPtr<FDragDropOperation> CreateDragDropOperation(const FPointerEvent& mouseEvent, const TArray<FEMOutlinerTreeItemPtr>& treeItems) const override;

	virtual bool ParseDragDrop(FEMOutlinerDragDropPayload& outPayload, const FDragDropOperation& operation) const override;
	virtual void OnDrop(IEMOutlinerTreeItem& dropTarget, const FEMOutlinerDragDropPayload& payload, const FEMOutlinerDragValidationInfo& validationInfo) const override;
	virtual FEMOutlinerDragValidationInfo ValidateDrop(const IEMOutlinerTreeItem& dropTarget, const FEMOutlinerDragDropPayload& payload) const override;

	virtual bool SupportsSaveAsAsset() const override { return false; }
	virtual bool SupportsCreateNewFolder() const override { return false; }

	virtual FText GetStatusText() const override;

protected:
	TWeakPtr<struct IEMOutlinerTreeItem> GetSelectedSingle() const; 

	void OnAssetRemoved(const FAssetData& assetData);
};
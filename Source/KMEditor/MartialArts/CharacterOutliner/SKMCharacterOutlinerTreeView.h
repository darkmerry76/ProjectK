#pragma once

#include "CoreMinimal.h"
#include "SEMOutlinerTreeView.h"

struct FWeakCharacterOutlinerItemSelector
{
	bool operator()(const TWeakPtr<IEMOutlinerTreeItem>& item, TSharedPtr<struct FKMCharacterOutlinerTreeItem>& outCameraItem) const;
};

class SKMCharacterOutlinerTreeDecorator : public FEMOutlinerTreeDecorator
{
public:
	SKMCharacterOutlinerTreeDecorator(SEMOutlinerTreeView& outlinerTreeView);
	virtual void UpdateOperationDecorator(const FDragDropEvent& event, const FEMOutlinerDragValidationInfo& validationInfo) override;
	virtual void ResetOperationDecorator(const FDragDropEvent& event) override;
	virtual FReply HandleOnDragDetected(const FGeometry& myGeometry, const FPointerEvent& mouseEvent) override;
	virtual FReply HandleDrop(TSharedPtr<SEMOutliner> OutlinerPtr, const FDragDropEvent& DragDropEvent, IEMOutlinerTreeItem& DropTarget, FEMOutlinerDragValidationInfo& ValidationInfo, bool bApplyDrop = false) override;
	virtual FReply HandleDropFromWeak(TWeakPtr<SEMOutliner> outlinerWeak, const FDragDropEvent& DragDropEvent, FEMOutlinerDragValidationInfo& ValidationInfo, bool bApplyDrop = false) override;
};

class SKMCharacterOutlinerTreeView : public SEMOutlinerTreeView
{
protected:
	virtual TSharedPtr<FEMOutlinerTreeDecorator> CreateDocorator() override;
};
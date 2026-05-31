#include "SKMTagChooserOutlinerTreeView.h"
#include "KMTagChooserOutlinerTreeItem.h"
#include "IEMOutlinerMode.h"
#include "SEMOutliner.h"

#define LOCTEXT_NAMESPACE "SKMTagChooserOutlinerTreeView"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FWeakChooserOutlinerItemSelector
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FKMWeakTagChooserOutlinerItemSelector::operator()(const TWeakPtr<IEMOutlinerTreeItem>& item, TSharedPtr<FKMTagChooserOutlinerTreeItem>& outTreeItem) const
{
	if (TSharedPtr<IEMOutlinerTreeItem> itemPtr = item.Pin())
	{
		if (itemPtr->IsA<FKMTagChooserOutlinerTreeItem>())
		{
			outTreeItem = StaticCastSharedPtr<FKMTagChooserOutlinerTreeItem>(itemPtr);
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SKMTagChooserOutlinerTreeDecorator
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SKMTagChooserOutlinerTreeDecorator::SKMTagChooserOutlinerTreeDecorator(SEMOutlinerTreeView& outlinerTreeView) : FEMOutlinerTreeDecorator(outlinerTreeView)
{

}

void SKMTagChooserOutlinerTreeDecorator::UpdateOperationDecorator(const FDragDropEvent& event, const FEMOutlinerDragValidationInfo& validationInfo)
{
	const FSlateBrush* icon = validationInfo.IsValid() ? FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK")) : FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));

	FDragDropOperation* operation = event.GetOperation().Get();
	if (operation && operation->IsOfType<FDecoratedDragDropOp>())
	{
		FDecoratedDragDropOp* decoratedOp = static_cast<FDecoratedDragDropOp*>(operation);
		decoratedOp->SetToolTip(validationInfo.ValidationText, icon);
	}
}

void SKMTagChooserOutlinerTreeDecorator::ResetOperationDecorator(const FDragDropEvent& event)
{
	FDragDropOperation* operation = event.GetOperation().Get();
	if (operation)
	{
		if (operation->IsOfType<FEMOutlinerDragDropOp>())
		{
			static_cast<FEMOutlinerDragDropOp*>(operation)->ResetTooltip();
		}
		else if (operation->IsOfType<FDecoratedDragDropOp>())
		{
			static_cast<FDecoratedDragDropOp*>(operation)->ResetToDefaultToolTip();
		}
	}
}

FReply SKMTagChooserOutlinerTreeDecorator::HandleOnDragDetected(const FGeometry& myGeometry, const FPointerEvent& mouseEvent)
{
	if (mouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ))
	{
		auto operation = OutlinerTreeView.GetOutlinerPtr().Pin()->CreateDragDropOperation(mouseEvent, OutlinerTreeView.GetSelectedItems());

		if (operation.IsValid())
		{
			return FReply::Handled().BeginDragDrop(operation.ToSharedRef());
		}
	}

	return FReply::Unhandled();
}

FReply SKMTagChooserOutlinerTreeDecorator::HandleDrop(TSharedPtr<SEMOutliner> OutlinerPtr, const FDragDropEvent& DragDropEvent, IEMOutlinerTreeItem& DropTarget, FEMOutlinerDragValidationInfo& ValidationInfo, bool bApplyDrop)
{
	if (!OutlinerPtr.IsValid())
	{
		return FReply::Unhandled();
	}

	const FEMSharedOutlinerData& SharedData = OutlinerPtr->GetSharedData();
	if (!SharedData.bShowParentTree)
	{
		return FReply::Unhandled();
	}
	if (!OutlinerPtr->CanSupportDragAndDrop())
	{
		return FReply::Unhandled();
	}

	FEMOutlinerDragDropPayload DraggedObjects(*DragDropEvent.GetOperation());
	if (!OutlinerPtr->ParseDragDrop(DraggedObjects, *DragDropEvent.GetOperation()))
	{
		return FReply::Unhandled();
	}

	ValidationInfo = OutlinerPtr->ValidateDrop(StaticCast<IEMOutlinerTreeItem&>(DropTarget), DraggedObjects);

	if (!ValidationInfo.IsValid())
	{
		return FReply::Handled();
	}

	if (bApplyDrop)
	{
		OutlinerPtr->OnDropPayload(DropTarget, DraggedObjects, ValidationInfo);
	}

	return FReply::Handled();
}

FReply SKMTagChooserOutlinerTreeDecorator::HandleDropFromWeak(TWeakPtr<SEMOutliner> outlinerWeak, const FDragDropEvent& DragDropEvent, FEMOutlinerDragValidationInfo& ValidationInfo, bool bApplyDrop )
{
	const IEMOutlinerMode* Mode = outlinerWeak.IsValid() ? outlinerWeak.Pin()->GetMode() : nullptr;
	FFolder::FRootObject RootObject = Mode ? Mode->GetRootObject() : FFolder::GetInvalidRootObject();
	FFolder RootFolder(RootObject);
	FKMTagChooserOutlinerGroupTreeItem DropTarget(RootFolder);
	return HandleDrop(outlinerWeak.Pin(), DragDropEvent, DropTarget, ValidationInfo, bApplyDrop);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SEMChooserOutlinerTreeView
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedPtr<FEMOutlinerTreeDecorator> SKMTagChooserOutlinerTreeView::CreateDocorator()
{
	return MakeShared<SKMTagChooserOutlinerTreeDecorator>(*this);
}

#undef LOCTEXT_NAMESPACE

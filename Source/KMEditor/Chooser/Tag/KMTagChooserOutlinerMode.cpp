#include "KMTagChooserOutlinerMode.h"
#include "KMTagChooserOutlinerHierarchy.h"
#include "EMOutlinerTreeItem.h"

#define LOCTEXT_NAMESPACE "KMTagChooserOutlinerMode"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerMode
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FKMTagChooserOutlinerMode::FKMTagChooserOutlinerMode(SEMOutliner* sceneOutliner) : FEMOutlinerMode(sceneOutliner)
{
}

FKMTagChooserOutlinerMode::~FKMTagChooserOutlinerMode()
{
}

void FKMTagChooserOutlinerMode::BindCommands(const TSharedRef<FUICommandList>& outCommandList)
{
}

FText FKMTagChooserOutlinerMode::GetStatusText() const
{
	if (const FKMTagChooserOutlinerHierarchy* enumChooserOutlinerHierarchy = static_cast<FKMTagChooserOutlinerHierarchy*>(GetHierarchy()))
	{
		return FText::Format(FText::FromString(TEXT("{0} Tags")), enumChooserOutlinerHierarchy->NumItemCount());
	}
	return FText::GetEmpty();
}

TUniquePtr<IEMOutlinerHierarchy> FKMTagChooserOutlinerMode::CreateHierarchy()
{
	FKMTagChooserOutlinerHierarchy* newHierarchy = new FKMTagChooserOutlinerHierarchy(this);
	return TUniquePtr<FKMTagChooserOutlinerHierarchy>(newHierarchy);
}

TWeakPtr<IEMOutlinerTreeItem> FKMTagChooserOutlinerMode::GetSelectedSingle() const
{
	const FEMOutlinerItemSelection itemSelection(Outliner->GetSelection());

	if (itemSelection.SelectedItems.IsEmpty())
	{
		return nullptr;
	}
	return StaticCastWeakPtr<FEMOutlinerTreeItem>(itemSelection.SelectedItems[0]);
}

TSharedPtr<SWidget> FKMTagChooserOutlinerMode::CreateContextMenu()
{
	return SNullWidget::NullWidget;
}

TSharedPtr<FDragDropOperation> FKMTagChooserOutlinerMode::CreateDragDropOperation(const FPointerEvent& mouseEvent, const TArray<FEMOutlinerTreeItemPtr>& treeItems) const
{
	return nullptr;
}

bool FKMTagChooserOutlinerMode::ParseDragDrop(FEMOutlinerDragDropPayload& outPayload, const FDragDropOperation& operation) const
{
	return false;
}

void FKMTagChooserOutlinerMode::OnDrop(IEMOutlinerTreeItem& dropTarget, const FEMOutlinerDragDropPayload& payload, const FEMOutlinerDragValidationInfo& validationInfo) const
{
}

FEMOutlinerDragValidationInfo FKMTagChooserOutlinerMode::ValidateDrop(const IEMOutlinerTreeItem& dropTarget, const FEMOutlinerDragDropPayload& payload) const
{
	return FEMOutlinerDragValidationInfo(EEMOutlinerDropCompatibility::Incompatible, LOCTEXT("InvalidPayload", "Unsupported drag payload"));
}

#undef LOCTEXT_NAMESPACE
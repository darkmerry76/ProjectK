#include "KMCharacterOutlinerMode.h"
#include "EMOutlinerTreeItem.h"
#include "KMCharacterOutlinerHierarchy.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "KMCharacterOutlinerMode"

FKMCharacterOutlinerMode::FKMCharacterOutlinerMode(SEMOutliner* sceneOutliner) : FEMOutlinerMode(sceneOutliner)
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	if (!assetRegistryModule.Get().OnAssetRemoved().IsBoundToObject(this))
	{
		assetRegistryModule.Get().OnAssetRemoved().AddRaw(this, &FKMCharacterOutlinerMode::OnAssetRemoved);
	}
}

FKMCharacterOutlinerMode::~FKMCharacterOutlinerMode()
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	if (assetRegistryModule.Get().OnAssetRemoved().IsBoundToObject(this))
	{
		assetRegistryModule.Get().OnAssetRemoved().RemoveAll(this);
	}
}

void FKMCharacterOutlinerMode::BindCommands(const TSharedRef<FUICommandList>& outCommandList)
{
}

FText FKMCharacterOutlinerMode::GetStatusText() const
{
	if (const FKMCharacterOutlinerHierarchy* characterOutlinerHierarchy = static_cast<FKMCharacterOutlinerHierarchy*>(GetHierarchy()))
	{
		return FText::Format(FText::FromString(TEXT("{0} Characters")), characterOutlinerHierarchy->NumCharacter());
	}
	return FText::GetEmpty();
}

TUniquePtr<IEMOutlinerHierarchy> FKMCharacterOutlinerMode::CreateHierarchy()
{
	FKMCharacterOutlinerHierarchy* newHierarchy = new FKMCharacterOutlinerHierarchy(this);
	return TUniquePtr<FKMCharacterOutlinerHierarchy>(newHierarchy);
}

TWeakPtr<IEMOutlinerTreeItem> FKMCharacterOutlinerMode::GetSelectedSingle() const
{
	const FEMOutlinerItemSelection itemSelection(Outliner->GetSelection());

	if (itemSelection.SelectedItems.IsEmpty())
	{
		return nullptr;
	}
	return StaticCastWeakPtr<FEMOutlinerTreeItem>(itemSelection.SelectedItems[0]);
}

TSharedPtr<SWidget> FKMCharacterOutlinerMode::CreateContextMenu()
{
	return SNullWidget::NullWidget;
}

TSharedPtr<FDragDropOperation> FKMCharacterOutlinerMode::CreateDragDropOperation(const FPointerEvent& mouseEvent, const TArray<FEMOutlinerTreeItemPtr>& treeItems) const
{
	return nullptr;
}

bool FKMCharacterOutlinerMode::ParseDragDrop(FEMOutlinerDragDropPayload& outPayload, const FDragDropOperation& operation) const
{
	return false;
}

void FKMCharacterOutlinerMode::OnDrop(IEMOutlinerTreeItem& dropTarget, const FEMOutlinerDragDropPayload& payload, const FEMOutlinerDragValidationInfo& validationInfo) const
{
}

FEMOutlinerDragValidationInfo FKMCharacterOutlinerMode::ValidateDrop(const IEMOutlinerTreeItem& dropTarget, const FEMOutlinerDragDropPayload& payload) const
{
	return FEMOutlinerDragValidationInfo(EEMOutlinerDropCompatibility::Incompatible, LOCTEXT("InvalidPayload", "Unsupported drag payload"));
}

void FKMCharacterOutlinerMode::OnAssetRemoved(const FAssetData& assetData)
{
}

#undef LOCTEXT_NAMESPACE
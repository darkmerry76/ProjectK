#pragma once

#include "CoreMinimal.h"
#include "EMOutlinerHierarchy.h"
#include "GameplayTagContainer.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMTagChooserOutlinerSelectedDelegate, const FGameplayTag& tag);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerHierarchy
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FKMTagChooserOutlinerHierarchy : public FEMOutlinerHierarchy
{
public:
	FKMTagChooserOutlinerHierarchy(class IEMOutlinerMode* mode);
	virtual ~FKMTagChooserOutlinerHierarchy() override;

	static TUniquePtr<FEMOutlinerHierarchy> CreateHierarchy(IEMOutlinerMode* mode);

	TSharedPtr<IEMOutlinerTreeItem> GetParentItem(const TSharedPtr<IEMOutlinerTreeItem>& item) const;

	int32 NumItemCount() const;

	static TArray<FEMOutlinerTreeItemPtr> RememberAllItems;
	static TMap<TSharedPtr<IEMOutlinerTreeItem>, TSharedPtr<IEMOutlinerTreeItem>> RememberParentItems;
	static FKMTagChooserOutlinerSelectedDelegate SelectedDelegate;
	
	TMap<TSharedPtr<IEMOutlinerTreeItem>, TSharedPtr<IEMOutlinerTreeItem>> ParentItems;
	int32 ItemCount = 0;
	
protected:
	virtual void Init() override;
	virtual void CreateItems(TArray<FEMOutlinerTreeItemPtr>& outItems) const override;
	
	template <typename TreeItemType, typename TreeItemData>
	TSharedPtr<TreeItemType> AddItem(const TreeItemData& data, TSharedPtr<IEMOutlinerTreeItem> parentItem = nullptr, bool bForce = false);
	virtual FEMOutlinerTreeItemPtr FindOrCreateParentItem(const IEMOutlinerTreeItem& item, const TMap<FEMOutlinerTreeItemID, FEMOutlinerTreeItemPtr>& items, bool bCreate = false) override;
};
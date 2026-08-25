#pragma once

#include "CoreMinimal.h"
#include "EMOutlinerHierarchy.h"
#include "Tables/Generated/KMTableEnums.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEMCharacterOutlinerSelectedDelegate, const struct FKMTable_Object_CharacterRow* newCharacterTable)
DECLARE_MULTICAST_DELEGATE_OneParam(FEMBeastOutlinerSelectedDelegate, const struct FKMTable_Object_BeastRow* newBeastTable)

class FKMCharacterOutlinerHierarchy : public FEMOutlinerHierarchy
{
public:
	FKMCharacterOutlinerHierarchy(class IEMOutlinerMode* mode);
	virtual ~FKMCharacterOutlinerHierarchy() override;

	static TUniquePtr<FEMOutlinerHierarchy> CreateHierarchy(IEMOutlinerMode* mode);

	void SelectItem(FEMOutlinerTreeItemPtr treeItem);

	TSharedPtr<IEMOutlinerTreeItem> GetParentItem(const TSharedPtr<IEMOutlinerTreeItem>& item) const;

	int32 NumCharacter() const;

	static TArray<FEMOutlinerTreeItemPtr> RememberAllItems;
	static TMap<TSharedPtr<IEMOutlinerTreeItem>, TSharedPtr<IEMOutlinerTreeItem>> RememberParentItems;
	static FEMCharacterOutlinerSelectedDelegate CharacterSelectedDelegate;
	static FEMBeastOutlinerSelectedDelegate BeastSelectedDelegate;
	
protected:
	virtual void Init() override;
	virtual void CreateItems(TArray<FEMOutlinerTreeItemPtr>& outItems) const override;
	
	void OnMouseDoubleClick(FEMOutlinerTreeItemPtr treeItem);
	void OnItemSelectionChanged(FEMOutlinerTreeItemPtr treeItem, ESelectInfo::Type type);

	template <typename TreeItemType, typename TreeItemData>
	TSharedPtr<TreeItemType>  AddItem(const TreeItemData& data, TSharedPtr<IEMOutlinerTreeItem> parentItem = nullptr, bool bForce = false);
	virtual FEMOutlinerTreeItemPtr FindOrCreateParentItem(const IEMOutlinerTreeItem& item, const TMap<FEMOutlinerTreeItemID, FEMOutlinerTreeItemPtr>& items, bool bCreate = false) override;

protected:
	TMap<TSharedPtr<IEMOutlinerTreeItem>, TSharedPtr<IEMOutlinerTreeItem>> ParentItems;
	TMap<EKMObjectType, TSharedPtr<struct FKMCharacterOutlinerGroupTreeItem>> CharacterTypeItems;
	int32 CharacterCount = 0;
};
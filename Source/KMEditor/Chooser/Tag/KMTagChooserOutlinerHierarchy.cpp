#include "KMTagChooserOutlinerHierarchy.h"
#include "KMTagChooserOutlinerTreeItem.h"
#include "EMOutlinerMode.h"
#include "GameplayTagsManager.h"
#include "KMTagChooserOutlinerMode.h"
#include "SKMTagChooserOutliner.h"
#include "AssetRegistry/AssetRegistryModule.h"

TArray<FEMOutlinerTreeItemPtr> FKMTagChooserOutlinerHierarchy::RememberAllItems;
TMap<TSharedPtr<IEMOutlinerTreeItem>, TSharedPtr<IEMOutlinerTreeItem>> FKMTagChooserOutlinerHierarchy::RememberParentItems;
FKMTagChooserOutlinerSelectedDelegate FKMTagChooserOutlinerHierarchy::SelectedDelegate;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerHierarchy
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TUniquePtr<FEMOutlinerHierarchy> FKMTagChooserOutlinerHierarchy::CreateHierarchy(IEMOutlinerMode* mode)
{
	return TUniquePtr<FEMOutlinerHierarchy>(new FKMTagChooserOutlinerHierarchy(mode));
}
FKMTagChooserOutlinerHierarchy::FKMTagChooserOutlinerHierarchy(IEMOutlinerMode* mode) : FEMOutlinerHierarchy(mode)
{
}

FKMTagChooserOutlinerHierarchy::~FKMTagChooserOutlinerHierarchy()
{
}

void FKMTagChooserOutlinerHierarchy::CreateItems(TArray<FEMOutlinerTreeItemPtr>& outItems) const
{
	for (auto treeItem : AllItems)
	{
		outItems.Add(treeItem);	
	}
}

TSharedPtr<IEMOutlinerTreeItem> FKMTagChooserOutlinerHierarchy::GetParentItem(const TSharedPtr<IEMOutlinerTreeItem>& item) const
{
	const TSharedPtr<IEMOutlinerTreeItem>* parentItem = ParentItems.Find(item);
	if (!parentItem)
	{
		return nullptr;
	}
	return *parentItem;
}

FEMOutlinerTreeItemPtr FKMTagChooserOutlinerHierarchy::FindOrCreateParentItem(const IEMOutlinerTreeItem& item, const TMap<FEMOutlinerTreeItemID, FEMOutlinerTreeItemPtr>& items, bool bCreate)
{
	TSharedPtr<IEMOutlinerTreeItem>* parentItem = ParentItems.Find(item.AsShared().ToSharedPtr());
	if (!parentItem)
	{
		return nullptr;
	}

	if (!bCreate)
	{
		if (!items.Contains((*parentItem)->GetID()))
		{
			return nullptr;
		}
	}	
	return *parentItem;
}

template <typename TreeItemType, typename TreeItemData>
TSharedPtr<TreeItemType>  FKMTagChooserOutlinerHierarchy::AddItem(const TreeItemData& data, TSharedPtr<IEMOutlinerTreeItem> parentItem, bool bForce)
{
	TSharedPtr<TreeItemType> newOutlinerItem = StaticCastSharedPtr<TreeItemType>(Mode->CreateItemFor<TreeItemType>(data, bForce));
	if (newOutlinerItem.IsValid())
	{
		if (parentItem.IsValid())
		{
			ParentItems.FindOrAdd(newOutlinerItem, parentItem);
		}
		AllItems.Emplace(newOutlinerItem);
	}	
	return newOutlinerItem;
}

void FKMTagChooserOutlinerHierarchy::Init()
{
	AllItems.Empty();
	ParentItems.Empty();

	SKMTagChooserOutliner* chooserOutliner = static_cast<SKMTagChooserOutliner*>(Mode->GetOutliner());

	FGameplayTag rootTag = FGameplayTag::RequestGameplayTag(TEXT("Anim"));
	FGameplayTagContainer tagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(rootTag);

	TMap<FGameplayTag, TSharedPtr<IEMOutlinerTreeItem>> tagItemMap;
	for (const FGameplayTag& tag : tagContainer)
	{
		FGameplayTagContainer childTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(tag);

		TSharedPtr<IEMOutlinerTreeItem> parentItem;
		const FGameplayTag parentTag = tag.RequestDirectParent();
		if (parentTag.IsValid())
		{
			if (TSharedPtr<IEMOutlinerTreeItem>* existParent = tagItemMap.Find(parentTag))
			{
				parentItem = *existParent;
			}
		}

		FString itemName;
		TArray<FString> tagParts;
		tag.GetTagName().ToString().ParseIntoArray(tagParts, TEXT("."), true);
		if (!tagParts.IsEmpty())
		{
			for (int32 tagIndex = 0; tagIndex < tagParts.Num(); ++tagIndex)
			{
				itemName += TEXT("/") + tagParts[tagIndex];
			}
		}
		TSharedPtr<IEMOutlinerTreeItem> newItem;
		FFolder itemFolder(Mode->GetRootObject(), *itemName);
		if (!childTags.IsEmpty())
		{
			TSharedPtr<FKMTagChooserOutlinerGroupTreeItem> groupItem = AddItem<FKMTagChooserOutlinerGroupTreeItem>(itemFolder, parentItem,true);
			groupItem->SetTag(tag);
			chooserOutliner->AddItemDelegate.ExecuteIfBound(groupItem);
			newItem = groupItem;
		}
		else
		{
			TSharedPtr<FKMTagChooserOutlinerTreeItem> treeItem = AddItem<FKMTagChooserOutlinerTreeItem>(FText::FromName(itemFolder.GetLeafName()), parentItem,true);
			treeItem->SetTag(tag);
			chooserOutliner->AddItemDelegate.ExecuteIfBound(treeItem);
			newItem = treeItem;
			ItemCount++;

		}
		if (newItem.IsValid())
		{
			tagItemMap.Add(tag, newItem);
		}
	}

	RememberAllItems = AllItems;
	RememberParentItems = ParentItems;
}

int32 FKMTagChooserOutlinerHierarchy::NumItemCount() const
{
	return ItemCount;
}
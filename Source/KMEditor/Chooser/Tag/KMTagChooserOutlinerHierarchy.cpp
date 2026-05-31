#include "KMTagChooserOutlinerHierarchy.h"
#include "KMTagChooserOutlinerTreeItem.h"
#include "EMOutlinerMode.h"
#include "GameplayTagsManager.h"
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
	TSharedPtr<IEMOutlinerTreeItem>* parentItem = ParentItems.Find(item.AsShared());
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

	FGameplayTag rootTag = FGameplayTag::RequestGameplayTag(TEXT("Anim"));
	FGameplayTagContainer tagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(rootTag);

	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FAssetData assetData = assetRegistryModule.Get().GetAssetByObjectPath(
			FSoftObjectPath(TEXT("/Game/KM/World/Character/Hero/RyuX/Animation/Code_CDY_NGD_6_Montage.Code_CDY_NGD_6_Montage")));

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

		FString itemName = TEXT("None");
		TArray<FString> tagParts;
		tag.GetTagName().ToString().ParseIntoArray(tagParts, TEXT("."), true);
		if (!tagParts.IsEmpty())
		{
			itemName = tagParts[tagParts.Num() - 1];
		}
		TSharedPtr<IEMOutlinerTreeItem> newItem;
		if (!childTags.IsEmpty())
		{
			TSharedPtr<FKMTagChooserOutlinerGroupTreeItem> groupItem = AddItem<FKMTagChooserOutlinerGroupTreeItem>(FFolder(Mode->GetRootObject(), *itemName), parentItem,true);;
			groupItem->SetTag(tag);
			groupItem->SetAssetData(assetData);
			newItem = groupItem;
		}
		else
		{
			TSharedPtr<FKMTagChooserOutlinerTreeItem> treeItem = AddItem<FKMTagChooserOutlinerTreeItem>(FText::FromString(itemName), parentItem,true);
			treeItem->SetTag(tag);
			treeItem->SetAssetData(assetData);
			newItem = treeItem;

		}
		if (newItem.IsValid())
		{
			ItemCount++;
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
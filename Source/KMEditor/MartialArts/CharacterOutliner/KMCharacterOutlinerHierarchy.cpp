#include "KMCharacterOutlinerHierarchy.h"
#include "EMDataTable.h"
#include "EMOutlinerMode.h"
#include "KMCharacterOutlinerBeastTreeItem.h"
#include "KMCharacterOutlinerGroupTreeItem.h"
#include "KMCharacterOutlinerTreeItem.h"
#include "Core/KMDefine.h"
#include "Tables/Generated/KMTable_Object_Beast.h"
#include "Tables/Generated/KMTable_Object_Character.h"

TArray<FEMOutlinerTreeItemPtr> FKMCharacterOutlinerHierarchy::RememberAllItems;
TMap<TSharedPtr<IEMOutlinerTreeItem>, TSharedPtr<IEMOutlinerTreeItem>> FKMCharacterOutlinerHierarchy::RememberParentItems;
FEMCharacterOutlinerSelectedDelegate FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate;
FEMBeastOutlinerSelectedDelegate FKMCharacterOutlinerHierarchy::BeastSelectedDelegate;

TUniquePtr<FEMOutlinerHierarchy> FKMCharacterOutlinerHierarchy::CreateHierarchy(IEMOutlinerMode* mode)
{
	return TUniquePtr<FEMOutlinerHierarchy>(new FKMCharacterOutlinerHierarchy(mode));
}

FKMCharacterOutlinerHierarchy::FKMCharacterOutlinerHierarchy(IEMOutlinerMode* mode) : FEMOutlinerHierarchy(mode)
{
	if (FEMOutlinerMode* outlinerMode = static_cast<FEMOutlinerMode*>(Mode))
	{
		outlinerMode->GetOutliner()->GetDoubleClickEvent().AddRaw(this, &FKMCharacterOutlinerHierarchy::OnMouseDoubleClick);
		outlinerMode->GetOutliner()->GetOnItemSelectionChanged().AddRaw(this, &FKMCharacterOutlinerHierarchy::OnItemSelectionChanged);
	}

	if (!FKMCharacterOutlinerTreeItem::GetCharacterLabelCommttedDelgate().IsBoundToObject(this))
	{
	}
	if (!FKMCharacterOutlinerTreeItem::GetCharacterVerifyItemLabelChangedDelgate().IsBoundToObject(this))
	{
	}
	if (!FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupLabelCommttedDelgate().IsBoundToObject(this))
	{
	}
	if (!FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupVerifyItemLabelChangedDelgate().IsBoundToObject(this))
	{
	}
}

FKMCharacterOutlinerHierarchy::~FKMCharacterOutlinerHierarchy()
{
	if (FEMOutlinerMode* outlinerMode = static_cast<FEMOutlinerMode*>(Mode))
	{
		outlinerMode->GetOutliner()->GetDoubleClickEvent().RemoveAll(this);
		outlinerMode->GetOutliner()->GetOnItemSelectionChanged().RemoveAll(this);
	}
	if (FKMCharacterOutlinerTreeItem::GetCharacterLabelCommttedDelgate().IsBoundToObject(this))
	{
		FKMCharacterOutlinerTreeItem::GetCharacterLabelCommttedDelgate().Unbind();
	}
	if (FKMCharacterOutlinerTreeItem::GetCharacterVerifyItemLabelChangedDelgate().IsBoundToObject(this))
	{
		FKMCharacterOutlinerTreeItem::GetCharacterVerifyItemLabelChangedDelgate().Unbind();
	}
	if (FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupLabelCommttedDelgate().IsBoundToObject(this))
	{
		FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupLabelCommttedDelgate().Unbind();
	}
	if (FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupVerifyItemLabelChangedDelgate().IsBoundToObject(this))
	{
		FKMCharacterOutlinerGroupTreeItem::GetCharacterGroupVerifyItemLabelChangedDelgate().Unbind();
	}
}

void FKMCharacterOutlinerHierarchy::CreateItems(TArray<FEMOutlinerTreeItemPtr>& outItems) const
{
	for (auto treeItem : AllItems)
	{
		outItems.Add(treeItem);	
	}
}

TSharedPtr<IEMOutlinerTreeItem> FKMCharacterOutlinerHierarchy::GetParentItem(const TSharedPtr<IEMOutlinerTreeItem>& item) const
{
	const TSharedPtr<IEMOutlinerTreeItem>* parentItem = ParentItems.Find(item);
	if (!parentItem)
	{
		return nullptr;
	}
	return *parentItem;
}

FEMOutlinerTreeItemPtr FKMCharacterOutlinerHierarchy::FindOrCreateParentItem(const IEMOutlinerTreeItem& item, const TMap<FEMOutlinerTreeItemID, FEMOutlinerTreeItemPtr>& items, bool bCreate)
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
TSharedPtr<TreeItemType>  FKMCharacterOutlinerHierarchy::AddItem(const TreeItemData& data, TSharedPtr<IEMOutlinerTreeItem> parentItem, bool bForce)
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

void FKMCharacterOutlinerHierarchy::Init()
{
	AllItems.Empty();
	ParentItems.Empty();

	UEnum* characterTypeEnum = KMGame::GetCharacterTypeEnum();

	for (int32 enumIndex = 0; enumIndex < characterTypeEnum->NumEnums() - 1; ++enumIndex)
	{
		FString characterEnumName = characterTypeEnum->GetNameStringByIndex(enumIndex);

		EKMObjectType objectType = static_cast<EKMObjectType>(characterTypeEnum->GetValueByIndex(enumIndex));
		
		TSharedPtr<FKMCharacterOutlinerGroupTreeItem> characterGroupItem =
			AddItem<FKMCharacterOutlinerGroupTreeItem>(FFolder(Mode->GetRootObject(), *characterEnumName),nullptr, true);
		CharacterTypeItems.Emplace(objectType, characterGroupItem);
	}

	const TMap<FName, FKMTable_Object_CharacterRow*> characterRows = FEMDataTableHelper::Get().GetRowMap<FKMTable_Object_CharacterRow>();
	CharacterCount = characterRows.Num();
	for (auto characterItr = characterRows.CreateConstIterator(); characterItr; ++characterItr)
	{
		const FKMTable_Object_CharacterRow* characterTable = characterItr.Value();
		if (!characterTable)
		{
			continue;;
		}

		const TSharedPtr<FKMCharacterOutlinerGroupTreeItem>* characterGroupItem = CharacterTypeItems.Find(characterTable->Type);
		if (!characterGroupItem || !characterGroupItem->IsValid())
		{
			continue;
		}
		
		TSharedPtr<FKMCharacterOutlinerTreeItem> characterItem = AddItem<FKMCharacterOutlinerTreeItem>(FText::FromName(characterTable->Id), *characterGroupItem);
		if (characterItem.IsValid())
		{
			characterItem->SetCharacterTable(characterTable);
		}
	}

	const TMap<FName, FKMTable_Object_BeastRow*> beastRows = FEMDataTableHelper::Get().GetRowMap<FKMTable_Object_BeastRow>();
	CharacterCount += beastRows.Num();
	for (auto beastItr = beastRows.CreateConstIterator(); beastItr; ++beastItr)
	{
		const FKMTable_Object_BeastRow* beastTableRow = beastItr.Value();
		if (!beastTableRow)
		{
			continue;;
		}

		const TSharedPtr<FKMCharacterOutlinerGroupTreeItem>* characterGroupItem = CharacterTypeItems.Find(beastTableRow->Type);
		if (!characterGroupItem || !characterGroupItem->IsValid())
		{
			continue;
		}
		
		TSharedPtr<FKMCharacterOutlinerBeastTreeItem> beastItem = AddItem<FKMCharacterOutlinerBeastTreeItem>(FText::FromName(beastTableRow->Id), *characterGroupItem);
		if (beastItem.IsValid())
		{
			beastItem->SetBeastTable(beastTableRow);
		}
	}
	
	RememberAllItems = AllItems;
	RememberParentItems = ParentItems;
}

int32 FKMCharacterOutlinerHierarchy::NumCharacter() const
{
	return CharacterCount;
}

void FKMCharacterOutlinerHierarchy::OnItemSelectionChanged(FEMOutlinerTreeItemPtr treeItem, ESelectInfo::Type type)
{

}

void FKMCharacterOutlinerHierarchy::OnMouseDoubleClick(FEMOutlinerTreeItemPtr treeItem)
{
	SelectItem(treeItem);
}

void FKMCharacterOutlinerHierarchy::SelectItem(FEMOutlinerTreeItemPtr treeItem)
{
	if (!treeItem.IsValid())
	{
		return;
	}

	if (treeItem->IsA<FKMCharacterOutlinerTreeItem>())
	{
		TSharedPtr<FKMCharacterOutlinerTreeItem> characterTeeItem = StaticCastSharedPtr<FKMCharacterOutlinerTreeItem>(treeItem);
		if (characterTeeItem.IsValid())
		{
			TSharedPtr<FKMCharacterOutlinerGroupTreeItem> parentTreeItem = StaticCastSharedPtr<FKMCharacterOutlinerGroupTreeItem>(GetParentItem(characterTeeItem));
			if (!parentTreeItem.IsValid())
			{
				return;
			}
			if (!parentTreeItem->IsA<FKMCharacterOutlinerGroupTreeItem>())
			{
				return;
			}
			CharacterSelectedDelegate.Broadcast(characterTeeItem->GetCharacterTable());
		}
	}
	else if (treeItem->IsA<FKMCharacterOutlinerBeastTreeItem>())
	{
		TSharedPtr<FKMCharacterOutlinerBeastTreeItem> beastTeeItem = StaticCastSharedPtr<FKMCharacterOutlinerBeastTreeItem>(treeItem);
		if (beastTeeItem.IsValid())
		{
			TSharedPtr<FKMCharacterOutlinerGroupTreeItem> parentTreeItem = StaticCastSharedPtr<FKMCharacterOutlinerGroupTreeItem>(GetParentItem(beastTeeItem));
			if (!parentTreeItem.IsValid())
			{
				return;
			}
			if (!parentTreeItem->IsA<FKMCharacterOutlinerGroupTreeItem>())
			{
				return;
			}
			BeastSelectedDelegate.Broadcast(beastTeeItem->GetBeastTable());
		}
	}
}
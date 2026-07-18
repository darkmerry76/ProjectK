#include "SKMCharacterOutliner.h"

#include "EMOutlinerTextInfoColumn.h"
#include "KMCharacterOutlinerBeastTreeItem.h"
#include "KMCharacterOutlinerGroupTreeItem.h"
#include "KMCharacterOutlinerItemColumn.h"
#include "KMCharacterOutlinerMode.h"
#include "KMCharacterOutlinerTreeItem.h"
#include "SKMCharacterOutlinerTreeView.h"
#include "Core/KMDefine.h"
#include "Tables/Generated/KMTable_Beast.h"
#include "Tables/Generated/KMTable_Character.h"

SKMCharacterOutliner::SKMCharacterOutliner() : SEMOutliner()
{
}

SKMCharacterOutliner::~SKMCharacterOutliner()
{
}

void SKMCharacterOutliner::Construct(const FArguments& args, const FEMOutlinerInitializationOptions& initOptions)
{
	SEMOutliner::Construct(args, initOptions);

	SortByColumn = FKMCharacterOutlinerItemLabelColumn::GetID();
	SortMode = EColumnSortMode::Ascending;
}

FString SKMCharacterOutliner::GetNameRowText(const IEMOutlinerTreeItem& treeItem) const
{
	if (treeItem.IsA<FKMCharacterOutlinerTreeItem>())
	{
		const FKMCharacterOutlinerTreeItem* characterItem = reinterpret_cast<const FKMCharacterOutlinerTreeItem*>(&treeItem);
		if (characterItem->GetCharacterTable())
		{
			return characterItem->GetCharacterTable()->Name;
		}
	}
	else if (treeItem.IsA<FKMCharacterOutlinerBeastTreeItem>())
	{
		const FKMCharacterOutlinerBeastTreeItem* beastItem = reinterpret_cast<const FKMCharacterOutlinerBeastTreeItem*>(&treeItem);
		if (beastItem->GetBeastTable())
		{
			return beastItem->GetBeastTable()->Name;
		}
	}
	return TEXT("");
}

FString SKMCharacterOutliner::GetTypeRowText(const IEMOutlinerTreeItem& treeItem) const
{
	static UEnum* characterTypeEnum = KMGame::GetCharacterTypeEnum();
	check(IsValid(characterTypeEnum));

	if (treeItem.IsA<FKMCharacterOutlinerTreeItem>())
	{
		const FKMCharacterOutlinerTreeItem* characterItem = reinterpret_cast<const FKMCharacterOutlinerTreeItem*>(&treeItem);
		if (characterItem->GetCharacterTable())
		{
			return characterTypeEnum->GetNameStringByValue(static_cast<int64>(characterItem->GetCharacterTable()->Type));
		}
	}
	else if (treeItem.IsA<FKMCharacterOutlinerBeastTreeItem>())
	{
		return characterTypeEnum->GetNameStringByValue(static_cast<int64>(EKMCharacterType::Beast));
	}
	return TEXT("");
}

void SKMCharacterOutliner::InitHeaderColumn()
{
	RegisterColumnDefault<FKMCharacterOutlinerItemLabelColumn>(EEMOutlinerColumnVisibility::Visible, 0, FText::FromName(FKMCharacterOutlinerItemLabelColumn::GetID()));
	RegisterColumnText(EEMOutlinerColumnVisibility::Visible, 10, TEXT("Name"), FText::FromName(TEXT("Name")), FEMGetTextForItem::CreateRaw(this, &SKMCharacterOutliner::GetNameRowText));
	RegisterColumnText(EEMOutlinerColumnVisibility::Visible, 20, TEXT("Type"), FText::FromName(TEXT("Type")), FEMGetTextForItem::CreateRaw(this, &SKMCharacterOutliner::GetTypeRowText));
}

TSharedRef<ITableRow> SKMCharacterOutliner::OnGenerateRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable)
{
	TSharedRef<SEMOutlinerTreeRow> outlinerTreeRow = SNew(SEMOutlinerTreeRow, OutlinerTreeView.ToSharedRef(), SharedThis(this)).Item(item);
	return outlinerTreeRow;
}

TSharedRef<ITableRow> SKMCharacterOutliner::OnGeneratePinnedRowForOutlinerTree(FEMOutlinerTreeItemPtr item, const TSharedRef<STableViewBase>& ownerTable)
{
	return SNew(SEMOutlinerPinnedTreeRow, OutlinerTreeView.ToSharedRef(), SharedThis(this)).Item(item);
}

void SKMCharacterOutliner::Populate()
{
	SEMOutliner::Populate();
}

TSharedRef<SEMOutlinerTreeView> SKMCharacterOutliner::CreateOutlinerTreeView()
{
	return MakeShared<SKMCharacterOutlinerTreeView>();	
}

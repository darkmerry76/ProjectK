#pragma once

#include "EMOutlinerFolderTreeItem.h"
#include "EMOutlinerTextTreeItem.h"
#include "Widgets/SWidget.h"
#include "Folder.h"
#include "GameplayTagContainer.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserContainer
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FKMTagChooserContainer
{
public:
	void SetTag(const FGameplayTag& tag) { Tag = tag; }
	const FGameplayTag& GetTag() const { return Tag; }

	void SetAssetData(const FAssetData& newAssetData) { AssetData = newAssetData; }
	FAssetData GetAssetData() const { return AssetData; }

protected:
	FGameplayTag Tag;
	FAssetData AssetData;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FKMTagChooserOutlinerTreeItem : public FEMOutlinerTextTreeItem, public FKMTagChooserContainer
{
public:
	static const FEMOutlinerTreeItemType Type;
	FKMTagChooserOutlinerTreeItem(const FText& text);
	
protected:
	virtual TSharedRef<SWidget> GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row) override;

protected:
	const struct FKMTable_CharacterRow* CharacterTable = nullptr;;

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SKMTagChooserOutlinerTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SKMTagChooserOutlinerTreeItem : public SEMOutlinerTextTreeItem
{
protected:
	virtual FSlateColor GetForegroundColor() const override;
	virtual FText GetDisplayText() const override;
	virtual const struct FSlateBrush* GetIcon() const override;
	virtual FText GetTooltipText() const override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMTagChooserOutlinerGroupTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FKMTagChooserOutlinerGroupTreeItem : public FEMOutlinerFolderTreeItem, public FKMTagChooserContainer
{
public:
	static const FEMOutlinerTreeItemType Type;

	FKMTagChooserOutlinerGroupTreeItem(const FFolder& folder, FEMOutlinerTreeItemType type);
	FKMTagChooserOutlinerGroupTreeItem(const FFolder& folder);
	FKMTagChooserOutlinerGroupTreeItem(FName path);
	FKMTagChooserOutlinerGroupTreeItem(FName InPath, FEMOutlinerTreeItemType Type);

public:
	virtual FEMOutlinerTreeItemType GetType() const override { return FKMTagChooserOutlinerGroupTreeItem::Type; }

protected:
	virtual TSharedRef<SWidget> GenerateLabelWidget(IEMOutliner& outliner, const STableRow<FEMOutlinerTreeItemPtr>& row) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SKMTagChooserOutlinerGroupTreeItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SKMTagChooserOutlinerGroupTreeItem : public SEMOutlinerFolderTreeItem
{
protected:
	virtual FSlateColor GetForegroundColor() const override;
	virtual FText GetDisplayText() const override;
	virtual FText GetTooltipText() const override;
};
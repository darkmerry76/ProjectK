#pragma once

#include "EMTableGenerator.h"
#include "EMSchema.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableSchemaMgr
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableSchemaMgr
{
public:
	static EMTableSchemaMgr& Inst();

	EMTableSchemaMgr();

	void Initalize();
	void PostInitalize();

	int32 GetNum() const;

	static const TSharedPtr<EMSchemaItem>	GetRefPathItem(const FString& Path);

	const TSharedPtr<EMSchemaItem>	GetSchemaItem(const FString& Path) const;
	const TSharedPtr<EMSchemaItem>	GetLocalSchemaItem(const FString& Path) const;
	const TSharedPtr<EMSchemaItem>	SearchLocalSchemaItem(const FString& Path) const;

	const FString GetLocalPathToFilePath(const FString& LocalPath) const;

	const TSharedPtr<EMSchemaItem> GetSchemaValueToPath(const FString& Path) const;

	TSet<FString>& GetForceIncludeEnums();

private:
	bool ImportFile(const FString& Path);
	void RefCollection(TSharedPtr<EMSchemaItem> SchemaItem, TSharedPtr<EMSchemaItem> RefChild);

	void AssignItems(const FString& Path, TSharedPtr<EMSchemaItem> Item, TSharedPtr<EMSchemaItem> Parent = nullptr);

protected:
	TMap<FString, TSharedPtr<EMSchemaItem>> Items;
	TMap<FString, TSharedPtr<EMSchemaItem>> LocalItems;
	TMap<FString, TSharedPtr<EMSchemaItem>> DefinitionItems;
	TMap<FString, FString> ItemLocalPathToFilePath;
	TArray<TSharedPtr<EMTableSchema>> SchemaFactorys;

	TSet<FString> ForceIncludeEnums;
};

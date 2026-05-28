#pragma once
#include "EMTableGenerator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSchemaItem
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSchemaItem
{
public:
	EMSchemaItem(TSharedPtr<EMSchemaItem> InParent);

	FString	Path;
	FString	LocalPath;
	FName Name;
	FName DisplayName;

	FString Value;

	const FString GetClassNamePath() const;

	TWeakPtr<EMSchemaItem> Parent;
	TArray<TSharedPtr<EMSchemaItem>> Childs;
	TArray<TSharedPtr<EMSchemaItem>> Arrays;
	TArray<TSharedPtr<EMSchemaItem>> Properties;

	class EMTableSchema* SchemaTemplate;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableSchema
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableSchema
{
public:
	virtual ~EMTableSchema() { };

	virtual EMTableCore::EEMVariabaleType FindStringToTableType(const FString& TypeString, const FString& FormatString) const = 0;

	virtual TSharedPtr<EMSchemaItem> Do(const FString& Path) = 0;
};
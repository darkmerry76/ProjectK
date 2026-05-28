#pragma once

#include "EMSchema.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableSchemaJSON
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableSchemaJSON : public EMTableSchema
{
public:
	EMTableSchemaJSON();

	virtual TSharedPtr<EMSchemaItem> Do(const FString& Path);

private:
	TSharedPtr<EMSchemaItem> Translate(const FString& Filename, TSharedPtr<class FJsonObject> JsonObject);
	void TranslateRecursive(const FString& Key, TSharedPtr<class FJsonValue> JsonValue, TSharedPtr<EMSchemaItem> ParentItem, TSharedPtr<class FJsonObject> RootJsonObject = nullptr);

	virtual EMTableCore::EEMVariabaleType FindStringToTableType(const FString& TypeString, const FString& FormatString) const;

private:
	TMap<FString, EMTableCore::EEMVariabaleType> JsonToTableTypeMapping;
	TMap<FString, EMTableCore::EEMVariabaleType> JsonToTableFormatMapping;
};

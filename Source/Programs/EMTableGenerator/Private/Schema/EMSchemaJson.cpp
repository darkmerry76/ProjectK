#include "EMSchemaJSON.h"
#include "Utility/EMTableUtility.h"

EMSchemaItem::EMSchemaItem(TSharedPtr<EMSchemaItem> InParent) : Parent(InParent)
{
}

const FString EMSchemaItem::GetClassNamePath() const
{
	TArray<FString> OutStrings;

	Path.ParseIntoArray(OutStrings, TEXT("/"));

	if (false == Parent.IsValid())
	{
		return Name.ToString();
	}

	FString Result = OutStrings[0] + TEXT("::") + Name.ToString();
	return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableSchemaJSON
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableSchemaJSON::EMTableSchemaJSON()
{
	JsonToTableTypeMapping.Emplace(TEXT("boolean"), EMTableCore::EEMVariabaleType::Boolean);
	JsonToTableTypeMapping.Emplace(TEXT("integer"), EMTableCore::EEMVariabaleType::Int32);
	JsonToTableTypeMapping.Emplace(TEXT("string"), EMTableCore::EEMVariabaleType::String);
	JsonToTableTypeMapping.Emplace(TEXT("number"), EMTableCore::EEMVariabaleType::Float);

	JsonToTableFormatMapping.Emplace(TEXT("int32"), EMTableCore::EEMVariabaleType::Int32);
	JsonToTableFormatMapping.Emplace(TEXT("int64"), EMTableCore::EEMVariabaleType::Int64);
	JsonToTableFormatMapping.Emplace(TEXT("float"), EMTableCore::EEMVariabaleType::Float);
	JsonToTableFormatMapping.Emplace(TEXT("double"), EMTableCore::EEMVariabaleType::Double);
}

EMTableCore::EEMVariabaleType EMTableSchemaJSON::FindStringToTableType(const FString& TypeString, const FString& FormatString) const
{
	const EMTableCore::EEMVariabaleType* ResultType = JsonToTableTypeMapping.Find(TypeString);
	if (nullptr == ResultType)
	{
		return EMTableCore::EEMVariabaleType::String;
	}
	if(FormatString.Len() < 1)
	{
		return *ResultType;
	}
	const EMTableCore::EEMVariabaleType* ResultFormat = JsonToTableFormatMapping.Find(FormatString);
	if (nullptr == ResultFormat)
	{
		return *ResultType;
	}
	return *ResultFormat;
}

void EMTableSchemaJSON::TranslateRecursive(const FString& Key, TSharedPtr<FJsonValue> JsonValue, TSharedPtr<EMSchemaItem> ParentItem, TSharedPtr<FJsonObject> RootJsonObject)
{
	ParentItem->SchemaTemplate = this;

	if(true == RootJsonObject.IsValid() || JsonValue->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> JsonObject = RootJsonObject.IsValid() == true ? RootJsonObject : JsonValue->AsObject();
		for (auto Value : JsonObject->Values)
		{
			TSharedPtr<EMSchemaItem> newSchemaItem = MakeShareable(new EMSchemaItem(ParentItem));
			newSchemaItem->Path = ParentItem->Path + TEXT("/") + Value.Key;
			newSchemaItem->LocalPath = ParentItem->LocalPath + TEXT("/") + Value.Key;
			newSchemaItem->Name = *Value.Key;

			ParentItem->Childs.Emplace(newSchemaItem);
			TranslateRecursive(Value.Key, Value.Value, newSchemaItem);
		}
	}
	else if (JsonValue->Type == EJson::Array)
	{
		const TArray<TSharedPtr<FJsonValue>> FieldArray = JsonValue->AsArray();
		for(int i=0; i < FieldArray.Num(); ++i)
		{
			TSharedPtr<EMSchemaItem> newSchemaItem = MakeShareable(new EMSchemaItem(ParentItem));
			newSchemaItem->Path = ParentItem->Path + TEXT("/") + FString::FromInt(i);
			newSchemaItem->LocalPath = ParentItem->LocalPath + TEXT("/") + FString::FromInt(i);
			newSchemaItem->Name = *FString::FromInt(i);

			ParentItem->Arrays.Emplace(newSchemaItem);
			TranslateRecursive(TEXT("#"), FieldArray[i], newSchemaItem);
		}
	}
	else
	{
		ParentItem->Value = JsonValue->AsString();
	}
}

TSharedPtr<EMSchemaItem> EMTableSchemaJSON::Translate(const FString& Filename, TSharedPtr<FJsonObject> JsonObject)
{
	TSharedPtr<EMSchemaItem> RootSchemaItem = MakeShareable(new EMSchemaItem(nullptr));
	RootSchemaItem->Path = FString(TEXT("/")) + Filename;
	RootSchemaItem->LocalPath = TEXT("");
	RootSchemaItem->Name = *Filename;

	TranslateRecursive(TEXT(""), nullptr, RootSchemaItem, JsonObject);

	return RootSchemaItem;
}

TSharedPtr<EMSchemaItem> EMTableSchemaJSON::Do(const FString& Path)
{
	TSharedPtr<FJsonObject> JsonObject = EMTableUtility::LoadJsonObject(Path);
	if (nullptr == JsonObject || 0 >= JsonObject->Values.Num())
	{
		return nullptr;
	}
	return Translate(EMTableUtility::GetFullPathToFilenameOrigin(Path), JsonObject);
}
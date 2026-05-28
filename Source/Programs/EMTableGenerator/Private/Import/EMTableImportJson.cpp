#include "EMTableImportJSON.h"
#include "Utility/EMTableUtility.h"
#include "SheetBuilder/EMSheet.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportJSON
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableImportJSON::EMTableImportJSON() : EMTableImport()
{
	static const TCHAR* ReservedJsonMember[] = {
		TEXT("items"),
		TEXT("content_config"),
		TEXT("definitions"),
		TEXT("enums"),
		TEXT("keys"),
		TEXT("properties"),
		TEXT("#schema"),
		TEXT("title"),
		TEXT("additionalproperties")
	};

	int32 MaxReservedJsonMember = sizeof(ReservedJsonMember) / sizeof(ReservedJsonMember[0]);
	for (int32 i = 0; i < MaxReservedJsonMember; ++i)
	{
		ReservedJsonMemberSet.Emplace(ReservedJsonMember[i]);
	}
}

void EMTableImportJSON::Clear()
{
}

bool EMTableImportJSON::IsReservedJsonMember(const TSharedPtr<EMSchemaItem>& PropertyItem) const
{
	if (false == PropertyItem.IsValid())
	{
		return false;
	}
	return ReservedJsonMemberSet.Contains(PropertyItem->Name);
}

bool EMTableImportJSON::SetStructureProperties(TSharedPtr<FEMStructure> ParentJsonStruct, const FName& MemberName, const TSharedPtr<EMSchemaItem>& PropertyItem, const EMSchemaItem* ParentItem, bool IsArray)
{
	TSharedPtr<EMSchemaItem> PropertiesSchemaItem;
	TSharedPtr<EMSchemaItem> EnumSchemaItem;
	TSharedPtr<EMSchemaItem> TypeSchemaItem;

	TSharedPtr<EMSchemaItem> ItemsSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(PropertyItem->Path + "/$ref");
	if (false == ItemsSchemaItem.IsValid())
	{
		ItemsSchemaItem = PropertyItem;
	}
	if (true == ItemsSchemaItem.IsValid())
	{
		PropertiesSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(ItemsSchemaItem->Path + TEXT("/properties"));

		EnumSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(ItemsSchemaItem->Path + "/enum");
		TypeSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(ItemsSchemaItem->Path + "/type");

		TSharedPtr<EMSchemaItem> ChildRefSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(ItemsSchemaItem->Path + "/$ref");
		if(true == ChildRefSchemaItem.IsValid())
		{
			ChildRefSchemaItem->DisplayName = (ItemsSchemaItem->DisplayName == NAME_None) ? ItemsSchemaItem->Name : ItemsSchemaItem->DisplayName;
			SetStructureProperties(ParentJsonStruct, MemberName, ItemsSchemaItem, ItemsSchemaItem.Get(), IsArray);
			return true;
		}
	}

	if (false == PropertiesSchemaItem.IsValid() && false == EnumSchemaItem.IsValid())
	{
		if (true == TypeSchemaItem.IsValid())
		{
			if (TypeSchemaItem->Value == TEXT("object"))
			{
				ParentJsonStruct->AddMemeber(MemberName, MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, TEXT("string"), TEXT(""), IsArray)));
				return true;
			}
		}

		return false;
	}
	if (false == EnumSchemaItem.IsValid())
	{
		const TSharedPtr<FEMStructure> Structure = SetStructuresMembers(PropertiesSchemaItem, true == ItemsSchemaItem.IsValid() ? ItemsSchemaItem.Get() : PropertyItem.Get());
		if (false == Structure.IsValid())
		{
			return false;
		}
		ParentJsonStruct->AddMemeber(MemberName, MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, Structure->GetName(), TEXT(""), IsArray)));
	}
	else
	{
		FString EnumString = FString::Printf(TEXT("%s"), (TypeSchemaItem->Parent.Pin()->DisplayName == NAME_None) ? *TypeSchemaItem->Parent.Pin()->Name.ToString() : *TypeSchemaItem->Parent.Pin()->DisplayName.ToString());
		FString CppEnumString = FString::Printf(TEXT("e%s"), *EnumString);
		if (true == TypeSchemaItem->Path.Contains(TEXT("::enum")) && false == EMTableSchemaMgr::Inst().GetForceIncludeEnums().Contains(EnumString))
		{
			ParentJsonStruct->AddMemeber(MemberName, MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, true == TypeSchemaItem.IsValid() ? *TypeSchemaItem->Value : TEXT("string"), TEXT(""), IsArray)));
		}
		else
		{
			ParentJsonStruct->AddMemeber(MemberName, MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, true == TypeSchemaItem->Parent.IsValid() ? *CppEnumString : TEXT("string"), TEXT(""), IsArray)));
		}
	}
	return true;
}

bool EMTableImportJSON::SetStructuresMember(TSharedPtr<FEMStructure> ParentJsonStruct, const TSharedPtr<EMSchemaItem>& PropertyItem, const EMSchemaItem* ParentItem, bool IsArray)
{
	if (false == PropertyItem.IsValid())
	{
		return false;
	}

	FName ObjectType = TEXT("object");

	FName MemberName = (true == IsArray) ? ParentItem->Name : PropertyItem->Name;

	const TSharedPtr<EMSchemaItem> TypeItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(PropertyItem->Path + "/type");
	const TSharedPtr<EMSchemaItem> FormatItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(PropertyItem->Path + "/format");
	const TSharedPtr<EMSchemaItem> DefaultItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(PropertyItem->Path + "/default");

	FString defaultValue;
	if (DefaultItem.IsValid() == true)
	{
		defaultValue = DefaultItem->Value;
	}
	
	if (true == TypeItem.IsValid())
	{
		ObjectType = *TypeItem->Value;
	}

	TSharedPtr<EMSchemaItem> ContentsConfigKeysFieldSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(PropertyItem->Path + "/content_config/keys/id/fields");
	if (true == ContentsConfigKeysFieldSchemaItem.IsValid())
	{
		TSharedPtr<EMSchemaItem> ContentsConfigKeysUniqueSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(PropertyItem->Path + "/content_config/keys/id/unique");
		bool IsUnique = false;
		if(true == ContentsConfigKeysUniqueSchemaItem.IsValid())
		{
			if (ContentsConfigKeysUniqueSchemaItem->Value == TEXT("true") || ContentsConfigKeysUniqueSchemaItem->Value == TEXT("yes") ||
				ContentsConfigKeysUniqueSchemaItem->Value == TEXT("1"))
			{
				IsUnique = true;
			}
		}
		for (int32 i = 0; i < ContentsConfigKeysFieldSchemaItem->Arrays.Num(); ++i)
		{
			ParentJsonStruct->AddKey(*ContentsConfigKeysFieldSchemaItem->Arrays[i]->Value, IsUnique);
		}
	}

	if (ObjectType == TEXT("array"))
	{
		const TSharedPtr<EMSchemaItem> ItemsSchemaItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(PropertyItem->Path + TEXT("/items"));
		if (false == SetStructureProperties(ParentJsonStruct, MemberName, ItemsSchemaItem, PropertyItem.Get(), true))
		{
			SetStructuresMember(ParentJsonStruct, ItemsSchemaItem, PropertyItem.Get(), true);
		}
	}
	else if (ObjectType == TEXT("object"))
	{
		return SetStructureProperties(ParentJsonStruct, MemberName, PropertyItem, ParentItem, IsArray);
	}
	else if (ObjectType == TEXT("boolean"))
	{
		ParentJsonStruct->AddMemeber(MemberName,
			MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, TEXT("bool"), defaultValue, IsArray)));
	}
	else if (ObjectType == TEXT("integer"))
	{
		if (true == FormatItem.IsValid())
		{
			ParentJsonStruct->AddMemeber(MemberName,
				MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, *FormatItem->Value, defaultValue, IsArray)));
		}
		else
		{
			ParentJsonStruct->AddMemeber(MemberName,
				MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, TEXT("int32"), defaultValue, IsArray)));
		}
	}
	else if (ObjectType == TEXT("number"))
	{
		if(true == FormatItem.IsValid())
		{
			FName FinalType = *FormatItem->Value;
			if (FormatItem->Value == TEXT("integer"))
			{
				FinalType = TEXT("int32");
			}
			ParentJsonStruct->AddMemeber(MemberName,
				MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, FinalType, defaultValue, IsArray)));
		}
		else
		{
			ParentJsonStruct->AddMemeber(MemberName,
				MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, TEXT("float"), defaultValue, IsArray)));
		}
	}
	else if (ObjectType == TEXT("string"))
	{
		ParentJsonStruct->AddMemeber(MemberName,
			MakeShareable(new FEMStructureMember(ParentJsonStruct, MemberName, ObjectType, defaultValue, IsArray)));
	}
	return false;
}

const TArray<TSharedPtr<EMSchemaItem>> EMTableImportJSON::GetMemberItems(const TSharedPtr<EMSchemaItem>& PropertyItem) const
{
	TArray<TSharedPtr<EMSchemaItem>> Result;
	if (false == PropertyItem.IsValid())
	{
		return Result;
	}
	for (int32 i = 0; i < PropertyItem->Childs.Num(); ++i)
	{
		if (true == IsReservedJsonMember(PropertyItem->Childs[i]))
		{
			continue;
		}
		Result.Emplace(PropertyItem->Childs[i]);
	}
	return Result;
}

TSharedPtr<FEMStructure> EMTableImportJSON::SetStructuresMembers(const TSharedPtr<EMSchemaItem>& PropertyItem, const EMSchemaItem* ParentItem, bool IsArray)
{
	TSharedPtr<FEMStructure> ResultStructure = FEMStructureManager::Inst().GetStructure(*ParentItem->GetClassNamePath());
	if (true == ResultStructure.IsValid())
	{
		return ResultStructure;
	}

	TSharedPtr<FEMStructure> newStructure = MakeShareable(new FEMStructure(*ParentItem->GetClassNamePath(), false));
	FEMStructureManager::Inst().AddStructure(*ParentItem->GetClassNamePath(), newStructure);

	TArray<TSharedPtr<EMSchemaItem>> Members = GetMemberItems(PropertyItem);
	for (int32 i = 0; i < Members.Num(); ++i)
	{
		ensure(Members[i]->Name != TEXT("$ref"));
		SetStructuresMember(newStructure, Members[i], ParentItem, IsArray);
	}

	return newStructure;
}

void EMTableImportJSON::CollectStructureContentConfig(TSharedPtr<FEMStructure> ParentJsonStruct, const TSharedPtr<EMSchemaItem>& PropertyItem)
{
}

bool EMTableImportJSON::DoImport(const FString& Path)
{
	double XMLSecondsScope = FPlatformTime::Seconds();

	TSharedPtr<FJsonObject> JsonDataObject = EMTableUtility::LoadJsonObject(Path);
	if (nullptr == JsonDataObject)
	{		
		return false;
	}
	EM_LOG(EMLog, Display, TEXT("JSON[%s] 불러오기 완료[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), FPlatformTime::Seconds() - XMLSecondsScope);

	bool IsArrayType = false;
	for (auto Value : JsonDataObject->Values)
	{
		TSharedPtr<FJsonValue> JsonValue = Value.Value;

		if (JsonValue->Type == EJson::Array)
		{
			FString SheetName = Value.Key;
			const TSharedPtr<EMSchemaItem> SchemaPropertyItem = EMTableSchemaMgr::Inst().SearchLocalSchemaItem(FString::Printf(TEXT("::schema/properties/%s"), *SheetName));
			if (false == SchemaPropertyItem.IsValid())
			{
				EM_LOG(EMLog, Display, TEXT("JSON[%s] Schema Properties[%s] 찾기 실패[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), *SheetName, FPlatformTime::Seconds() - XMLSecondsScope);
				continue;
			}

			const TSharedPtr<EMSchemaItem> SchemaRootPropertiesItem = SchemaPropertyItem->Parent.Pin();
			if (false == SchemaRootPropertiesItem.IsValid())
			{
				EM_LOG(EMLog, Display, TEXT("JSON[%s] Schema Root Properties[%s] 찾기 실패[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), *SheetName, FPlatformTime::Seconds() - XMLSecondsScope);
				continue;
			}
			const TSharedPtr<EMSchemaItem> SchemaRootItem = SchemaRootPropertiesItem->Parent.Pin();
			if (false == SchemaRootItem.IsValid())
			{
				EM_LOG(EMLog, Display, TEXT("JSON[%s] Schema Root[%s] 찾기 실패[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), *SheetName, FPlatformTime::Seconds() - XMLSecondsScope);
				continue;
			}

			TSharedPtr<FEMStructure> RootStructure = MakeShareable(new FEMStructure(*SchemaRootItem->GetClassNamePath(), false, true));
			if (false == SetStructuresMember(RootStructure, SchemaRootItem))
			{
				EM_LOG(EMLog, Display, TEXT("JSON[%s] Schema Create[%s] 실패[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), *SheetName, FPlatformTime::Seconds() - XMLSecondsScope);
				continue;
			}
			FName SchemaName = *SchemaRootItem->Name.ToString().Replace(TEXT("::schema"), TEXT(""));

			AddSheetAnalizer(EMTableUtility::GetFullPathToFilenameExt(Path), Value.Key, MakeShareable(new EMSheetJSON(SchemaName, JsonValue)));
			IsArrayType = true;
		}
	}
	FString JsonFilename = EMTableUtility::GetFullPathToFilename(Path);
	if(false == IsArrayType)
	{
		const TSharedPtr<EMSchemaItem> SchemaRootItem = EMTableSchemaMgr::Inst().SearchLocalSchemaItem(FString::Printf(TEXT("%s::schema"), *JsonFilename));
		if (false == SchemaRootItem.IsValid())
		{
			EM_LOG(EMLog, Display, TEXT("JSON[%s] Schema Root[%s] 찾기 실패[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), *JsonFilename, FPlatformTime::Seconds() - XMLSecondsScope);
			return false;
		}

		TSharedPtr<FEMStructure> RootStructure = MakeShareable(new FEMStructure(*SchemaRootItem->GetClassNamePath(), false, false));
		if (false == SetStructuresMember(RootStructure, SchemaRootItem))
		{
			EM_LOG(EMLog, Display, TEXT("JSON[%s] Schema Create[%s] 실패[%.4f]"), *EMTableUtility::GetFullPathToFilenameExt(Path), *JsonFilename, FPlatformTime::Seconds() - XMLSecondsScope);
			return false;
		}
		TSharedPtr<FJsonValueObject> newJsonValue = MakeShareable(new FJsonValueObject(JsonDataObject));
		AddSheetAnalizer(EMTableUtility::GetFullPathToFilenameExt(Path), JsonFilename, MakeShareable(new EMSheetJSON(*JsonFilename, newJsonValue)));
	}
	return true;
}

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

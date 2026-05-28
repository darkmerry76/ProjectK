#include "EMSchemaMgr.h"
#include "Config/EMTableConfig.h"
#include "Utility/EMTableUtility.h"
#include "EMSchemaJSON.h"
#include "Parser/EMCppParser.h"
#include "HAL/FileManagerGeneric.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

EMTableSchemaMgr& EMTableSchemaMgr::Inst()
{
	static EMTableSchemaMgr Mgr;
	return Mgr;
}

EMTableSchemaMgr::EMTableSchemaMgr()
{
	SchemaFactorys.Emplace(MakeShareable(new EMTableSchemaJSON()));
}

TSet<FString>& EMTableSchemaMgr::GetForceIncludeEnums()
{
	return ForceIncludeEnums;
}

const TSharedPtr<EMSchemaItem>	EMTableSchemaMgr::GetRefPathItem(const FString& Path)
{
	const TSharedPtr<EMSchemaItem> SchemaPathItem = EMTableSchemaMgr::Inst().GetLocalSchemaItem(Path + TEXT("/$ref"));
	if (nullptr == SchemaPathItem)
	{
		return EMTableSchemaMgr::Inst().GetLocalSchemaItem(Path);;
	}
	FString FinalRefPath = Path;

	TArray<FString> OutRefPaths;
	SchemaPathItem->Value.ParseIntoArray(OutRefPaths, TEXT("#/"));
	if (OutRefPaths.Num() > 1)
	{
		FinalRefPath = TEXT("#/") + OutRefPaths[1];
	}
	else if (OutRefPaths.Num() == 1)
	{
		FinalRefPath = TEXT("#/") + OutRefPaths[0];
	}

	return EMTableSchemaMgr::Inst().GetLocalSchemaItem(FinalRefPath);
}

const TSharedPtr<EMSchemaItem>	EMTableSchemaMgr::GetSchemaItem(const FString& Path) const
{
	const TSharedPtr<EMSchemaItem>* Result = Items.Find(Path);
	if (nullptr == Result)
	{
		return nullptr;
	}

	return *Result;
}

const TSharedPtr<EMSchemaItem>	EMTableSchemaMgr::GetLocalSchemaItem(const FString& Path) const
{
	const TSharedPtr<EMSchemaItem>* Result = LocalItems.Find(Path);
	if (nullptr == Result)
	{
		return nullptr;
	}

	return *Result;
}

const TSharedPtr<EMSchemaItem>	EMTableSchemaMgr::SearchLocalSchemaItem(const FString& Path) const
{
	for (auto ItemItr : LocalItems)
	{
		if (true == ItemItr.Key.Contains(Path))
		{
			return ItemItr.Value;
		}
	}
	return nullptr;
}

const FString EMTableSchemaMgr::GetLocalPathToFilePath(const FString& LocalPath) const
{
	const FString* Result = ItemLocalPathToFilePath.Find(LocalPath);
	if(nullptr == Result)
	{
		return TEXT("");
	}
	return *Result;
}

void EMTableSchemaMgr::AssignItems(const FString& Path, TSharedPtr<EMSchemaItem> Item, TSharedPtr<EMSchemaItem> Parent)
{	
	Items.Emplace(Path + TEXT("#") + Item->LocalPath, Item);
	ensure(false == LocalItems.Contains(Item->Path));
	LocalItems.Emplace(Item->Path, Item);
	ItemLocalPathToFilePath.Emplace(Item->Path, Path);

	if (Parent != nullptr && Parent->Name == TEXT("definitions"))
	{
		DefinitionItems.Emplace(Item->Path, Item);
	}
	for (int i = 0; i < Item->Childs.Num(); ++i)
	{
		AssignItems(Path, Item->Childs[i], Item);
	}
}

bool EMTableSchemaMgr::ImportFile(const FString& Path)
{
	for(int i=0; i < SchemaFactorys.Num(); ++i)
	{
		TSharedPtr<EMSchemaItem> ResultSchemaDatas = SchemaFactorys[i]->Do(Path);
		if (nullptr != ResultSchemaDatas)
		{
			AssignItems(Path, ResultSchemaDatas);
			return true;
		}
	}
	return false;
}

void EMTableSchemaMgr::Initalize()
{
	TArray<FString> OutPathFiles;
	TArray<FString> IgnorePathFiles;

	for (int32 i = 0; i < (int)EMTableConfig::Get().GetSchemaSection().IgnoreFiles.Num(); ++i)
	{
		TArray<FString> TempPathFiles;

		FFileManagerGeneric::Get().FindFilesRecursive(TempPathFiles,
			*EMTableConfig::Get().GetSchemaSection().Path, *EMTableConfig::Get().GetSchemaSection().IgnoreFiles[i], true, false, false);

		for (int32 j = 0; j < TempPathFiles.Num(); ++j)
		{
			TempPathFiles[j] = EMTableUtility::GetFullPathToFullPathnameOrganize(TempPathFiles[j]);
		}

		IgnorePathFiles.Append(TempPathFiles);
	}

	for (int32 i = 0; i < (int)EMTableConfig::Get().GetSchemaSection().Files.Num(); ++i)
	{
		TArray<FString> TempPathFiles;

		FString AddDirectory = EMTableUtility::GetFullPathToPathname(EMTableConfig::Get().GetSchemaSection().Files[i]);
		AddDirectory = false == AddDirectory.IsEmpty() ? FString(TEXT("/")) + AddDirectory : "";

		FFileManagerGeneric::Get().FindFilesRecursive(TempPathFiles,
			*(EMTableConfig::Get().GetSchemaSection().Path + AddDirectory), *EMTableConfig::Get().GetSchemaSection().Files[i], true, false, false);

		for (int32 j = 0; j < TempPathFiles.Num(); ++j)
		{
			TempPathFiles[j] = EMTableUtility::GetFullPathToFullPathnameOrganize(TempPathFiles[j]);
		}

		for (int32 j = 0; j < TempPathFiles.Num(); ++j)
		{
			if (INDEX_NONE == IgnorePathFiles.Find(TempPathFiles[j]))
			{
				OutPathFiles.Emplace(TempPathFiles[j]);
			}
		}
	}

	EM_LOG(EMLog, Display, TEXT("============ 스키마 파일[%d] 불러오기 ============"), OutPathFiles.Num());

	FString ExcelFilePath = EMTableConfig::Get().GetExcelFileSection().Path + TEXT("/");
	for (int32 i = 0; i < OutPathFiles.Num(); i++)
	{
		ImportFile(OutPathFiles[i]);
	}
	PostInitalize();
}

void EMTableSchemaMgr::PostInitalize()
{
	for (auto VariableItemItr : DefinitionItems)
	{
		TSharedPtr<EMSchemaItem> VariableItem = VariableItemItr.Value;
		TSharedPtr<EMSchemaItem> SchemaTypeItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(FString::Printf(TEXT("%s/$ref"), *VariableItem->Path));
		if (true == SchemaTypeItem.IsValid())
		{
			VariableItem = SchemaTypeItem;
			SchemaTypeItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(FString::Printf(TEXT("%s/type"), *VariableItem->Path));
		}
		else
		{
			SchemaTypeItem = EMTableSchemaMgr::Inst().GetSchemaValueToPath(FString::Printf(TEXT("%s/type"), *VariableItem->Path));
		}
		if (true == SchemaTypeItem.IsValid())
		{
			if (SchemaTypeItem->Value == TEXT("string"))
			{
				if (const TSharedPtr<EMSchemaItem> SchemaEnumItem = EMTableSchemaMgr::Inst().GetLocalSchemaItem(FString::Printf(TEXT("%s/enum"), *VariableItem->Path)))
				{
					if(0 < SchemaEnumItem->Arrays.Num() && (false == SchemaEnumItem->Path.Contains(TEXT("::enum")) || true == EMTableSchemaMgr::Inst().GetForceIncludeEnums().Contains(VariableItemItr.Value->Name.ToString())))
					{
						const TSharedPtr<EMCppNameSpaceNode> RootNameSpace = EMCppParser::Get().GetRootNameSpace();

						FString EnumName = FString::Printf(TEXT("e%s"), *VariableItemItr.Value->Name.ToString());
						TSharedPtr<EMCppEnumNode> NewEnum = MakeShareable(new EMCppEnumNode(*EnumName, RootNameSpace.Get()));

						RootNameSpace->Enums.Emplace(*EnumName, NewEnum);
						EMCppParser::Get().GetMutableEnums().Emplace(*EnumName, NewEnum);

						NewEnum->OwnerType = eEnumOnwerType::eEnumOnwerType_Excel;

						if(SchemaEnumItem->Arrays.Num() < 256)
						{
							NewEnum->VariableType = EMTableCore::EEMEnumVariableType::Uint8;
						}
						else if (SchemaEnumItem->Arrays.Num() < 65536)
						{
							NewEnum->VariableType = EMTableCore::EEMEnumVariableType::Uint16;
						}
						else if (SchemaEnumItem->Arrays.Num() < 4294967296)
						{
							NewEnum->VariableType = EMTableCore::EEMEnumVariableType::Uint32;
						}
						else
						{
							NewEnum->VariableType = EMTableCore::EEMEnumVariableType::Uint64;
						}

						NewEnum->IsClass = true;
						NewEnum->Description = TEXT("");

						for(int i=0; i < SchemaEnumItem->Arrays.Num(); ++i)
						{
							TSharedPtr<EMCppEnumMemberNode> EnumValuePtr = MakeShareable(new EMCppEnumMemberNode(*SchemaEnumItem->Arrays[i]->Value, NewEnum.Get()));

							EnumValuePtr->Value = i;
							EnumValuePtr->DisplayName = SchemaEnumItem->Arrays[i]->Value;
							EnumValuePtr->Description = SchemaEnumItem->Arrays[i]->Value;

							NewEnum->MembersMap.Emplace(EnumValuePtr->Name, EnumValuePtr);
							NewEnum->Members.Emplace(EnumValuePtr);
						}
					}
				}
			}
		}
	}

// 	for (auto it = LocalItems.CreateIterator(); it; ++it)
// 	{
// 		if (it->Value->Name == TEXT("$ref"))
// 		{			
// 			RefCollection(it->Value->Parent.Pin(), GetSchemaValueToPath(it->Value->Path));
// 			it.RemoveCurrent();
// 		}
// 	}
// 	return;
 }

void EMTableSchemaMgr::RefCollection(TSharedPtr<EMSchemaItem> ParentItem, TSharedPtr<EMSchemaItem> RefChild)
{
	if (false == ParentItem.IsValid() || false == RefChild.IsValid())
	{
		return;
	}

	TSharedPtr<EMSchemaItem> newSchemaItem = MakeShareable(new EMSchemaItem(ParentItem));
	newSchemaItem->Path = ParentItem->Path + TEXT("/") + RefChild->Name.ToString();
	newSchemaItem->LocalPath = ParentItem->Path + TEXT("/") + RefChild->Name.ToString();
	newSchemaItem->Name = RefChild->Name;

	ParentItem->Childs.Emplace(newSchemaItem);

	for (int32 i = 0; i < RefChild->Childs.Num(); ++i)
	{
		RefCollection(newSchemaItem, RefChild->Childs[i]);
	}
}

const TSharedPtr<EMSchemaItem> EMTableSchemaMgr::GetSchemaValueToPath(const FString& Path) const
{
	const TSharedPtr<EMSchemaItem> ValueItem = GetLocalSchemaItem(Path);
	if (false == ValueItem.IsValid())
	{
		return nullptr;
	}

	TArray<FString> OutArray;
	ValueItem->Value.ParseIntoArray(OutArray, TEXT("#"), false);

	if (OutArray.Num() < 2)
	{
		return GetLocalSchemaItem(Path);
	}

	if (true == OutArray[0].IsEmpty())
	{
		TArray<FString> OutArray2;
		Path.ParseIntoArray(OutArray2, TEXT("::schema"), false);

		FString FinalRefPath = FString::Printf(TEXT("%s::schema%s"), *OutArray2[0], *OutArray[1]);
		OutArray[0] = GetLocalPathToFilePath(FinalRefPath);
	}

	TCHAR SavePath[MAX_PATH];
	_tgetcwd(SavePath, MAX_PATH);

	_tchdir(*EMTableConfig::Get().GetSchemaSection().Path);

	FString SchemaSectionPath = EMTableUtility::GetFullPathToPathname(OutArray[0]);
	_tchdir(*SchemaSectionPath);

	TCHAR ImportedPath[MAX_PATH];
	_tgetcwd(ImportedPath, MAX_PATH);

	_tchdir(SavePath);

	FString FinalImportedPath = FString(ImportedPath) + TEXT("/") + EMTableUtility::GetFullPathToFilenameExt(OutArray[0]) + TEXT("#") + OutArray[1];
	FinalImportedPath = FinalImportedPath.Replace(TEXT("\\"), TEXT("/"));

	const TSharedPtr<EMSchemaItem> ResultItem = GetSchemaItem(FinalImportedPath);
	return ResultItem;
}

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

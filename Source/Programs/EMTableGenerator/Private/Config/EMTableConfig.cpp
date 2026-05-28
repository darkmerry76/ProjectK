#include "EMTableConfig.h"

#include "Misc/PackageName.h"
#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"
#include "Schema/EMSchemaMgr.h"
#include "Utility/EMTableUtility.h"

FString GWorkPath;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EMTableConfig::EMTableConfig()
{

}

EMTableConfig::~EMTableConfig()
{

}

bool EMTableConfig::Init(const FString& Filename)
{
	if (false == FFileManagerGeneric::Get().FileExists(*Filename))
	{
		return false;
	}
	
	ConfigFile.Combine(Filename);

	FString OnlyWorkPath = EMTableUtility::GetOnlyPathToPathname(GWorkPath);

	FPackageName::RegisterMountPoint(OnlyWorkPath, GWorkPath);

	// GenericSection
	TArray<FString> DisableExceptionMsgs;
	ConfigFile.GetArray(TEXT("GenericSection"), TEXT("DisableExceptionMsgs"), DisableExceptionMsgs);
	for (int32 i = 0; i < DisableExceptionMsgs.Num(); i++)
	{
		int32 ExceptionNumber = FCString::Atoi(*DisableExceptionMsgs[i]);
		if (0 != ExceptionNumber)
		{
			GenericSection.DisableExceptionMsgs.Add(ExceptionNumber);
		}
	}

	TArray<FString> WarnningToErrors;
	ConfigFile.GetArray(TEXT("GenericSection"), TEXT("WarnningToErrors"), WarnningToErrors);
	for (int32 i = 0; i < WarnningToErrors.Num(); i++)
	{
		int32 ExceptionNumber = FCString::Atoi(*WarnningToErrors[i]);
		if (0 != ExceptionNumber)
		{
			GenericSection.WarnningToErrors.Add(ExceptionNumber);
		}
	}
	ConfigFile.GetArray(TEXT("GenericSection"), TEXT("ArrayDelims"), GenericSection.ArrayDelims);
	TArray<FString> LocalStringToClasses;
	ConfigFile.GetArray(TEXT("GenericSection") , TEXT("StringToClass"), LocalStringToClasses);
	for (int32 i = 0; i < LocalStringToClasses.Num(); i++)
	{
		TArray<FString> ParseStrings;
		LocalStringToClasses[i].ParseIntoArray(ParseStrings, TEXT(","));
		if(ParseStrings.Num() == 2)
		{
			GenericSection.StringToClasses.Add(TPair<FString, FString>(ParseStrings[0], ParseStrings[1]));
		}
		else if (ParseStrings.Num() == 1)
		{
			GenericSection.StringToClasses.Add(TPair<FString, FString>(ParseStrings[0], ParseStrings[0]));
		}
		else
		{
			checkf(0, TEXT("StringToClass Split Error!!!!"));
		}
	}

	TArray<FString> LocalStringToObjects;
	ConfigFile.GetArray(TEXT("GenericSection"), TEXT("StringToObject"), LocalStringToObjects);
	for (int32 i = 0; i < LocalStringToObjects.Num(); i++)
	{
		TArray<FString> ParseStrings;
		LocalStringToObjects[i].ParseIntoArray(ParseStrings, TEXT(","));
		if (ParseStrings.Num() == 2)
		{
			GenericSection.StringToObjects.Add(TPair<FString, FString>(ParseStrings[0], ParseStrings[1]));
		}
		else if (ParseStrings.Num() == 1)
		{
			GenericSection.StringToObjects.Add(TPair<FString, FString>(ParseStrings[0], ParseStrings[0]));
		}
		else
		{
			checkf(0, TEXT("StringToObject Split Error!!!!"));
		}
	}

	ConfigFile.GetInt(TEXT("GenericSection"), TEXT("ShutdownErrorCount"), GenericSection.ShutdownErrorCount);
	ConfigFile.GetString(TEXT("GenericSection"), TEXT("PrefixSheetName"), GenericSection.PrefixSheetName);
	ConfigFile.GetString(TEXT("GenericSection"), TEXT("MetaDataDelims"), GenericSection.MetaDataDelim);
	ConfigFile.GetString(TEXT("GenericSection"), TEXT("PrefixMetaSheet"), GenericSection.PrefixMetaSheet);
	ConfigFile.GetBool(TEXT("GenericSection"), TEXT("CheckKeyOverlap"), GenericSection.CheckKeyOverlap);
	ConfigFile.GetArray(TEXT("GenericSection"), TEXT("KeyDelims"), GenericSection.KeyDelims);
	if (GenericSection.KeyDelims.Num() < 1)
	{
		GenericSection.KeyDelims.Add(TEXT("("));
		GenericSection.KeyDelims.Add(TEXT(")"));
	}

	FString KeyEmptyAuto;
	ConfigFile.GetString(TEXT("GenericSection"), TEXT("KeyEmptyAuto"), KeyEmptyAuto);
	if (true == KeyEmptyAuto.Equals(TEXT("true"), ESearchCase::IgnoreCase))
	{
		GenericSection.KeyEmptyAuto = true;
	}
	ConfigFile.GetString(TEXT("GenericSection"), TEXT("MultiKeyDelim"), GenericSection.MultiKeyDelim);
	if (GenericSection.MultiKeyDelim.Len() < 1)
	{
		GenericSection.MultiKeyDelim = TEXT("$");
	}

	// ExcelFileSection
	ConfigFile.GetArray(TEXT("ExcelFileSection"), TEXT("Files"), ExcelFileSection.Files);
	ConfigFile.GetArray(TEXT("ExcelFileSection"), TEXT("IgnoreFiles"), ExcelFileSection.IgnoreFiles);
	ConfigFile.GetString(TEXT("ExcelFileSection"), TEXT("Path"), ExcelFileSection.Path);
	ExcelFileSection.Path = FString::Printf(TEXT("%s%s"), *GWorkPath, *ExcelFileSection.Path);

	// SchemaSection
	ConfigFile.GetArray(TEXT("SchemaSection"), TEXT("Files"), SchemaSection.Files);
	ConfigFile.GetArray(TEXT("SchemaSection"), TEXT("IgnoreFiles"), SchemaSection.IgnoreFiles);
	ConfigFile.GetString(TEXT("SchemaSection"), TEXT("Path"), SchemaSection.Path);
	ConfigFile.GetArray(TEXT("SchemaSection"), TEXT("ForceIncludeEnum"), SchemaSection.ForceIncludeEnum);

	for (int32 i = 0; i < SchemaSection.ForceIncludeEnum.Num(); ++i)
	{
		EMTableSchemaMgr::Inst().GetForceIncludeEnums().Emplace(SchemaSection.ForceIncludeEnum[i]);
	}
	SchemaSection.Path = FString::Printf(TEXT("%s%s"), *GWorkPath, *SchemaSection.Path);

	// BinaryExportSection
	ConfigFile.GetBool(TEXT("BinaryExportSection"), TEXT("Export"), BinaryExportSection.IsExport);
	ConfigFile.GetBool(TEXT("BinaryExportSection"), TEXT("Merge"), BinaryExportSection.IsMerge);
	ConfigFile.GetBool(TEXT("BinaryExportSection"), TEXT("Compress"), BinaryExportSection.IsCompress);
	ConfigFile.GetInt(TEXT("BinaryExportSection"), TEXT("PartitionCell"), BinaryExportSection.PartitionCell);
	ConfigFile.GetString(TEXT("BinaryExportSection"), TEXT("Path"), BinaryExportSection.Path);
	BinaryExportSection.Path = FString::Printf(TEXT("%s%s"), *GWorkPath, *BinaryExportSection.Path);

	ConfigFile.GetString(TEXT("BinaryExportSection"), TEXT("Extname"), BinaryExportSection.Extname);
	ConfigFile.GetString(TEXT("BinaryExportSection"), TEXT("MergeFilename"), BinaryExportSection.MergeFilename);

	// UAssetExportSection
	ConfigFile.GetBool(TEXT("UAssetExportSection"), TEXT("Export"), UAssetExportSection.IsExport);
	ConfigFile.GetBool(TEXT("UAssetExportSection"), TEXT("Merge"), UAssetExportSection.IsMerge);
	ConfigFile.GetString(TEXT("UAssetExportSection"), TEXT("MergeFilename"), UAssetExportSection.MergeFilename);
	ConfigFile.GetString(TEXT("UAssetExportSection"), TEXT("ContentsPath"), UAssetExportSection.ContentsPath);
	ConfigFile.GetString(TEXT("UAssetExportSection"), TEXT("Path"), UAssetExportSection.Path);

	UAssetExportSection.Path = UAssetExportSection.Path.Replace(TEXT("./"), TEXT(""));
	ConfigFile.GetString(TEXT("UAssetExportSection"), TEXT("Project"), UAssetExportSection.Project);

	// JsonExportSection
	ConfigFile.GetBool(TEXT("JsonExportSection"), TEXT("Export"), JsonExportSection.IsExport);
	ConfigFile.GetBool(TEXT("JsonExportSection"), TEXT("Merge"), JsonExportSection.IsMerge);
	ConfigFile.GetString(TEXT("JsonExportSection"), TEXT("Path"), JsonExportSection.Path);
	JsonExportSection.Path = FString::Printf(TEXT("%s%s"), *GWorkPath, *JsonExportSection.Path);
	ConfigFile.GetString(TEXT("JsonExportSection"), TEXT("Extname"), JsonExportSection.Extname);
	ConfigFile.GetString(TEXT("JsonExportSection"), TEXT("MergeFilename"), JsonExportSection.MergeFilename);

	// CodeSection
	ConfigFile.GetBool(TEXT("CodeSection"), TEXT("Export"), CodeSection.IsExport);
	ConfigFile.GetBool(TEXT("CodeSection"), TEXT("Merge"), CodeSection.IsMerge);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("ClassName"), CodeSection.ClassName);
	ConfigFile.GetArray(TEXT("CodeSection"), TEXT("AddInclude"), CodeSection.AddInclude);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("TableHelperClassName"), CodeSection.TableHelperClassName);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("Path"), CodeSection.Path);
	CodeSection.Path = FString::Printf(TEXT("%s%s"), *GWorkPath, *CodeSection.Path);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("MergeFilename"), CodeSection.MergeFilename);

	TArray<FString> LocalInValidVariableName;
	ConfigFile.GetArray(TEXT("CodeSection"), TEXT("InValidVariableName"), LocalInValidVariableName);
	for (int32 i = 0; i < LocalInValidVariableName.Num(); i++)
	{
		TArray<FString> ParseStrings;
		LocalInValidVariableName[i].ParseIntoArray(ParseStrings, TEXT(","));
		CodeSection.InValidVariableName += ParseStrings;
	}

	ConfigFile.GetString(TEXT("CodeSection"), TEXT("PrefixVariableName"), CodeSection.PrefixVariableName);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("SuffixVariableName"), CodeSection.SuffixVariableName);

	ConfigFile.GetArray(TEXT("CodeSection"), TEXT("Namespace"), CodeSection.Namespace);

	ConfigFile.GetString(TEXT("CodeSection"), TEXT("EnumExcelFile"), CodeSection.EnumExcelFile);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("EnumSheetName"), CodeSection.EnumSheetName);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("EnumDelim"), CodeSection.EnumDelim);

	CodeSection.EnumExcelFile = GWorkPath + CodeSection.EnumExcelFile;

	TArray<FString> LocalEnumCppPath;
	ConfigFile.GetArray(TEXT("CodeSection"), TEXT("EnumCppPath"), LocalEnumCppPath);
	for (int32 i = 0; i < LocalEnumCppPath.Num(); i++)
	{
		CodeSection.EnumCppPath.Add(*FString::Printf(TEXT("%s%s"), *GWorkPath, *LocalEnumCppPath[i]));
	}

	ConfigFile.GetString(TEXT("CodeSection"), TEXT("EnumPath"), CodeSection.EnumPath);
	CodeSection.EnumPath = FString::Printf(TEXT("%s%s"), *GWorkPath, *CodeSection.EnumPath);

	ConfigFile.GetString(TEXT("CodeSection"), TEXT("EnumMergeFilename"), CodeSection.EnumMergeFilename);
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("StructureMergeFilename"), CodeSection.StructureMergeFilename);
	FString StructureString;
	ConfigFile.GetString(TEXT("CodeSection"), TEXT("Structure"), StructureString);

	if (true == StructureString.Equals(TEXT("Inheritance"), ESearchCase::IgnoreCase))
	{
		CodeSection.StructureType = EStructureType::EStructureType_Inheritance;
	}
	else if (true == StructureString.Equals(TEXT("Base"), ESearchCase::IgnoreCase))
	{
		CodeSection.StructureType = EStructureType::EStructureType_Base;
	}
	else if (true == StructureString.Equals(TEXT("Merge"), ESearchCase::IgnoreCase))
	{
		CodeSection.StructureType = EStructureType::EStructureType_Merge;
	}

	return true;
}
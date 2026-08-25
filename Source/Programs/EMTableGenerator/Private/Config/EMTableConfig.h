#pragma once

#include "EMTableGenerator.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EStructureType
////////////////////////////////////////////////////////////////////////////////////////////////////
enum EStructureType
{
	EStructureType_Base,
	EStructureType_Merge,
	EStructureType_Inheritance,
	EStructureType_Max,
};

extern FString GWorkPath;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMStringParse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMGenericSection
{
	EMGenericSection() : KeyEmptyAuto(false), ShutdownErrorCount(0), PrefixSheetName(TEXT("EM_")), CheckKeyOverlap(false) { };
	TSet<int32>	DisableExceptionMsgs;
	TSet<int32>	WarnningToErrors;
	TArray<FString> ArrayDelims;
	TArray<FString> KeyDelims;
	bool KeyEmptyAuto;
	FString MultiKeyDelim;
	FString MetaDataDelim;
	FString PrefixMetaSheet;
	int32 ShutdownErrorCount;
	FString PrefixSheetName;
	bool CheckKeyOverlap;
	TArray<TPair<FString,FString>> StringToClasses;
	TArray<TPair<FString,FString>> StringToObjects;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMExcelFileSection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMExcelFileSection
{
	FString Path;
	TArray<FString> Files;
	TArray<FString> IgnoreFiles;
	TMap<FString,FString> SheetRenames;

	FString GetFinalFilesname(int32 Index) const {
		return FString::Printf(TEXT("%s/%s"), *Path, *Files[Index]);
	};
	FString GetFinalIgnoreFilesname(int32 Index) const {
		return FString::Printf(TEXT("%s/%s"), *Path, *IgnoreFiles[Index]);
	};

	FString GetFinalSheetName(const FString& originName) const
	{
		const FString* existName = SheetRenames.Find(originName);
		if (!existName || (*existName).IsEmpty())
		{
			return originName;
		}
		return *existName;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSchemaSection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMSchemaSection
{
	FString Path;
	TArray<FString> Files;
	TArray<FString> IgnoreFiles;
	TArray<FString> ForceIncludeEnum;
	
	const FString GetFinalFilesname(int32 Index) const {
		return FString::Printf(TEXT("%s/%s"), *Path, *Files[Index]);
	};
	const FString GetFinalIgnoreFilesname(int32 Index) const {
		return FString::Printf(TEXT("%s/%s"), *Path, *IgnoreFiles[Index]);
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMExportSection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMExportSection
{
	EMExportSection() : IsExport(false), IsMerge(false) { };
	bool		IsExport;
	bool		IsMerge;
	FString		Path;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMBinaryExportSection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMBinaryExportSection : public EMExportSection
{
	EMBinaryExportSection() : IsCompress(false), PartitionCell(500000000) { };
	FString		Extname;
	FString		MergeFilename;
	bool		IsCompress;
	int			PartitionCell;

	const FString GetFinalMergeFilesname() const {
		return FString::Printf(TEXT("%s/%s.%s"), *Path, *MergeFilename, *Extname);
	};
	const FString GetFinalSingleFilename(const TCHAR* Filename) const {
		return FString::Printf(TEXT("%s/%s.%s"), *Path, Filename, *Extname);
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMJsonExportSection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMJsonExportSection : public EMExportSection
{
	EMJsonExportSection(){ };
	FString		Extname;
	FString		MergeFilename;

	const FString GetFinalMergeFilesname() const {
		return FString::Printf(TEXT("%s/%s.%s"), *Path, *MergeFilename, *Extname);
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMUAssetExportSection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMUAssetExportSection  : public EMExportSection
{
	EMUAssetExportSection() { };

	FString		Extname;
	FString		MergeFilename;
	FString		ContentsPath;
	FString		Project;

	const FString GetFinalMergeFilesname() const {
		return FString::Printf(TEXT("%s/%s.%s"), *Path, *MergeFilename, *Extname);
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMCodeSection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EMCodeSection : public EMExportSection
{
	EMCodeSection() : StructureType(EStructureType::EStructureType_Base){ };

	FString		ClassName;
	TArray<FString> AddInclude;
	FString		TableHelperClassName;
	FString		Path;
	FString		MergeFilename;
	TArray<FString> InValidVariableName;
	FString		PrefixVariableName;
	FString		SuffixVariableName;
	TArray<FString>	Namespace;
	FString		EnumExcelFile;
	TArray<FString>	EnumCppPath;
	FString		EnumSheetName;
	FString		EnumDelim;
	FString		EnumPath;
	FString		EnumMergeFilename;
	FString		StructureMergeFilename;
	EStructureType StructureType;
	
	const FString GetFinalBPHeaderFilename() const {
		return FString::Printf(TEXT("%s/%s.h"), *Path, *ClassName);
	};
	const FString GetFinalMergeHeaderFilename() const {
		return FString::Printf(TEXT("%s/%s.h"), *Path, *MergeFilename);
	};
	const FString GetFinalMergeSourceFilename() const {
		return FString::Printf(TEXT("%s/%s.cpp"), *Path, *MergeFilename);
	};
	const FString GetFinalStructureMergeFilenam() const {
		return FString::Printf(TEXT("%s/%s"), *Path, *StructureMergeFilename);
	};
	const FString GetFinalMergeEnumFilename() const {
		return FString::Printf(TEXT("%s/%s.h"), *EnumPath, *EnumMergeFilename);
	};

	const FString GetFinalHeaderFilename(const TCHAR* Filename) const {
		return FString::Printf(TEXT("%s/%s.h"), *Path, Filename);
	};
	const FString GetFinalSourceFilename(const TCHAR* Filename) const {
		return FString::Printf(TEXT("%s/%s.cpp"), *Path, Filename);
	};

	const FString GetFinalEnumHeaderFilename(const TCHAR* Filename) const {
		return FString::Printf(TEXT("%s/%s.h"), *EnumPath, Filename);
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableConfig
{
public:
	EMTableConfig();
	virtual ~EMTableConfig();

	bool Init(const FString& Filename);

	static EMTableConfig& Get()
	{
		static EMTableConfig Instance;
		return Instance;
	}

public:
	const EMGenericSection&				GetGenericSection() const { return GenericSection; };
	const EMSchemaSection&				GetSchemaSection() const { return SchemaSection; };
	const EMExcelFileSection&			GetExcelFileSection() const { return ExcelFileSection; };
	const EMBinaryExportSection&		GetBinaryExportSection() const { return BinaryExportSection; }
	const EMJsonExportSection&			GetJsonExportSection() const { return JsonExportSection; };
	const EMUAssetExportSection&		GetUAssetExportSection() const { return UAssetExportSection; };
	const EMCodeSection&				GetCodeSection() const { return CodeSection; };

protected:
	const FString ArrangePrivateProfileString(const TCHAR* Section, const TCHAR* Key) const;
	bool ArrangePrivateProfileBool(const TCHAR* Section, const TCHAR* Key) const;
	int32 ArrangePrivateProfileInt(const TCHAR* Section, const TCHAR* Key) const;

protected:
	EMGenericSection		GenericSection;
	EMSchemaSection		SchemaSection;
	EMExcelFileSection		ExcelFileSection;
	EMBinaryExportSection	BinaryExportSection;
	EMJsonExportSection	JsonExportSection;
	EMUAssetExportSection	UAssetExportSection;
	EMCodeSection			CodeSection;

	FConfigFile				ConfigFile;
};

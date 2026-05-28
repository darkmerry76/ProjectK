#pragma  once

#include "EMTableGenerator.h"
#include "libxl.h"
#include "pugixml.hpp"
#include "DataTable/EMDataTable.h"
#include "Export/EMTableExport.h"
#include "Import/EMTableImport.h"
#include "SheetBuilder/EMSheetBuilder.h"
#include "SheetBuilder/EMSheetBuilderTransform.h"
#include "Schema/EMSchemaMgr.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderMgr
// SheetBuilder및 전체 Process를 관리
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheetBuilderMgr
{
public:
	EMSheetBuilderMgr();
	virtual ~EMSheetBuilderMgr();

	// 싱글턴
	static EMSheetBuilderMgr& Get()
	{
		static EMSheetBuilderMgr Instance;
		return Instance;
	}

	// 테이블제네레이터의 전체 흐름
	bool Process();

	// 모든 멤버 Clear
	void Clear();

	// SheetBuilder를 계층구조에 맞게 생성한다.
	TSharedPtr<EMSheetBuilder> AddSheetBuilder(const FString& SheetName);

	// Sheet이름으로 EMSheetBuilder를 하나 찾아준다. mutable형태
	TSharedPtr<EMSheetBuilder> FindMutableSheetBuilder(const FString& SheetName) const;

	// Sheet이름으로 EMSheetBuilder를 하나 찾아준다. const형태
	const TSharedPtr<EMSheetBuilder> FindSheetBuilder(const FString& SheetName) const;

	// SheetBuilderTransform에 const로 접근가능
	const EMSheetBuilderTransform& GetSheetBuilderTransform() const { return SheetBuilderTransform; };

	// 모든 SheetBuilder를 const로 접근가능
	const TArray<TSharedPtr<EMSheetBuilder>>& GetSheetBuilders() const { return SheetBuilders; };

	// 최상위 Sheet
	const TSharedPtr<EMSheetBuilder> GetRootSheet() const{ return RootSheet; };

protected:
	// c++이나 midl파일의 enum을 파싱한다.
	bool ParsingEnums();

	// 하나의 엑셀파일을 임포트한다.
	bool ImportExcelFile(const FString& ExcelFilename);

	// Config.ini에 설정한 모든 엑셀파일을 임포트 한다.
	bool ImportExcelFiles();

	// 사용가능한 Importer를 등록한다.
	void RegisterImporter();

	// 사용가능한 Exporter를 등록한다.
	void RegisterExporter();

	// 변수중에 링크가 걸려있으면 링크된 Sheet의 키타입으로 변환시켜준다.
	bool ExchangeLinkType(EMTableCore::FEMVariable& DestVariable);

	// 전체 Sheet를 돌면서 EMSheetBuilderMgr::ExchangeLinkType()를 호출
	void ExchangeLinkTypes();

protected:
	// Sheet이름이 유효한지 검사한다.
	bool IsVaildSheetName(const FString& xlSheetName) const;

	// Sheet이름을 계층구조에 맞게 분리한다.
	bool SeparationHierarchySheetName(const FString& SheetName, TArray<FString>& OutHierarchySheetNames);

	// 하나의 SheetBuilder를 찾거나 추가한다.
	TSharedPtr<EMSheetBuilder> FindOrAddSheetBuilder(const FString& SheetName, TSharedPtr<EMSheetBuilder> InParent);

	// SheetBuilder를 계층구조에 맞게 각각 찾거나 추가한다.
	void AddHierarchySheetBuilder(TArray<FString>& OutSheetNames);

	// SheetBilder 순서를 정렬
	TArray<TSharedPtr<EMSheetBuilder>> HierarchyShortBuilders();

	bool AnalizyEnumCells(TSharedPtr<EMSheet> Sheet, int32 InColumn, int32 InRow);

	// 하나의 EnumSheet를 파싱하여 CppParser의 Enums에 추가
	bool ParseEnumSheet(TSharedPtr<EMSheet> Sheet);

	// EnumSheet들을 파싱한다.
	bool ParseEnumSheets(TArray<TSharedPtr<EMSheet>> Sheets);

	// 하나의 MetaSheet를 파싱하여 CppParser의 Meta에 추가
	bool ParseMetaSheet(TSharedPtr<EMSheet> Sheet);
	// MetaSheet들을 파싱한다.
	bool ParseMetaSheets(TArray<TSharedPtr<EMSheet>> Sheets);


protected:
	// 최상위 SheetBuilder객체
	TSharedPtr<EMSheetBuilder> RootSheet;

	// 검색을 SheetBuilder의 맵
	TMap<FString, TSharedPtr<EMSheetBuilder>> SheetBuilderMaps;

	// 인덱스로 접근하기 위한 SheetBuilder의 Array
	TArray<TSharedPtr<EMSheetBuilder>> SheetBuilders;
	
	// SheetBuilder를 이용하여 다른 객체에 맞는 유형으로 변경하기 위한 멤버
	EMSheetBuilderTransform SheetBuilderTransform;

	// Importer를 생성하기위한 Importer Factory Array
	TArray<TSharedPtr<EMTableImportFactoryBase>> ImporterFactorys;

	// 임포트된 Array
	TArray<TSharedPtr<EMTableImport>> Importers;

	// 익스포트할 Array
	TArray<TSharedPtr<EMTableExport>> Exporters;
};

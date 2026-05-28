#pragma once

#include "EMTableGenerator.h"
#include "EMSheetBuilder.h"
#include "EMSheetBuilderTransform.generated.h"

USTRUCT()
struct FEMColor : public FColor
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT()
struct FEMVector2D : public FVector2D
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT()
struct FEMVector : public FVector
{
	GENERATED_USTRUCT_BODY()
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMDataTableListener
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FEMDataTableListener : public EMTableCore::FEMRowStructVariableListener
{
public:
	FEMDataTableListener(UScriptStruct* RowStruct, TMap<FName, uint8*>* InRowMap);
	virtual void AddRowInternal(FName RowName, uint8* RowDataPtr) override;

	static TSet<FName> RowKeys;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderTransform
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheetBuilderTransform
{
public:
	EMSheetBuilderTransform();
	virtual ~EMSheetBuilderTransform();

	bool DoTransform(const TArray<TSharedPtr<EMSheetBuilder>>& SheetBuilders);

	void Clear();

	const TArray<class UDataTable*>& GetDataTables() const { return DataTables; };

protected:
	bool IsExistKey(const EMSheetBuilder* SheetBuilder);
	void AddFieldProperty(UScriptStruct* ScriptStruct, const EMTableCore::FEMVariable& Variable, bool AddElementSize = true);
	UDataTable* CreateUAsset(UPackage* DataPackage,
		EMTableCore::FEMTableBinaryReader& Reader, const TSharedPtr<EMSheetBuilder>& SheetBuilder);

	UScriptStruct* CreateUStruct(TSharedPtr<FEMStructure> Structure);
	void CreateUStructProperties(TSharedPtr<FEMStructure> Structure);

protected:
	TArray<class UDataTable*> DataTables;
	TMap<FString, UEnum*> UnrealEnums;
};
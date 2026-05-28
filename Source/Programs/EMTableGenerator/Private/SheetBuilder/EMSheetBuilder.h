#pragma once

#include "EMTableGenerator.h"
#include "Utility/EMTableGrammar.h"
#include "Utility/EMTableGrammar.h"
#include "EMTableSheetBuilder.h"
#include "EMSheet.h"
#include "EMMetaSheet.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilder
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheetBuilder : public EMTableCore::FEMSheetBuilder
{
public:
	EMSheetBuilder(const FString& InSheetName, TWeakPtr<EMSheetBuilder> InParent);
	virtual ~EMSheetBuilder();

	virtual const FName CastName() const override { return FName("EMSheetBuilder"); };

public:
	virtual void BuildHead();
	virtual void BuildBody();

	const TArray<int32> GetKeyColumnIndices(const TArray<EMTableCore::FEMVariable>& InVariables, bool IsFillOne = true) const;
	const TArray<EMTableCore::FEMVariable> GetKeyColumns(bool IsFillOne = true) const;

	void AddAnalizer(TSharedPtr<EMSheet> Analizer);
	const TArray<TSharedPtr<EMSheet>>& GetAnalizers() const { return Analizers; }
	TArray<TSharedPtr<EMSheet>>& GetMutableAnalizers() { return Analizers; }

	bool AdjustVariableType(const FString& Cell, EMTableCore::FEMVariable& InVariable);
	TArray<EMTableCore::FEMVariable> GetOptimizeVariables(const TArray<EMTableCore::FEMVariable>& InVariables) const;
	TArray<EMTableCore::FEMVariable> GetReArrangeArrayVariables(const TArray<EMTableCore::FEMVariable>& InVariables) const;

	template<typename _TL>
	_TL StringToType(const EMTableCore::FEMVariable& InVariable, const FString& String);

	TWeakPtr<EMSheetBuilder> GetParent() const { return Parent; };
	const TArray<TWeakPtr<EMSheetBuilder>>& GetChilds() const { return Childs; };
	TArray<TWeakPtr<EMSheetBuilder>>& GetMutableChilds() { return Childs; };

	void SetExport(bool InIsExport) { IsExport = InIsExport; };
	bool GetExport() const { return IsExport; };

	EMSheetBuilder* DifferenceMergeVariables(TArray<EMTableCore::FEMVariable>& SourceVariables);
	void ReArrangeLocalToWorldVariables();

	//메타 시트 값 저장.
	TSharedPtr<EMMetaSheet> MetaSheet;

protected:
	void RemoveNoneOverlapsLocalVariables(const TMap<FString, int32>& OverlapCounts, int32 EqualCount);
	void RemoveChildOverlapsLocalVariables(const TMap<FString, int32>& OverlapCounts, int32 EqualCount);

	bool MergeVariables(TArray<EMTableCore::FEMVariable>& InDestVariables, const TArray<EMTableCore::FEMVariable>& SourceVariables);

	bool MergeLocalVariables(const TArray<EMTableCore::FEMVariable>& InVariables);
	bool MergeWorldVariables(const TArray<EMTableCore::FEMVariable>& InVariables);

	virtual void SetVariableDescription(TArray<EMTableCore::FEMVariable>& InVariables, int32 Column, const FString& Cell);
	virtual void SetVariableType(TArray<EMTableCore::FEMVariable>& InVariables, int32 Column, const FString& Cell);
	virtual void SetVariableName(TArray<EMTableCore::FEMVariable>& InVariables, int32 Column, const FString& Cell);
	virtual void SetVariableDatas(const EMSheet* Sheet, int32 Row);

	void EnumToIntegerString(const EMTableCore::FEMVariable& DestVariable, FString& CellString, bool SpaceAllow = false);

protected:
	bool ExceptionCaseKey(const EMTableCore::FEMVariable& KeyVariable, const EMSheet* Sheet, int32 Column, int32 Row) const;
	virtual void SetVariableData(const EMTableCore::FEMVariable& DestVariable, TArray<FString>& ArrayString, bool SpaceAllow = false);
	virtual void SetVariableDataStructure(const EMTableCore::FEMVariable& DestVariable, const FString& String);

protected:
	TArray<TSharedPtr<EMSheet>> Analizers;

	TWeakPtr<EMSheetBuilder>	Parent;
	TArray<TWeakPtr<EMSheetBuilder>> Childs;
	bool IsExport;
};

template<typename _TL>
inline _TL EMSheetBuilder::StringToType(const EMTableCore::FEMVariable& InVariable, const FString& String)
{
	if (true == EMTableCore::FEMTableString::IsEmptyWord(String))
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_Empty,
			*FString::Printf(TEXT("변수타잎('%s')이 비어있습니다."), *InVariable.Name), EMTableCore::EEMSyntexException::Warning);
	}
	else if (false == EMTableCore::FEMTableString::IsValidType<_TL>(String))
	{
		EMTableGrammar::OnException(EExceptionNumber::EExceptionNumber_Variable_InVaild,
			*FString::Printf(TEXT("변수타잎('%s')과 일치하는 입력값('%s')이 아닙니다."), *InVariable.Name, *String),
			EMTableCore::EEMSyntexException::Error);
	}
	return EMTableCore::FEMTableString::AtoType<_TL>(String);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EMSheetBuilderBinary_MemoryFile
////////////////////////////////////////////////////////////////////////////////////////////////////
class EMSheetBuilder_MemoryFile : public EMSheetBuilder
{
public:
	EMSheetBuilder_MemoryFile(const FString& _SheetName, TWeakPtr<EMSheetBuilder> InParent);

	virtual const FName CastName() const override { return FName("EMSheetBuilder_MemoryFile"); };
	
	TSharedPtr<EMTableCore::FEMTableMemoryFile>			GetMutableMemFile() { return MemFile; };
	const TSharedPtr<EMTableCore::FEMTableMemoryFile>	GetMemFile() const { return MemFile; };

	virtual int64	GetDataLength() const override;
	virtual uint8*	GetDataBuffer() const override;

protected:
	virtual void SetVariableData(const EMTableCore::FEMVariable& DestVariable, TArray<FString>& ArrayString, bool SpaceAllow = false) override;
	virtual void SetVariableDataStructure(const EMTableCore::FEMVariable& DestVariable, const FString& String) override;
	virtual void BuildHead() override;
	virtual void BuildBody() override;

protected:
	void SetVariableDataStructureRecursive(EMTableCore::FEMTableFileInterface& File,
		const TSharedPtr<FEMStructure> TypeStructure, TSharedPtr<FJsonValue> Value, FString StructName);

	bool SerializeVariables(EMTableCore::FEMTableFileInterface& File, const TArray<EMTableCore::FEMVariable>& OptimizeVariables);
	bool SerializeValueFromString(EMTableCore::FEMTableFileInterface& File, const EMTableCore::FEMVariable& Variable, const TArray<FString>& VariableStrings);
	bool SerializeValueFromString(EMTableCore::FEMTableFileInterface& File, const EMTableCore::FEMVariable& Variable, const FString& VariableString, FString StructName);

protected:
	TSharedPtr<EMTableCore::FEMTableMemoryFile>	MemFile;
};

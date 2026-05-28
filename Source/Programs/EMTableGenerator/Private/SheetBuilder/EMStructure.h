#pragma once

#include "EMTableGenerator.h"

class FEMStructure;
////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMStructureMember
////////////////////////////////////////////////////////////////////////////////////////////////////
class FEMStructureMember
{
public:
	FEMStructureMember(TSharedPtr<FEMStructure> InOwner, const FName& InName, const FName& InType, FString defaultValue, bool bInIsArray = false);

	const FName& GetName() const;
	const FName& GetType() const;

	bool IsArray() const;

	TWeakPtr<FEMStructure> GetOwner() const;

	EMTableCore::EEMVariabaleType GetTableVariableType() const;

	const FString GetDefaultValue() const;

	bool IsKey() const;

protected:
	TWeakPtr<FEMStructure> Owner;

	FName Name = NAME_None;
	FName Type = NAME_None;
	FString DefaultValue;
	
	bool bIsArray = false;
	bool bIsKey = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMStructure
////////////////////////////////////////////////////////////////////////////////////////////////////
class FEMStructure
{
public:
	FEMStructure(const FName& InName, bool bInIsEnum = false, bool bInIsArray = false);

	bool IsTableStructure() const;

	const FName& GetName() const;
	bool IsEnum() const;

	bool IsArray() const;

	bool AddMemeber(const FName& MemberName, TSharedPtr<FEMStructureMember> InMember);
	bool RemoveMember(const FName& MemberName);
	void ClearAllMembers();

	bool ContainsMember(const FName& MemberName);
	TSharedPtr<FEMStructureMember> GetMember(const FName& MemberName) const;
	TSharedPtr<FEMStructureMember> GetMember(const int32 Index) const;

	int32 GetMemberCount() const;

	const TMap<FName, TSharedPtr<FEMStructureMember>>& GetMembers() const;

	static FString GetClassNameUE4(FEMStructure* Structure);
	static FName GetTypeNameUE4(const FName& TypeName);

	UScriptStruct* GetUStruct();
	void SetUStruct(UScriptStruct* InStructure);

	int32 GetElementSize() const;
	int32 GetAlignSize() const;

	void AddKey(const FName& Key, bool IsUnique);
	bool IsKeyContains(const FName& Key) const;
	bool IsKeyUnique(const FName& Key) const;


protected:
	FName Name;
	bool bIsEnum = false;
	bool bIsArray = false;
	TMap<FName, TSharedPtr<FEMStructureMember>> Members;
	TArray<TSharedPtr<FEMStructureMember>> ArrayMembers;

	UScriptStruct* Structure;
	TMap<FName, bool> Keys;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMStructureManager
////////////////////////////////////////////////////////////////////////////////////////////////////
class FEMStructureManager
{
public:
	static FEMStructureManager& Inst()
	{
		static FEMStructureManager Instance;
		return Instance;
	}

	bool AddStructure(const FName& StructureFullName, TSharedPtr<FEMStructure> newStructure);
	bool RemoveStructure(const FName& StructureFullName);
	void ClearAllStructures();

	bool ContainsLeafStructure(const FName& StructureLeafName);
	const TSharedPtr<FEMStructure> GetLeafStructure(const FName& StructureName) const;
	
	bool ContainsStructure(const FName& StructureFullName);
	const TSharedPtr<FEMStructure> GetStructure(const FName& StructureFullName) const;

	const TMap<FName, TSharedPtr<FEMStructure>>& GetStructures() const;

protected:
	TMap<FName, TSharedPtr<FEMStructure>> FullnameStructures;
	TMap<FName, TSharedPtr<FEMStructure>> LeafStructures;
};
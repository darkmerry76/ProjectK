#include "EMStructure.h"
#include "Parser/EMCppParser.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMStructureMember
////////////////////////////////////////////////////////////////////////////////////////////////////
FEMStructureMember::FEMStructureMember(TSharedPtr<FEMStructure> InOwner, const FName& InName, const FName& InType, FString defaultValue, bool bInIsArray) : Owner(InOwner)
	, Name(InName)
	, Type(InType)
	, DefaultValue(defaultValue)
	, bIsArray(bInIsArray)
{

}

const FName& FEMStructureMember::GetName() const
{
	return Name;
}

const FName& FEMStructureMember::GetType() const
{
	return Type;
}

bool FEMStructureMember::IsArray() const
{
	return bIsArray;
}

bool FEMStructureMember::IsKey() const
{
	return bIsKey;
}

TWeakPtr<FEMStructure> FEMStructureMember::GetOwner() const
{
	return Owner;
}

EMTableCore::EEMVariabaleType FEMStructureMember::GetTableVariableType() const
{
	if (Type == TEXT("int32"))
	{
		return EMTableCore::EEMVariabaleType::Int32;
	}
	else if (Type == TEXT("int64"))
	{
		return EMTableCore::EEMVariabaleType::Int64;
	}
	else if (Type == TEXT("string"))
	{
		return EMTableCore::EEMVariabaleType::String;
	}
	else if (Type == TEXT("bool"))
	{
		return EMTableCore::EEMVariabaleType::Boolean;
	}
	else if (Type == TEXT("float"))
	{
		return EMTableCore::EEMVariabaleType::Float;
	}
	else if (Type == TEXT("double"))
	{
		return EMTableCore::EEMVariabaleType::Double;
	}

	const TSharedPtr<EMCppEnumNode> EnumNode = EMCppParser::Get().FindEnum(Type.ToString());
	if (true == EnumNode.IsValid())
	{
		return EMTableCore::EEMVariabaleType::Enum;
	}

	return EMTableCore::EEMVariabaleType::JsonStruct;
}

const FString FEMStructureMember::GetDefaultValue() const
{
	if (DefaultValue.IsEmpty() == true)
	{
		if (Type == TEXT("int32"))
		{
			return TEXT("0");
		}
		else if (Type == TEXT("int64"))
		{
			return TEXT("0");
		}
		else if (Type == TEXT("string"))
		{
			return TEXT("");
		}
		else if (Type == TEXT("bool"))
		{
			return TEXT("0");
		}
		else if (Type == TEXT("float"))
		{
			return TEXT("0.0f");
		}
		else if (Type == TEXT("double"))
		{
			return TEXT("0.0f");
		}
	}
	return DefaultValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMStructure
////////////////////////////////////////////////////////////////////////////////////////////////////

FEMStructure::FEMStructure(const FName& InName, bool bInIsEnum, bool bInIsArray) : Name(InName)
	, bIsEnum(bInIsEnum)
	, bIsArray(bInIsArray)
	, Structure(nullptr)
{

}

bool FEMStructure::IsTableStructure() const
{
	TArray<FString> OutStrings;

	GetName().ToString().ParseIntoArray(OutStrings, TEXT("::schema::"));
	if (2 > OutStrings.Num())
	{
		return true;
	}
	return false;
}

const FName& FEMStructure::GetName() const
{
	return Name;
}

bool FEMStructure::IsEnum() const
{
	return bIsEnum;
}

bool FEMStructure::IsArray() const
{
	return bIsArray;
}

bool FEMStructure::AddMemeber(const FName& MemberName, TSharedPtr<FEMStructureMember> InMember)
{
	if (true == ContainsMember(MemberName))
	{
		return false;
	}
	Members.Emplace(MemberName, InMember);
	ArrayMembers.Emplace(InMember);

	return true;
}

bool FEMStructure::RemoveMember(const FName& MemberName)
{
	if (false == ContainsMember(MemberName))
	{
		return false;
	}

	return 0 < Members.Remove(MemberName);
}

void FEMStructure::ClearAllMembers()
{
	Members.Empty();
}

bool FEMStructure::ContainsMember(const FName& MemberName)
{
	return Members.Contains(MemberName);
}

const TMap<FName, TSharedPtr<FEMStructureMember>>& FEMStructure::GetMembers() const
{
	return Members;
}

FString FEMStructure::GetClassNameUE4(FEMStructure* Structure)
{
	TArray<FString> OutStrings;

	Structure->GetName().ToString().ParseIntoArray(OutStrings, TEXT("::schema::"));
	if (2 > OutStrings.Num())
	{
		return TEXT("");
	}

	return FString::Printf(TEXT("FEM%s"), *OutStrings[1]);
}

FName FEMStructure::GetTypeNameUE4(const FName& TypeName)
{
	if (TypeName == TEXT("string"))
	{
		return TEXT("FString");
	}
	return TypeName;
}

void FEMStructure::SetUStruct(UScriptStruct* InStructure)
{
	Structure = InStructure;
}

int32 FEMStructure::GetElementSize() const
{
	int32 ElementSize = 0;
	for (auto Member : Members)
	{
		const TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(Member.Value->GetType());
		if (true == Member.Value->IsArray())
		{
			ElementSize += 16;
		}
		else if (true == MemberStructure.IsValid())
		{
			ElementSize += MemberStructure->GetElementSize();
		}
		else
		{
			switch(Member.Value->GetTableVariableType())
			{
				case EMTableCore::EEMVariabaleType::Boolean:
					ElementSize += 1;break;
				case EMTableCore::EEMVariabaleType::Int32:
					ElementSize += 4; break;
				case EMTableCore::EEMVariabaleType::Int64:
					ElementSize += 8; break;
				case EMTableCore::EEMVariabaleType::Float:
					ElementSize += 4; break;
				case EMTableCore::EEMVariabaleType::Double:
					ElementSize += 8; break;
				case EMTableCore::EEMVariabaleType::String:
					ElementSize += 16; break;
			}
		}
	}
	return ElementSize;
}

int32 FEMStructure::GetAlignSize() const
{
	int32 AlignSize = 0;
	for (auto Member : Members)
	{
		const TSharedPtr<FEMStructure> MemberStructure = FEMStructureManager::Inst().GetStructure(Member.Value->GetType());
		if (true == Member.Value->IsArray())
		{
			AlignSize = 8;
		}
		else if (true == MemberStructure.IsValid())
		{
			AlignSize = 8;
		}
		else
		{
			switch (Member.Value->GetTableVariableType())
			{
			case EMTableCore::EEMVariabaleType::Boolean:
				if(AlignSize < 4) AlignSize = 4; break;
			case EMTableCore::EEMVariabaleType::Int32:
				if (AlignSize < 4) AlignSize = 4; break;
			case EMTableCore::EEMVariabaleType::Int64:
				if (AlignSize < 8) AlignSize = 8; break;
			case EMTableCore::EEMVariabaleType::Float:
				if (AlignSize < 4) AlignSize = 4; break;
			case EMTableCore::EEMVariabaleType::Double:
				if (AlignSize < 8) AlignSize = 8; break;
			case EMTableCore::EEMVariabaleType::String:
				if (AlignSize < 8) AlignSize = 8; break;
			}
		}
	}
	return AlignSize;
}

void FEMStructure::AddKey(const FName& Key, bool IsUnique)
{
	bool IsExist = IsKeyContains(Key);
	ensure(false == IsExist);

	if (true == IsExist)
	{
		return;
	}
	Keys.Emplace(Key, IsUnique);
}

bool FEMStructure::IsKeyContains(const FName& Key) const
{
	return Keys.Contains(Key);
}

bool FEMStructure::IsKeyUnique(const FName& Key) const
{
	const bool* Result = Keys.Find(Key);
	if (nullptr == Result)
	{
		return false;
	}
	return *Result;
}

UScriptStruct* FEMStructure::GetUStruct()
{
	return Structure;
}

TSharedPtr<FEMStructureMember> FEMStructure::GetMember(const FName& MemberName) const
{
	const TSharedPtr<FEMStructureMember>* Result = Members.Find(MemberName);
	if(nullptr == Result)
	{
		return nullptr;
	}
	return *Result;
}

TSharedPtr<FEMStructureMember> FEMStructure::GetMember(const int32 Index) const
{
	return ArrayMembers[Index];
}

int32 FEMStructure::GetMemberCount() const
{
	return ArrayMembers.Num();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMStructureManager
////////////////////////////////////////////////////////////////////////////////////////////////////

bool FEMStructureManager::AddStructure(const FName& StructureName, TSharedPtr<FEMStructure> newStructure)
{
	if (true == ContainsStructure(StructureName))
	{
		return false;
	}
	FullnameStructures.Emplace(StructureName, newStructure);

	TArray<FString> splitNames;
	FString finalStructName = StructureName.ToString().Replace(TEXT("::schema"), TEXT(""));

	finalStructName.ParseIntoArray(splitNames, TEXT("::"));
	LeafStructures.Emplace(*splitNames[splitNames.Num() - 1], newStructure);
	return true;
}

bool FEMStructureManager::RemoveStructure(const FName& StructureName)
{
	if (false == ContainsStructure(StructureName))
	{
		return false;
	}
	return 0 < FullnameStructures.Remove(StructureName);
}

void FEMStructureManager::ClearAllStructures()
{
	FullnameStructures.Empty();
}

bool FEMStructureManager::ContainsStructure(const FName& StructureName)
{
	return FullnameStructures.Contains(StructureName);
}

bool FEMStructureManager::ContainsLeafStructure(const FName& StructureLeafName)
{
	return LeafStructures.Contains(StructureLeafName);
}

const TSharedPtr<FEMStructure> FEMStructureManager::GetLeafStructure(const FName& StructureName) const
{
	const TSharedPtr<FEMStructure>* Result = LeafStructures.Find(*StructureName.ToString().Replace(TEXT("[]"), TEXT("")));
	if (nullptr == Result)
	{
		return nullptr;
	}
	return *Result;
}

const TSharedPtr<FEMStructure> FEMStructureManager::GetStructure(const FName& StructureName) const
{
	const TSharedPtr<FEMStructure>* Result = FullnameStructures.Find(*StructureName.ToString().Replace(TEXT("[]"), TEXT("")));
	if (nullptr == Result)
	{
		return nullptr;
	}
	return *Result;
}

const TMap<FName, TSharedPtr<FEMStructure>>& FEMStructureManager::GetStructures() const
{
	return FullnameStructures;
}
#include "EMTableExportCode_ClientEnum.h"
#include "SheetBuilder/EMSheetBuilder.h"
#include "Config/EMTableConfig.h"
#include "Parser/EMCppParser.h"
#include "SheetBuilder/EMSheetBuilderMgr.h"
#include "Utility/EMTableUtility.h"

EMTableExportCode_ClientEnum::EMTableExportCode_ClientEnum() : EMTableExport()
{
}

bool EMTableExportCode_ClientEnum::DoExport()
{
	FString ResultString;

	ResultString += FString::Printf(TEXT("#pragma once\r\n"));
	ResultString += FString::Printf(TEXT("\r\n"));
	ResultString += FString::Printf(TEXT("#include \"CoreMinimal.h\"\r\n"));

	FString MergeFilename = EMTableUtility::GetFullPathToFilename(EMTableConfig::Get().GetCodeSection().EnumMergeFilename);

	bool IsFirst = true;
	for (auto it = EMCppParser::Get().GetEnums().CreateConstIterator(); it; ++it)
	{
		TSharedPtr<EMCppEnumNode> EnumNode = it->Value;
		if (false == EnumNode.IsValid())
		{
			continue;
		}
		if (EnumNode->OwnerType != eEnumOnwerType::eEnumOnwerType_Excel)
		{
			continue;
		}
		if(true == IsFirst)
		{
			ResultString += FString::Printf(TEXT("#include \"%s.generated.h\"\r\n"), *MergeFilename);
			ResultString += FString::Printf(TEXT("\r\n"));
			IsFirst = false;
		}
		ResultString += FString::Printf(TEXT("\r\n"));

		FString EnumDescription = EnumNode->Description.Len() > 0 ?
			FString::Printf(TEXT("// %s\r\n"), *EnumNode->Description) : TEXT("");

		ResultString += FString::Printf(TEXT("%s"), *EnumDescription);

		if(EnumNode->VariableType == EMTableCore::EEMEnumVariableType::Uint8)
		{
			ResultString += FString::Printf(TEXT("UENUM(BlueprintType)\r\n"));
		}
		else
		{
			ResultString += FString::Printf(TEXT("UENUM()\r\n"));
		}

		const EMTableCore::FEMEnumVariableTypeInfo& EnumVariableTypeInfo = EMTableCore::GetEnumVariableInfo(EnumNode->VariableType);

		ResultString += FString::Printf(TEXT("enum class %s : %s\r\n"), *EnumNode->Name, *EnumVariableTypeInfo.UnrealTypeName);
		ResultString += FString::Printf(TEXT("{\r\n"));

		FString NoneString = EnumNode->Name + "_None";
		FString ResultEnumMemberString;

		bool IsExistNone = false;

		for (int32 i = 0; i < EnumNode->Members.Num(); i++)
		{
			FString EnumMemberDescription = EnumNode->Members[i]->Description.Len() > 0 ?
				FString::Printf(TEXT("// %s"), *EnumNode->Members[i]->Description) : TEXT("");

			if (0 == EnumNode->Members[i]->Value)
			{
				IsExistNone = true;
			}
			FString FinalDisplayName = EnumNode->Members[i]->DisplayName.Len() > 0 ? FString::Printf(TEXT("UMETA(DisplayName = %s) "), *EnumNode->Members[i]->DisplayName) : TEXT("");

			ResultEnumMemberString += FString::Printf(TEXT("\t%s = %d %s, %s\r\n"),
				*EnumNode->Members[i]->Name, EnumNode->Members[i]->Value, *FinalDisplayName, *EnumMemberDescription);
		}
		if (false == IsExistNone)
		{
			ResultString += FString::Printf(TEXT("\t%s = 0,\n"), *NoneString);
		}
		ResultString += ResultEnumMemberString;
		ResultString += FString::Printf(TEXT("};\r\n"));
	}

	if (false == EMTableUtility::WriteUTF8FileW(*EMTableConfig::Get().GetCodeSection().GetFinalMergeEnumFilename(), ResultString))
	{
		EM_LOG(EMLog, Error, TEXT("EnumHeader[%s] 저장중 쓰기에 실패했습니다.!!!"), *EMTableConfig::Get().GetCodeSection().GetFinalMergeEnumFilename());
		return false;
	}

	return true;
}
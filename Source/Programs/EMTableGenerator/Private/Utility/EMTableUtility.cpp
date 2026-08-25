#include "EMTableUtility.h"

#include <tchar.h>

#include "EMTableGrammar.h"
#include "JsonReader.h"
#include "JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "Core/Public/HAL/PlatformFilemanager.h"
#include "Core/Public/Misc/Paths.h"
#include "RunTime/Launch//Public/LaunchEngineLoop.h"
#include "Core/Public/Modules/ModuleManager.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

namespace EMTableUtility
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetFullPathToFilename
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetFullPathToFilename(const FString& FullPath)
{
	return FPaths::GetBaseFilename(FullPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetFullPathToFilenameOrigin
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetFullPathToFilenameOrigin(const FString& FullPath)
{
	FString Result = FPaths::GetBaseFilename(FullPath);

	TArray<FString> OutFilenames;

	Result.ParseIntoArray(OutFilenames, TEXT("."));
	if (0 == OutFilenames.Num())
	{
		return FString(TEXT(""));
	}
	if (OutFilenames.Num() == 2)
	{
		return OutFilenames[0] + TEXT("::") + OutFilenames[1];
	}
	return OutFilenames[0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetFullPathToExt
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetFullPathToExt(const FString& FullPath)
{
	return FPaths::GetExtension(FullPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetFullPathToFilenameExt
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetFullPathToFilenameExt(const FString& FullPath)
{
	FString OutPath;
	OutPath = FPaths::GetBaseFilename(FullPath);
	FString Ext = FPaths::GetExtension(FullPath);
	if (false == Ext.IsEmpty())
	{
		OutPath += FString(TEXT(".")) + Ext;
	}

	return OutPath;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetFullPathToPathname
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetFullPathToPathname(const FString& FullPath)
{
	FString finalPath = FPaths::GetPath(FullPath);
	if(finalPath.IsEmpty() == false)
	{
		return finalPath + TEXT("/");
	}
	return FString(TEXT(""));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetFullPathToPathnameOrganize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetFullPathToPathnameOrganize(const FString& FullPath)
{
	TCHAR SavePath[MAX_PATH];
	FMemory::Memzero(SavePath, MAX_PATH);

	_tgetcwd(SavePath, MAX_PATH);
	FString ResultPath = GetFullPathToPathname(FullPath);
	_tchdir(*ResultPath);	

	TCHAR CurrentPath[MAX_PATH];
	FMemory::Memzero(CurrentPath, MAX_PATH);

	_tgetcwd(CurrentPath, MAX_PATH);

	_tchdir(SavePath);

	FString OutPath = FString(CurrentPath) + TEXT("/");
	OutPath = OutPath.Replace(TEXT("\\"), TEXT("/"));

	return OutPath;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetFullPathToFullPathnameOrganize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetFullPathToFullPathnameOrganize(const FString& FullPath)
{
	return EMTableUtility::GetFullPathToPathnameOrganize(FullPath) + EMTableUtility::GetFullPathToFilenameExt(FullPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetOnlyPathToPathname
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FString GetOnlyPathToPathname(const FString& FullPath)
{
	FString FinalPathName = GetFullPathToPathname(FullPath);
	int32 DriveIndex = FinalPathName.Find(TEXT(":"));
	if (DriveIndex != INDEX_NONE)
	{
		for (int i = 0; i <= DriveIndex; i++) FinalPathName = FinalPathName.RightChop(1);
	}
	if (false == FinalPathName.EndsWith(TEXT("/")))
	{
		FinalPathName += TEXT("/");
	}
	FinalPathName = FinalPathName.Replace(TEXT("\\"), TEXT("/"));
	return FinalPathName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WriteFile
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadFile(const TCHAR* Filename, TArray<uint8>& OutResult)
{
	IFileHandle* FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenRead(Filename);
	if (nullptr == FileHandle)
	{
		return false;
	}
	FString Result;

	OutResult.SetNumZeroed(FileHandle->Size() + 1);

	FileHandle->Read(OutResult.GetData(), FileHandle->Size());
	if (OutResult.Num() > 2)
	{
		if (OutResult[0] == 0xEF && OutResult[1] == 0xBB && OutResult[2] == 0xBF)
		{
			OutResult.RemoveAt(0, 3);
		}
	}

	delete FileHandle;

	return true;
}

bool WriteFile(const TCHAR* Filename, const uint8* Buffer, int32 Length)
{
	IFileHandle* FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenWrite(Filename);
	if (nullptr == FileHandle)
	{
		return false;
	}
	FileHandle->Write(Buffer, Length);

	delete FileHandle;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WriteUTF8File
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool WriteUTF8FileW(const TCHAR* Filename, const FString& Buffer)
{
	EMTableCore::FEMStringA UTF8String = TCHAR_TO_UTF8(*Buffer);
	return WriteFile(Filename, (const uint8*) *UTF8String, UTF8String.Len());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LoadJsonObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TSharedPtr<FJsonObject> LoadJsonObject(const FString& Filename)
{
	TArray<uint8> JsonArray;
	if (false == EMTableUtility::ReadFile(*Filename, JsonArray))
	{
		return nullptr;
	}
	
	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(UTF8_TO_TCHAR(reinterpret_cast<CHAR*>(JsonArray.GetData())));
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	FJsonSerializer::Deserialize(Reader, JsonObject);

	return JsonObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsEMPrefix
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsEMPrefix(const FString& SheetName, const FString& PrefixName)
{
	if (SheetName.Len() < PrefixName.Len())
	{
		return false;
	}
	for (int32 i = 0; i < PrefixName.Len(); i++)
	{
		if (TChar<TCHAR>::ToUpper((*SheetName)[i]) != TChar<TCHAR>::ToUpper((*PrefixName)[i]))
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsWordString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsWordString(const FString& String)
{
	const TCHAR* StrBuffer = String.GetCharArray().GetData();

	int32 WordCount = 0;
	for (int32 i = 0; i < String.Len(); i++)
	{
		TCHAR ch = StrBuffer[i];
		if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') ||
			('0' <= ch && ch <= '9') || ch == '_')
		{
			WordCount++;
		}
		else
		{
			if (ch == ' ' || ch == '\t')
			{
				continue;
			}

			return false;
		}
	}
	return (0 < WordCount) ? true : false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsNoSpaceWordString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsNoSpaceWordString(const FString& String)
{
	const TCHAR* StrBuffer = String.GetCharArray().GetData();

	int32 WordCount = 0;
	for (int32 i = 0; i < String.Len(); i++)
	{
		TCHAR ch = StrBuffer[i];
		if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') ||
			('0' <= ch && ch <= '9') || ch == '_')
		{
			WordCount++;
		}
		else
		{
			if (WordCount == 0 && (ch == ' ' || ch == '\t'))
			{
				continue;
			}

			return false;
		}
	}
	return (0 < WordCount) ? true : false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsWordIncludeSpaceString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsWordIncludeSpaceString(const FString& String)
{
	const TCHAR* StrBuffer = String.GetCharArray().GetData();

	int32 WordCount = 0;
	for (int32 i = 0; i < String.Len(); i++)
	{
		TCHAR ch = StrBuffer[i];
		if (ch == ' ' || ch == '\t')
		{
			if (0 < WordCount)
			{
				return true;
			}
		}
		else WordCount++;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsNumericString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsNumericString(const FString& String)
{
	const TCHAR* StrBuffer = String.GetCharArray().GetData();
	int32 NumberCount = 0;
	for (int32 i = 0; i < String.Len(); i++)
	{
		TCHAR ch = StrBuffer[i];
		if ('0' <= ch && ch <= '9')
		{
			NumberCount++;
		}
		else
		{
			if (NumberCount == 0 && (ch == ' ' || ch == '\t'))
			{
				continue;
			}

			return false;
		}
	}

	return (0 < NumberCount) ? true : false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsEmptyWordString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsEmptyWordString(const FString& String)
{
	int32 EmptyCount = 0;
	if (String.Len() == 0)
	{
		return true;
	}
	const TCHAR* StrBuffer = String.GetCharArray().GetData();
	int32 WordCount = 0;
	for (int32 i = 0; i < String.Len(); i++)
	{
		TCHAR ch = StrBuffer[i];
		if (ch != ' ' && ch != '\t')
		{
			WordCount++;
		}
	}
	return (0 < WordCount) ? false : true;
}

void SetConsoleTextColor(uint16 ColorID)
{
#if PLATFORM_WINDOWS
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ColorID);
	switch(ColorID)
	{
		case 7: printf("\x1b[31m"); break;
		case 12: printf("\x1b[31m");break;
		case 14: printf("\x1b[31m"); break;
	}
#endif
}

int32 ShutdownApplication()
{
	if (0 < EMTableGrammar::FatalExceptionCount)
	{
		EM_LOG(EMLog, Error, TEXT("%d개의 오류가 있습니다. 오류를 수정하고 다시 빌드하십시오. 오류가 있으면 Table은 데이터는 퍼포스에 올라가지 않습니다. 안심하세요!!!"), EMTableGrammar::FatalExceptionCount);
	}
	else if (0 < EMTableGrammar::WarningExceptionCount)
	{
		EM_LOG(EMLog, Warning, TEXT("%d개의 경고가 있습니다. 경고는 감춰져 있을수 있습니다.(대부분 데이터 경고는 0이나 비어있음으로 처리됩니다.)"), EMTableGrammar::WarningExceptionCount);
	}

	if (0 == EMTableGrammar::FatalExceptionCount)
	{
		EM_LOG(EMLog, Display, TEXT("빌드성공"));
	}
	else
	{
		EM_LOG(EMLog, Error, TEXT("빌드실패!!!"));
	}

	FEngineLoop::AppPreExit();
	FModuleManager::Get().UnloadModulesAtShutdown();
	FEngineLoop::AppExit();

	return (EMTableGrammar::FatalExceptionCount != 0) ? 1 : 0;
}

FString RemoveSpaceWord(const FString& Word)
{
	return Word.Replace(TEXT(" "), TEXT("")).Replace(TEXT("\t"), TEXT(""));
}
};

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif
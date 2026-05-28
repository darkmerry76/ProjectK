#include "EMTableFeedbackContext.h"

#if PLATFORM_WINDOWS
#include "locale.h"
#endif

FTableFeedbackContext::FTableFeedbackContext() : FFeedbackContextAnsi()
{
#if PLATFORM_WINDOWS
	OldLocale = _tsetlocale(LC_ALL, TEXT("korean"));
#endif
}

FTableFeedbackContext::~FTableFeedbackContext()
{
#if PLATFORM_WINDOWS
	if(nullptr != OldLocale)
	{
		_tsetlocale(LC_ALL, OldLocale);
	}
#endif
}

/*void FTableFeedbackContext::LocalPrint(const TCHAR* Str)
{
#if PLATFORM_WINDOWS
	printf("%s", TCHAR_TO_UTF8(Str));
#endif
}

void FTableFeedbackContext::Serialize(const TCHAR* V, EMogVerbosity::Type Verbosity, const class FName& Category)
{
	static bool bUsingStdOut = FParse::Param(FCommandLine::Get(), TEXT("stdout")) ||
		(KMogConsole != nullptr && KMogConsole->IsShown());

	if (bUsingStdOut == false &&
		(Verbosity == EMogVerbosity::Error || Verbosity == EMogVerbosity::Warning || Verbosity == EMogVerbosity::Display))
	{
		if (TreatWarningsAsErrors && Verbosity == EMogVerbosity::Warning)
		{
			Verbosity = EMogVerbosity::Error;
		}

		FString Prefix;
		if (Context)
		{
			Prefix = Context->GetContext() + TEXT(" : ");
		}
		FString Format = Prefix + FOutputDeviceHelper::FormatLogLine(Verbosity, Category, V);
		if (IsRunningCommandlet())
		{
			if (Verbosity == EMogVerbosity::Error)
			{
				AddError(Format);
			}
			else if (Verbosity == EMogVerbosity::Warning)
			{
				AddWarning(Format);
			}
		}
		LocalPrint(*Format);
		LocalPrint(TEXT("\n"));
	}
	else if (Verbosity == EMogVerbosity::SetColor)
	{
	}
	if (!KMog->IsRedirectingTo(this))
		KMog->Serialize(V, Verbosity, Category);
	if (AuxOut)
		AuxOut->Serialize(V, Verbosity, Category);
	fflush(stdout);
}*/
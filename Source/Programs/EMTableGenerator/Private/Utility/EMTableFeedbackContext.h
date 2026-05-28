#pragma once

#include "CoreMinimal.h"
#include "HAL/FeedbackContextAnsi.h"

class FTableFeedbackContext : public FFeedbackContextAnsi
{
public:
	FTableFeedbackContext();
	virtual ~FTableFeedbackContext();

private:
	//void LocalPrint(const TCHAR* Str);
	//void Serialize(const TCHAR* V, EMogVerbosity::Type Verbosity, const class FName& Category) override;

private:
	TCHAR* OldLocale;
};
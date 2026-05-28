#pragma once

#include "EMTableGenerator.h"

using MetaProperty = TPair<FString, FString>;

class EMMetaSheet
{

protected:

	TMap<FString, TArray<MetaProperty>>	MetaPropertyMap;
	
public:
	void AddMeta(FString VariableName, FString MetaKey, FString MetaValue);

	FString GetMetaString(FString VariableName, FString DefaultMeta = TEXT(""));
};
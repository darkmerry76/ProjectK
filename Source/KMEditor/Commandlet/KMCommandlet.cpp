#include "KMCommandlet.h"
#include "KMCommandletType.h"
#include "UObject/UObjectIterator.h"

UKMCommandlet::UKMCommandlet(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	if (HasAllFlags(RF_ClassDefaultObject) == true)
	{
		GetClass()->Rename(TEXT("CommandCommandlet"), GetClass()->GetOuter());
	}
}

void UKMCommandlet::RegisterCommands()
{
	for (TObjectIterator<UClass> classItr; classItr; ++classItr)
	{
		UClass* Class = *classItr;
		check(IsValid(Class) == true);
		
		UKMCommand* commandCDO = Cast<UKMCommand>(Class->GetDefaultObject());
		if (IsValid(commandCDO) == true && Class->HasAnyClassFlags(CLASS_Abstract) == false)
		{
			CommandMap.Emplace(commandCDO->GetCommandParamName(), commandCDO);
		}
	}	
}

int32 UKMCommandlet::Main(const FString& Params)
{
	TArray<FString> tokenCommands, Switches;
	ParseCommandLine(*Params, tokenCommands, Switches);

	if(tokenCommands.Num() == 0)
	{
		return 1;
	}

	RegisterCommands();

	UKMCommand** existCommand = CommandMap.Find(*tokenCommands[0]);
	if (existCommand != nullptr && IsValid(*existCommand) == true)
	{
		(*existCommand)->Main(tokenCommands);
	}
	
	return 0;
}
#pragma once

#include "CoreMinimal.h"
#include "EMGameInstanceSubsystem.h"
#include "EMWorldSubsystem.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UEMWorldSubsystem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, Abstract)
class EMCOREGAME_API UEMWorldSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual void OnLevelAdded(ULevel* level, UWorld* world);

	virtual void OnLoadingComplete();
};
#pragma once

#include "CoreMinimal.h"
#include "Stat/KMSecondaryBaseStat.h"
#include "System/EMCheatManager.h"
#include "UObject/Object.h"
#include "KMCheatManager.generated.h"


UCLASS()
class KMGAME_API UKMCheatManager : public UEMCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, BlueprintCallable, Category = "KMCheatManager")
	virtual void KMSetStage(const FName& stageId, const FName& chapterId, int32 waveLevel);
};

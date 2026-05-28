#pragma once

#include "CoreMinimal.h"
#include "Core/KMPlayerAccountDataTypes.h"
#include "Engine/DataAsset.h"
#include "KMPlayerAccount.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMPlayerAccount : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKMAccountInfo BaseInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKMAccountHeroListInfo CollectedHeroListInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKMAccountDeckInstance CurDeckInstance;

public:
	void SetCurrentStageInfo(FName stageId, FName chapterId, int32 waveLevel);
};

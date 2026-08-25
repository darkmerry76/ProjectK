#pragma once

#include <Tables/Generated/KMTable_Item.h>
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "KMPlayerAccountDataTypes.generated.h"

USTRUCT(BlueprintType)
struct KMGAME_API FKMAccountStageInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StageId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ChapterId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveLevel;
};

USTRUCT(BlueprintType)
struct KMGAME_API FKMAccountHeroInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HeroId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCollected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OutLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BattlePower;
	
	const struct FKMTable_Object_HeroRow* HeroRecord = nullptr;
};

USTRUCT(BlueprintType)
struct KMGAME_API FKMAccountHeroListInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FKMAccountHeroInfo> HeroList;
};

USTRUCT(BlueprintType)
struct KMGAME_API FKMAccountDeckInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> InitialDeckHeroList;
};

USTRUCT(BlueprintType)
struct KMGAME_API FKMAccountItemInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 0;

	const FKMTable_ItemRow* ItemRecord; 
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FKMData_Item;
};

USTRUCT(BlueprintType)
struct KMGAME_API FKMAccountInventoryInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FKMAccountItemInfo> ItemList;
};

USTRUCT(BlueprintType)
struct KMGAME_API FKMAccountInfo
{
	GENERATED_BODY()

public:
	// ==============================================
	UPROPERTY()
	int64 AccountId = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 LevelPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 Coin; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalPlayerExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProfileIconId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProfileFrameId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StatusMessage;

	FName LastFieldStageId;
	FName CurSelectedStageId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FKMAccountStageInfo> StageList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> UnlockIdList;

	UPROPERTY()
	TMap<FName, int32> PlayerBuffSkillList;

public:
	void SetStageInfo(const FName& stageId, const FName& chapterId, int32 waveLevel);
	FKMAccountStageInfo* GetStageInfo(FName stageId);
};

#pragma once

#include "CoreMinimal.h"
#include "Account/KMPlayerAccount.h"
#include "System/EMSaveGame.h"
#include "UObject/Object.h"
#include "KMSaveGame.generated.h"

UCLASS()
class KMGAME_API UKMSaveGame : public UEMSaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "PlayerData")
	FString SaveSlotName = TEXT("PlayerSaveData");
	
	UPROPERTY(VisibleAnywhere, Category = "PlayerData")
	TMap<int64, FKMAccountInfo> PlayerAccountMap;
	
public:
	UKMSaveGame();
};

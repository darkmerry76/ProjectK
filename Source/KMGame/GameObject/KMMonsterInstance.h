#pragma once

#include "CoreMinimal.h"
#include "KMCharacterInstance.h"
#include "Engine/TimerHandle.h"
#include "KMMonsterInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FKMSpawnPurifyDelegate, class UKMMonsterInstance* monster)

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMMonsterInstance : public UKMCharacterInstance
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay() override;

	virtual void OnDeath() override;

	virtual bool CanLockOn() const override;
	virtual bool CanBeTargeted() const override;
	virtual bool CanReceiveReward() const override;

public:
	static FKMSpawnPurifyDelegate& GetSpawnPurifyDelegate()
	{
		static FKMSpawnPurifyDelegate purifyDelegate;
		return purifyDelegate;
	}

protected:
	FTimerHandle DeathDelayTimer;
};
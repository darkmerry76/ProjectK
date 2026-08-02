#pragma once

#include "CoreMinimal.h"
#include "KMSecondaryBaseStat.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMStatModifierBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class KMGAME_API UKMStatModifierBase : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Init();
	virtual void Deinit();
	virtual void Compact();
	
	UFUNCTION(BlueprintPure, Category = "Stat")
	FName GetBaseStatKey() const;

	UFUNCTION(BlueprintPure, Category = "Stat")
	class UKMGameObjectInstance* GetOwner() const;
	
	virtual void ApplyLevel(int32 newLevel, bool bCurrentFull = false);
	
	FKMSecondaryStat& GetEffectiveStat();

	class UObject* ApplyEffectiveAnimation(const FName& pDAKey);
	void RemoveEffectiveAnimation(class UObject* effectObject, bool bCancel = false);

	int32 GetLevel() const { return Level; }

	void ComputePreEffectStat();;
	void ComputePostEffectStat();;

protected:
	virtual void ComputeEffectLevelStatInteral(int32 level);

protected:
	FKMBaseStat LevelBaseStat;
	FKMSecondaryStat EffectiveStat;
	int32 Level = 0;

	UPROPERTY()
	TSet<TObjectPtr<UObject>> AbnormalAbilities;
};

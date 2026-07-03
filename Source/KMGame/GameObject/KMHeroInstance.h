#pragma once

#include "CoreMinimal.h"
#include "KMCharacterInstance.h"
#include "KMHeroInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMHeroInstance : public UKMCharacterInstance
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnHeroSelected();

	UFUNCTION(BlueprintNativeEvent)
	void OnEnterGame();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay() override;

	virtual bool CanLockOn() const override;
	virtual bool CanBeTargeted() const override;

protected:
	void ApplyPlayerSkill(const FName& skillId, int32 skillLevel);
};
#pragma once

#include "CoreMinimal.h"
#include "KMPayloadBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMPayloadBase : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate();
	virtual void Deactivate();

	virtual void OnHit(const FHitResult& HitResult);

	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetCharacterInstance() const;

	UFUNCTION(BlueprintPure)
	class AKMCharacter* GetCharacter() const;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnActivate();

	UFUNCTION(BlueprintNativeEvent)
	void OnDeactivate();
};
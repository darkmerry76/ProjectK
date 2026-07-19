#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMAbility.h"
#include "Core/KMDefine.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMAbilityEffect.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilityEffect : public UKMAbility
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EndingTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDirectionFallow = false;

	UPROPERTY()
	TWeakObjectPtr<UKMCharacterInstance> CasterCharacterObject;

protected:
	FKMObjectKey CastObjectKey;

public:
	void SetCastObjectKey(FKMObjectKey newCasterObjectKey);
	const FKMObjectKey& GetCastObjectKey() const { return CastObjectKey; }

	UFUNCTION(BlueprintPure)
	class AKMCharacter* GetCasterCharacter() const;

	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetCasterCharacterInstance() const;
	
	virtual void Activate() override;
	virtual void Deactivate() override;
};
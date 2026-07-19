#pragma once

#include "CoreMinimal.h"
#include "KMAbility.h"
#include "Tables/Generated/KMTableEnums.h"
#include "KMAbilitySkill.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class UKMAbilitySkill : public UKMAbility
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsUseAnimSet = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDirectionFallow = true;
	
public:
	virtual void Activate() override;
	virtual void Deactivate() override;
};
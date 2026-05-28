#pragma once

#include "CoreMinimal.h"
#include "System/EMGameInstanceSubsystem.h"
#include "KMSkillSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMSkillSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
};

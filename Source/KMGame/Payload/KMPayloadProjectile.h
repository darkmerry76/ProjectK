#pragma once

#include "CoreMinimal.h"
#include "KMPayloadBase.h"
#include "KMPayloadProjectile.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMPayloadProjectile : public UKMPayloadBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate() override;
	virtual void Deactivate() override;
};
#pragma once

#include "CoreMinimal.h"
#include "KMPayloadBase.h"
#include "KMPayloadChain.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMPayloadChain : public UKMPayloadBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Activate() override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintPure)
	class USkeletalMeshComponent* GetChainMesh() const;
};
#pragma once

#include "CoreMinimal.h"
#include "Character/EMCharacterAnimInstance.h"
#include "KMChainAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMChainAnimInstance : public UEMCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPoseSnapshot Snapshot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendAlpha = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EnableAttack = false;
};
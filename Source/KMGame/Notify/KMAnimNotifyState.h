#pragma once

#include "CoreMinimal.h"
#include "Notify/EMAnimNotifyStateMA.h"
#include "GameplayTagContainer.h"
#include "KMAnimNotifyState.generated.h"

UENUM(BlueprintType)
enum class EKMTageCompareType : uint8
{
	None,
	Equal,
	NotEqual,
};

UENUM(BlueprintType)
enum class EKMTageOperatorType : uint8
{
	None,
	Or,
	And,
};

USTRUCT(Blueprintable, BlueprintType)
struct KMGAME_API FKMCompareTagData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMTageCompareType Compare = EKMTageCompareType::Equal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EKMTageOperatorType Operator = EKMTageOperatorType::Or;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Tag;
};

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMAnimNotifyState : public UEMAnimNotifyStateMA
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category=AnimNotify)
	TArray<FKMCompareTagData> CompareTags;

protected:
	bool IsCompareTag(const class UKMCharacterInstance* ownerCharacterInstance) const;
};

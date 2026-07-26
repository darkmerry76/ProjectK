#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "StructUtils/InstancedStruct.h"
#include "KMAnimNotifyState_Parameter.generated.h"

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Parameter")
class KMGAME_API UKMAnimNotifyState_Parameter : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	TArray<TInstancedStruct<struct FKMParameterBase>> Parameters;

public:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	FName GetLayerName() const;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};

#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_AttachInteractiveActor.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotifyState_AttachInteractiveActor
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Attach InteractiveActor")
class KMGAME_API UKMAnimNotifyState_AttachInteractiveActor : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, AnimNotifyBoneName="true", DisplayAfter="AttachTransform"))
	FName AttachSocket = NAME_None;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="bIsEndDetach"))
	TSubclassOf<class AKMInteractiveActorBase> PreviewInteractiveActorClass;
#endif

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
};

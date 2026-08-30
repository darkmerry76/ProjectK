#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_AttachInteractiveActor.generated.h"

USTRUCT()
struct KMGAME_API FKMAnimNotifyState_AttachInteractiveActor_Context
{
	GENERATED_USTRUCT_BODY()

	bool IsValid() const;

	UPROPERTY()
	TObjectPtr<class AKMInteractiveActorBase> InteractiveActor = nullptr;
};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="AttachSocket"))
	float BlendingDuration = 0.2f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="BlendingDuration"))
	TSubclassOf<class AKMInteractiveActorBase> PreviewInteractiveActorClass;
#endif

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
	TSharedPtr<FKMAnimNotifyState_AttachInteractiveActor_Context> GetContext(class USkeletalMeshComponent* meshComp, const FAnimNotifyEventReference& eventReference) const;

protected:
	mutable TMap<TObjectPtr<class USkeletalMeshComponent>, TSharedPtr<FKMAnimNotifyState_AttachInteractiveActor_Context>> Contexts;
};

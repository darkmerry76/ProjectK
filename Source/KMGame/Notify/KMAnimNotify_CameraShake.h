#pragma once

#include "CoreMinimal.h"
#include "Notify/EMAnimNotifyMA.h"
#include "KMAnimNotify_CameraShake.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotify_CameraShake
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play CameraShake")
class KMGAME_API UKMAnimNotify_CameraShake : public UEMAnimNotifyMA
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	TSubclassOf<class UCameraShakeBase> CameraShakeClass;
	
public:
	UKMAnimNotify_CameraShake();

protected:
	virtual void Notify(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;
	
protected:
	virtual FString GetNotifyName_Implementation() const override;
};

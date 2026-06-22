#pragma once

#include "CoreMinimal.h"
#include "EMMartialArtsType.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "Notify/EMAnimNotifyInterfaceMA.h"
#include "KMAnimNotify_Sound.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotify_Sound
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Sound")
class KMGAME_API UKMAnimNotify_Sound : public UAnimNotify_PlaySound, public IEMAnimNotifyInterfaceMA
{
	GENERATED_BODY()
	
public:
	UKMAnimNotify_Sound();
	
protected:
	UPROPERTY(VisibleAnywhere, Category=AnimNotify, meta=(AllowPrivateAccess=true))
	EEMNotifyGroupType GroupType = EEMNotifyGroupType::Sound;

protected:
	virtual void Notify(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;
	
protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual EEMNotifyGroupType GetGroupType() override { return GroupType; };
};

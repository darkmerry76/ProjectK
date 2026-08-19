#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KMAnimNotify.h"
#include "KMAnimNotify_Sound.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMAnimNotify_Sound
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play Sound")
class KMGAME_API UKMAnimNotify_Sound : public UKMAnimNotify
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="AnimNotify", DisplayName="UseSoundSet", meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	bool bIsUseSoundSet = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(EditCondition="bIsUseSoundSet", DisplayAfter="bIsUseSoundSet", ExposeOnSpawn=true))
	FGameplayTag SoundTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(EditCondition="!bIsUseSoundSet", DisplayAfter="SoundTag", ExposeOnSpawn=true))
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(DisplayAfter="Sound", ExposeOnSpawn=true))
	float VolumeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(DisplayAfter="VolumeMultiplier", ExposeOnSpawn=true))
	float PitchMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(DisplayAfter="PitchMultiplier"))
	uint32 bFollow:1 = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta=(DisplayAfter="bFollow"))
	uint32 bPreviewIgnoreAttenuation:1 = false;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(EditCondition="bFollow", DisplayAfter="bPreviewIgnoreAttenuation", ExposeOnSpawn=true))
	FName AttachName = NAME_None;

protected:
	virtual void Notify(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;
	class USoundBase* GetUsedSound(const class USkeletalMeshComponent* meshComp) const;

#if WITH_EDITOR
	virtual void ValidateAssociatedAssets() override;
	virtual void PostEditChangeProperty(AActor* ownerActor, FPropertyChangedEvent& propertyChangedEvent) override;
#endif
	
protected:
	virtual FString GetNotifyName_Implementation() const override;
};

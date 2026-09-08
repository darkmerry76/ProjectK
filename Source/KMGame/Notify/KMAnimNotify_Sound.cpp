#include "KMAnimNotify_Sound.h"
#include "EMMartialArts.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/UObjectToken.h"

UKMAnimNotify_Sound::UKMAnimNotify_Sound(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
#if WITH_EDITOR
	UEMAnimNotifyMA::SetGroupType(EEMNotifyGroupType::Sound);
#endif
}

FString UKMAnimNotify_Sound::GetNotifyName_Implementation() const
{
	FString notifyName = GetClass()->GetDisplayNameText().ToString();
	
	if (IsValid(Sound))
	{
		notifyName += FString::Printf(TEXT("-'%s'"), *Sound->GetName());
	}
	
	return notifyName;
}

USoundBase* UKMAnimNotify_Sound::GetUsedSound(const USceneComponent* ownerComponent) const
{
	if (!IsValid(ownerComponent))
	{
		return nullptr;
	}
	
	if (bIsUseSoundSet)
	{
		AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerComponent->GetOwner());
		if (IsValid(ownerCharacter))
		{
			return ownerCharacter->GetSoundTag(SoundTag);
		}
	}
	
	return Sound; 
}

void UKMAnimNotify_Sound::PlaySound(USceneComponent* ownerComponent)
{
	if (!IsValid(ownerComponent) || !IsValid(ownerComponent->GetWorld()))
	{
		return;
	}
	
	USoundBase* usedSound = GetUsedSound(ownerComponent);
	if (!IsValid(usedSound))
	{
		return;
	}
	if (!usedSound->IsOneShot())
	{
		UE_LOG(LogAudio, Warning, TEXT("PlaySound notify: Anim %s tried to play a sound asset which is not a one-shot: '%s'. Spawning suppressed."), *GetNameSafe(GetContainingAsset()), *GetNameSafe(usedSound));
		return;
	}

#if WITH_EDITORONLY_DATA
	UWorld* world = ownerComponent->GetWorld();
	if (bPreviewIgnoreAttenuation && world->WorldType == EWorldType::EditorPreview)
	{
		UGameplayStatics::PlaySound2D(world, usedSound, VolumeMultiplier, PitchMultiplier);
	}
	else
#endif
	{
		if (bFollow)
		{
			UGameplayStatics::SpawnSoundAttached(usedSound, ownerComponent, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(ownerComponent->GetWorld(), usedSound, ownerComponent->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
		}
	}
}

void UKMAnimNotify_Sound::Notify(USkeletalMeshComponent* meshComp, UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference)
{
	PlaySound(meshComp);
}

void UKMAnimNotify_Sound::NotifyEx(AActor* actor, UEMMartialArts* martialArts, const FAnimNotifyEventReference& eventReference)
{
	if (IsValid(actor))
	{
		PlaySound(actor->GetRootComponent());
	}
}

#if WITH_EDITOR
void UKMAnimNotify_Sound::ValidateAssociatedAssets()
{
	static const FName NAME_AssetCheck("AssetCheck");

	if (IsValid(Sound) && !Sound->IsOneShot())
	{
		UObject* ContainingAsset = GetContainingAsset();

		FMessageLog AssetCheckLog(NAME_AssetCheck);

		const FText MessageLooping = FText::Format(
			NSLOCTEXT("AnimNotify", "Sound_ShouldNotLoop", "Sound {0} used in anim notify for asset {1} is set to looping, but the slot is a one-shot (it won't be played to avoid leaking an instance per notify)."),
			FText::AsCultureInvariant(Sound->GetPathName()),
			FText::AsCultureInvariant(ContainingAsset->GetPathName()));
		AssetCheckLog.Warning()
			->AddToken(FUObjectToken::Create(ContainingAsset))
			->AddToken(FTextToken::Create(MessageLooping));

		if (GIsEditor)
		{
			AssetCheckLog.Notify(MessageLooping, EMessageSeverity::Warning, true);
		}
	}
}

void UKMAnimNotify_Sound::PostEditChangeProperty(AActor* ownerActor, FPropertyChangedEvent& propertyChangedEvent)
{
	Super::PostEditChangeProperty(ownerActor, propertyChangedEvent);
	if (propertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimNotify_Sound, SoundTag) ||
		propertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UKMAnimNotify_Sound, bIsUseSoundSet))
	{
		if (bIsUseSoundSet)
		{
			if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(ownerActor))
			{
				UKMCharacterInstance* ownerCharacteInstancer = Cast<UKMCharacterInstance>(ownerCharacter->GetCharacterInstance());
				if (!IsValid(ownerCharacteInstancer))
				{
					Sound = ownerCharacter->GetSoundTag(SoundTag);
				}
			}
		}
	}
}
#endif


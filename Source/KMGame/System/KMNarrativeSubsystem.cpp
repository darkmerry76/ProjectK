#include "KMNarrativeSubsystem.h"
#include "Character/KMCharacter.h"
#include "Components/AudioComponent.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMVoicePDA.h"
#include "Sound/SoundCue.h"
#include "Tables/Generated/KMTable_Narrative_Director.h"
#include "Tables/Generated/KMTable_Narrative_Sequence.h"
#include "Tables/Generated/KMTable_Narrative_Dialog.h"
#include "Tables/Generated/KMTable_Narrative_Prologue.h"
#include <Tables/Generated/KMTable_Narrative_Movie.h>
#include "Tables/Generated/KMTable_Narrative_Event.h"
#include "Nerrative/KMNarrativeNode.h"
#include "Nerrative/KMNarrativeNodeDialog.h"
#include "Nerrative/KMNarrativeNodeDirector.h"
#include "Nerrative/KMNarrativeNodeMovie.h"
#include "Nerrative/KMNarrativeNodePrologue.h"
#include "Nerrative/KMNarrativeNodeSequence.h"
#include "Nerrative/KMNarrativeNodeEvent.h"

UKMNarrativeSubsystem* UKMNarrativeSubsystem::GetNarrativeSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMNarrativeSubsystem>(GetSubsystem(worldContextObject, UKMNarrativeSubsystem::StaticClass()));
}

void UKMNarrativeSubsystem::Initialize()
{
	Super::Initialize();

	const TMap<FName, FKMTable_NarrativeRow*>& nerrativeRow = FEMDataTableHelper::Get().GetRowMap<FKMTable_NarrativeRow>();
	for (auto narrativeItr = nerrativeRow.CreateConstIterator(); narrativeItr; ++narrativeItr)
	{
		FString tableRowName = narrativeItr.Key().ToString();
		TArray<FString> arrayNames;
		tableRowName.ParseIntoArray(arrayNames, TEXT("$"));
		check(!arrayNames.IsEmpty());

		TArray<const FKMTable_NarrativeRow*>* arrayRow = NarrativeTableGroups.Find(*arrayNames[0]);
		if (arrayRow)
		{
			arrayRow->Emplace(narrativeItr->Value);
		}
		else
		{
			NarrativeTableGroups.Emplace(arrayNames[0], TArray<const FKMTable_NarrativeRow*>()).Emplace(narrativeItr->Value);	
		}
	}

	if (IsValid(VoicePDA))
	{
		for (auto voiceItr = VoicePDA->SoundWaves.CreateIterator(); voiceItr; ++voiceItr)
		{
			if (!IsValid(*voiceItr))
			{
				continue;
			}
			VoiceWaves.Emplace(voiceItr->GetFName(), *voiceItr);
		}
	}
}

void UKMNarrativeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKMNarrativeSubsystem::OnWorldCleanup(UWorld* cleaupWorld, bool bSessionEnded, bool bCleanupResources)
{
	NodeInstances.Empty();
}

bool UKMNarrativeSubsystem::StartDirector(const FName directorId)
{
	BranchNode(nullptr, directorId);
	return true;
}

template<typename _TL>
_TL* UKMNarrativeSubsystem::CastNode(UKMNarrativeNode* node) const
{
	if (!IsValid(node))
	{
		return nullptr;
	}
	if (_TL* castNode = Cast<_TL>(node))
	{
		return castNode;
	}
	return CastNode<_TL>(node->GetParent());
}

void UKMNarrativeSubsystem::BranchNode(UKMNarrativeNode* fromNode, FName toNodeId)
{
	if (toNodeId == NAME_None)
	{
		return;
	}

	TArray<const FKMTable_NarrativeRow*>* narrativeTableGroup = NarrativeTableGroups.Find(toNodeId);
	check(narrativeTableGroup && !narrativeTableGroup->IsEmpty());

	UKMNarrativeNode* newNarrativeNode = nullptr;
	
	const FKMTable_NarrativeRow* narrativeRow = (*narrativeTableGroup)[0];
	check(narrativeRow);
	
	if (narrativeRow->NodePda.PdaKey != NAME_None)
	{
		UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
		check(IsValid(assetManager));

		UClass* narrativeNodeClass = Cast<UClass>(assetManager->GetAsset(narrativeRow->NodePda));
		check(IsValid(narrativeNodeClass));

		newNarrativeNode = NewObject<UKMNarrativeNode>(this, narrativeNodeClass);
	}
	else
	{
		if (const FKMTable_Narrative_DirectorRow* directorTableRow = CastRow<FKMTable_Narrative_DirectorRow>(narrativeRow))
		{
			newNarrativeNode = NewObject<UKMNarrativeNodeDirector>(this);
		}
		if (const FKMTable_Narrative_SequenceRow* sequenceTableRow = CastRow<FKMTable_Narrative_SequenceRow>(narrativeRow))
		{
			newNarrativeNode = NewObject<UKMNarrativeNodeSequence>(this);
		}
		else if (const FKMTable_Narrative_DialogRow* dialogTableRow = CastRow<FKMTable_Narrative_DialogRow>(narrativeRow))
		{
			newNarrativeNode = NewObject<UKMNarrativeNodeDialog>(this);
		}
		else if (const FKMTable_Narrative_PrologueRow* prologueTableRow = CastRow<FKMTable_Narrative_PrologueRow>(narrativeRow))
		{
			newNarrativeNode = NewObject<UKMNarrativeNodePrologue>(this);
		}
		else if (const FKMTable_Narrative_MovieRow* movieTableRow = CastRow<FKMTable_Narrative_MovieRow>(narrativeRow))
		{
			newNarrativeNode = NewObject<UKMNarrativeNodeMovie>(this);
		}
		else if (const FKMTable_Narrative_EventRow* eventTableRow = CastRow<FKMTable_Narrative_EventRow>(narrativeRow))
		{
			newNarrativeNode = NewObject<UKMNarrativeNodeEvent>(this);
		}
	}
	check(newNarrativeNode);

	for (auto narrativeItr = narrativeTableGroup->CreateConstIterator(); narrativeItr; ++narrativeItr)
	{
		newNarrativeNode->TableGroup.Emplace(*narrativeItr);
	}

	if (newNarrativeNode->IsA<UKMNarrativeNodeTask>())
	{
		if (UKMNarrativeNodeSequence* narrativeNodeSequence = CastNode<UKMNarrativeNodeSequence>(fromNode))
		{
			narrativeNodeSequence->AddChild(newNarrativeNode);
		}
	}
	else if (newNarrativeNode->IsA<UKMNarrativeNodeSequence>())
	{
		if (UKMNarrativeNodeDirector* narrativeNodeDirector = CastNode<UKMNarrativeNodeDirector>(fromNode))
		{
			narrativeNodeDirector->AddChild(newNarrativeNode);
		}
	}
	
	if (newNarrativeNode->IsA<UKMNarrativeNodeDirector>())
	{
		NodeInstances.Emplace(newNarrativeNode);
	}
	
	newNarrativeNode->BranchDelegate.BindUObject(this, &UKMNarrativeSubsystem::BranchNode);
	newNarrativeNode->Activate();
	
	OnBranch(fromNode, newNarrativeNode);
}

TStatId UKMNarrativeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKMNarrativeNodeTask, STATGROUP_Tickables)
}

ETickableTickType UKMNarrativeSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool UKMNarrativeSubsystem::IsTickable() const
{
	return HasAnyFlags(RF_ClassDefaultObject) == false && GetWorld()->IsPaused() == false;
}

void UKMNarrativeSubsystem::Tick(float deltaTime)
{
	for (auto narrativeNodeItr = NodeInstances.CreateIterator(); narrativeNodeItr; ++narrativeNodeItr)
	{
		(*narrativeNodeItr)->Tick(deltaTime);
	}
}

USoundWave* UKMNarrativeSubsystem::GetVoice(FName voiceName) const
{
	const TObjectPtr<USoundWave>* soundWav = VoiceWaves.Find(voiceName);
	if (!soundWav || !IsValid(*soundWav))
	{
		return nullptr;
	}
	return *soundWav;
}

void UKMNarrativeSubsystem::PlayVoice(FName speakerUniqueId, FName voiceName)
{
	if (speakerUniqueId == NAME_None)
	{
		return;
	}

	USoundWave* soundWav = GetVoice(voiceName);
	if (!IsValid(soundWav))
	{
		return;
	}

	AKMCharacter* speakerCharacter = Cast<AKMCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()); 
	if (speakerUniqueId == TEXT("player"))
	{
		speakerCharacter = Cast<AKMCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}

	UAudioComponent* audioComponent = speakerCharacter->FindComponentByClass<UAudioComponent>();
	if (!IsValid(audioComponent))
	{
		audioComponent = Cast<UAudioComponent>(speakerCharacter->AddComponentByClass(UAudioComponent::StaticClass(), false, FTransform::Identity, false));
	}
	check(IsValid(audioComponent));
	
	audioComponent->SetSound(VoicePDA->NarrativeSoundCue);
	audioComponent->SetWaveParameter("VoiceWave", soundWav);
	audioComponent->Play();
}

void UKMNarrativeSubsystem::BroadcastEvent(FGameplayTag newTag)
{
	EventDelegate.Broadcast(newTag);
}
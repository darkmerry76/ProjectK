#include "KMNarrativeNodeMovie.h"
#include "EMMartialArts.h"
#include "EMMartialArtsComponent.h"
#include "Character/KMCharacter.h"
#include "DataAsset/KMAssetManager.h"
#include "GameObject/KMCharacterInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include <Tables/Generated/KMTable_Narrative_Movie.h>

UKMNarrativeNodeMovie::UKMNarrativeNodeMovie(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMNarrativeNodeMovie::Activate()
{
	Super::Activate();
	
	check(TableGroup.Num() == 1);

	MovieTableRow = CastRow<FKMTable_Narrative_MovieRow>(TableGroup[0]);
	check(MovieTableRow);

	BeginTime = GetWorld()->GetTimeSeconds();

	UObject* movieAsset = UKMAssetManager::GetAssetManager()->GetAsset(MovieTableRow->MovieAsset);
	if (UEMMartialArts* martialArts = Cast<UEMMartialArts>(movieAsset))
	{
		if (UKMCharacterInstance* authCharacterInstance = UKMGameObjectSubsystem::GetGameObjectSubsystem(this)->GetAuthCharacterInstance())
		{
			if (AKMCharacter* authCharacter = authCharacterInstance->GetCharacter())
			{
				Duration = martialArts->GetPlayLength();
				if (Duration > 0.f)
				{
					if (MovieTableRow->StartDelay > 0.f)
					{
						GetWorld()->GetTimerManager().SetTimer(MovieTimerHandle, FTimerDelegate::CreateLambda([this, authCharacter, martialArts]()
						{
							int32 martialArtsInstanceId = authCharacter->GetMartialArtsComponent()->Play(martialArts);
							martialArtsInstance = authCharacter->GetMartialArtsComponent()->GetInstance(martialArtsInstanceId);
						}), MovieTableRow->StartDelay, false);
					}
					else
					{
						int32 martialArtsInstanceId = authCharacter->GetMartialArtsComponent()->Play(martialArts);
						martialArtsInstance = authCharacter->GetMartialArtsComponent()->GetInstance(martialArtsInstanceId);
					}
				}
			}
		}
	}
}

void UKMNarrativeNodeMovie::Deactivate()
{
	Super::Deactivate();
}

void UKMNarrativeNodeMovie::CopyFrom(class UKMNarrativeNode* source)
{
	Super::CopyFrom(source);
}

bool UKMNarrativeNodeMovie::IsEnd() const
{
	if (GetWorld()->GetTimeSeconds() - BeginTime >= Duration + MovieTableRow->StartDelay + MovieTableRow->FadeOutTime + MovieTableRow->EndDelay)
	{
		return true;
	}
	
	return false;
}

void UKMNarrativeNodeMovie::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsEnd())
	{
		BranchDelegate.Execute(this, MovieTableRow->Branch);
	}
}
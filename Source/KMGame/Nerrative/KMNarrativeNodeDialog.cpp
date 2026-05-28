#include "KMNarrativeNodeDialog.h"

#include "DataAsset/KMAssetManager.h"
#include "System/KMNarrativeSubsystem.h"
#include "System/KMUiSubsystem.h"
#include "Tables/Generated/KMTable_Narrative_Task_Dialog.h"
#include "Ui/Component/KMCinematicWidget.h"
#include "Ui/Component/KMNarrativeWidget.h"

FKMNarrativeDialogSchedule::FKMNarrativeDialogSchedule(const FKMTable_Narrative_Task_DialogRow* tableRow) :
	TableRow(tableRow)
{
}

void FKMNarrativeDialogSchedule::Start()
{
	StatusChangedDelegate.ExecuteIfBound(this, Status);
}

void FKMNarrativeDialogSchedule::Stop()
{
	StatusChangedDelegate.Unbind();
}

void FKMNarrativeDialogSchedule::ChangeStatus(EKMNarrativeDialogStatus newStatus)
{
	Status = newStatus;
	StatusChangedDelegate.ExecuteIfBound(this, newStatus);
}

void FKMNarrativeDialogSchedule::Tick(float deltaTime)
{
	ElipsedTime += deltaTime;
	
	if (Status == EKMNarrativeDialogStatus::FadeIn && ElipsedTime > TableRow->FadeInTime)
	{
		ChangeStatus(EKMNarrativeDialogStatus::Idle);
	}
	else if (Status == EKMNarrativeDialogStatus::Idle && ElipsedTime > (TableRow->FadeInTime + Duration))
	{
		ChangeStatus(EKMNarrativeDialogStatus::FadeOut);
	}
	else if (Status == EKMNarrativeDialogStatus::FadeOut && ElipsedTime > (TableRow->FadeOutTime + Duration + TableRow->FadeOutTime))
	{
		ChangeStatus(EKMNarrativeDialogStatus::Stop);
	}
}

UKMNarrativeNodeDialog::UKMNarrativeNodeDialog(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMNarrativeNodeDialog::CopyFrom(class UKMNarrativeNode* source)
{
	UKMNarrativeNodeDialog* sourceNoedDialog = Cast<UKMNarrativeNodeDialog>(source);
	
	Super::CopyFrom(source);
}

void UKMNarrativeNodeDialog::ScheduleStatusChanged(FKMNarrativeDialogSchedule* Schedule, EKMNarrativeDialogStatus newStatus)
{
	if (newStatus == EKMNarrativeDialogStatus::Idle)
	{
		UKMUiSubsystem* uiSubsystem = UKMUiSubsystem::GetUiSubsystem(this);
		UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
		if (!Schedule->TableRow->Message.IsEmpty())
		{
			uiSubsystem->PrintNarrativeMessage(Schedule->TableRow->Message, Schedule->TableRow->MessageColor, Schedule->TableRow->MessageClear);
		}

		if (Schedule->TableRow->VoiceId != NAME_None)
		{
			UKMNarrativeSubsystem* narrativeSubsystem = UKMNarrativeSubsystem::GetNarrativeSubsystem(this);
			narrativeSubsystem->PlayVoice(Schedule->TableRow->Speaker, Schedule->TableRow->VoiceId);
		}

		if (Schedule->TableRow->CinematicWidget.IsValid())
		{
			UClass* cinematicWidgetClass = Cast<UClass>(assetManager->GetAsset(Schedule->TableRow->CinematicWidget));
			if (IsValid(cinematicWidgetClass))
			{
				UKMCinematicWidget* CinematicWidget = uiSubsystem->DrawCienmaticImage(cinematicWidgetClass);
				CinematicWidgets.Emplace(CinematicWidget);
			}
		}
	}
	else if (newStatus == EKMNarrativeDialogStatus::Stop)
	{
		Scheduler.Remove(Schedule->AsShared());
	}
	
	OnScheduleStatusChanged(newStatus);
}

bool UKMNarrativeNodeDialog::IsEnd() const
{
	return Scheduler.IsEmpty();
}

void UKMNarrativeNodeDialog::Activate()
{
	Super::Activate();

	UKMNarrativeSubsystem* narrativeSubsystem = UKMNarrativeSubsystem::GetNarrativeSubsystem(this);

	for (auto narrativeDialogItr = TableGroup.CreateConstIterator(); narrativeDialogItr; ++narrativeDialogItr)
	{
		TSharedPtr<FKMNarrativeDialogSchedule> newDialogSchedule =
			MakeShared<FKMNarrativeDialogSchedule>(CastRow<FKMTable_Narrative_Task_DialogRow>(*narrativeDialogItr));

		if (USoundWave* voiceWav = narrativeSubsystem->GetVoice(newDialogSchedule->TableRow->VoiceId))
		{
			newDialogSchedule->Duration = voiceWav->GetDuration() > newDialogSchedule->TableRow->Duration ? voiceWav->GetDuration() : newDialogSchedule->TableRow->Duration;
		}
		else
		{
			newDialogSchedule->Duration = newDialogSchedule->TableRow->Duration;
		}
		
		Scheduler.Emplace(newDialogSchedule);
		newDialogSchedule->StatusChangedDelegate.BindUObject(this, &ThisClass::ScheduleStatusChanged);
		newDialogSchedule->Start();
	}
}

void UKMNarrativeNodeDialog::Deactivate()
{
	Super::Deactivate();	
}

void UKMNarrativeNodeDialog::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	bool isScheduleEmpty = Scheduler.IsEmpty();
	for (auto scheduleItr = Scheduler.CreateIterator(); scheduleItr; ++scheduleItr)
	{
		(*scheduleItr)->Tick(deltaTime);
	}

	if (IsEnd() && !isScheduleEmpty)
	{
		for (auto cinematicItr = CinematicWidgets.CreateIterator(); cinematicItr; ++cinematicItr)
		{
			(*cinematicItr)->RemoveFromParent();
		}
		CinematicWidgets.Empty();
	
		for (auto tableItr = TableGroup.CreateConstIterator(); tableItr; ++tableItr)
		{
			const FKMTable_Narrative_Task_DialogRow* narrativeDialogTable = CastRow<FKMTable_Narrative_Task_DialogRow>(*tableItr);
			check(narrativeDialogTable);

			BranchDelegate.Execute(this, narrativeDialogTable->Branch);
			break;
		}
	}
}
#pragma once

#include "CoreMinimal.h"
#include "KMNarrativeNodeTask.h"
#include "KMNarrativeNodeDialog.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EKMNarrativeDialogStatus : uint8
{
	FadeIn,
	Idle,
	FadeOut,
	Stop,
};

DECLARE_DELEGATE_TwoParams(FKMNarrativeDialogStatusChangedDelegate, struct FKMNarrativeDialogSchedule* Schedule, EKMNarrativeDialogStatus newStatus);

struct FKMNarrativeDialogSchedule : public TSharedFromThis<FKMNarrativeDialogSchedule>
{
	FKMNarrativeDialogSchedule(const struct FKMTable_Narrative_Task_DialogRow* tableRow);
	
	void Start();
	void Stop();
	void ChangeStatus(EKMNarrativeDialogStatus newStatus);
	void Tick(float deltaTime);
	
	float ElipsedTime = 0.f;
	float Duration = 0.f;

	EKMNarrativeDialogStatus Status = EKMNarrativeDialogStatus::FadeIn;
	FKMNarrativeDialogStatusChangedDelegate StatusChangedDelegate;
	
	const struct FKMTable_Narrative_Task_DialogRow* TableRow = nullptr;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMNarrativeNodeDialog : public UKMNarrativeNodeTask
{
	GENERATED_UCLASS_BODY()
	
protected:
	TArray<TSharedPtr<FKMNarrativeDialogSchedule>> Scheduler;
	
	UPROPERTY()
	TArray<class UKMCinematicWidget*> CinematicWidgets;
	
public:
	virtual void CopyFrom(class UKMNarrativeNode* source) override;

protected:
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual bool IsEnd() const override;
	
	virtual void Tick(float deltaTime);
	
	void ScheduleStatusChanged(FKMNarrativeDialogSchedule* Schedule, EKMNarrativeDialogStatus newStatus);

	UFUNCTION(BlueprintImplementableEvent)
	void OnScheduleStatusChanged(EKMNarrativeDialogStatus newStatus);
};
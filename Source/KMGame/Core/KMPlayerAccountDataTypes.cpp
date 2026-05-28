#include "KMPlayerAccountDataTypes.h"

#include "Tables/Generated/KMTable_Stage.h"

void FKMAccountInfo::SetStageInfo(const FName& stageId, const FName& chapterId, int32 waveLevel)
{
	if (StageList.Contains(stageId))
	{
		auto& stageInfo = StageList[stageId];
		stageInfo.StageId = stageId;
		stageInfo.ChapterId = chapterId;
		stageInfo.WaveLevel = waveLevel;
		StageList[stageId] = stageInfo;
	}
	else
	{
		FKMAccountStageInfo stageInfo = FKMAccountStageInfo();
		stageInfo.StageId = stageId;
		stageInfo.ChapterId = chapterId;
		stageInfo.WaveLevel = waveLevel;
		StageList.Add(stageId, stageInfo);
	}
}

FKMAccountStageInfo* FKMAccountInfo::GetStageInfo(FName stageId)
{
	if (StageList.Contains(stageId))
	{
		return &StageList[stageId];
	}

	// 한번도 진행하지 않은 경우는 새로 추가해서 반환
	FKMAccountStageInfo stageInfo = FKMAccountStageInfo();
	stageInfo.StageId = stageId;

	auto stageData = FKMTable_StageRow::FindRowPtr(stageId);
	check(stageData != nullptr);
		
	check(stageData->ChapterList.Num() > 0);
		
	auto firstChapterId = stageData->ChapterList[0];
	stageInfo.ChapterId = firstChapterId;
	stageInfo.WaveLevel = 0;
		
	StageList.Add(stageId, stageInfo);
	return &StageList[stageId];
}


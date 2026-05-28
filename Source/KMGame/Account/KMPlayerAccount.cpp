#include "KMPlayerAccount.h"

void UKMPlayerAccount::SetCurrentStageInfo(FName stageId, FName chapterId, int32 waveLevel)
{
	BaseInfo.SetStageInfo(stageId, chapterId, waveLevel);
}

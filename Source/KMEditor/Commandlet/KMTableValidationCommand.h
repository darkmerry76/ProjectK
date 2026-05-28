#pragma once

#include "CoreMinimal.h"
#include "KMCommandletType.h"
#include "KMTableValidationCommand.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogValidation, Log, All)

UCLASS()
class KMEDITOR_API UKMTableValidationCommand : public UKMCommand
{
public:
	GENERATED_BODY()

	virtual FName GetCommandParamName() const override { return TEXT("TableValidation"); };
	
	virtual int32 Main(const TArray<FString>& params) override;
	
	static bool IsValidationCharacterTable(const FName& Id);
	static bool IsValidationStatTable(const FName& statId);
	static bool IsValidationStatPerLevelTable(const FName& statPerLevelId);
	static bool IsValidationSkillTable(const FName& skillId, int32 level);
	static bool IsValidationSkillEffectTable(const FName& skillEffectId);

	static bool IsValidationStageTable(const FName& stageId);
	static bool IsValidationChapterTable(const FName& chapterId);
	static bool IsValidationWaveTable(const FName& waveId);
	static bool IsValidationRewardTable(const FName& rewardId);

	static bool IsValidationDropRateTable(const FName& dropRateId);
};
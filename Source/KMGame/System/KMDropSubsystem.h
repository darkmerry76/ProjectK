#pragma once

#include <Tables/Generated/KMTable_Drop_InGameEvent.h>
#include <Tables/Generated/KMTable_Drop_InGameSkill.h>
#include <Tables/Generated/KMTable_Drop_Item.h>

#include "CoreMinimal.h"
#include "System/EMGameInstanceSubsystem.h"
#include "UObject/Object.h"
#include "KMDropSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMDropSubsystem : public UEMGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMDropSubsystem* GetDropSubsystem(const UObject* worldContextObject);

private:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

public:
	void RequestCheckDrop(const FName& dropItemDataId);

private:
	void CheckDropItem(const FKMTable_Drop_ItemRow* dropItemData);
	void CheckDropInGameEvent(const FKMTable_Drop_InGameEventRow* dropEvent);
	void CheckDropInGameSkill(const FKMTable_Drop_InGameSkillRow* dropSkill);
};

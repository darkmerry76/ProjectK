#include "KMDropSubsystem.h"
#include <Tables/Generated/KMTable_Drop.h>
#include "KMSkillSubsystem.h"

UKMDropSubsystem* UKMDropSubsystem::GetDropSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMDropSubsystem>(GetSubsystem(worldContextObject, UKMDropSubsystem::StaticClass()));
}

void UKMDropSubsystem::Initialize()
{
	Super::Initialize();
}

void UKMDropSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKMDropSubsystem::RequestCheckDrop(const FName& dropItemDataId)
{
	auto dropData = FKMTable_DropRow::FindRowPtr(dropItemDataId);
	if (dropData == nullptr) return;

	if (auto dropItemData = FKMTable_Drop_ItemRow::FindRowPtr(dropItemDataId))
	{
		CheckDropItem(dropItemData);
	}
	else if (auto dropInGameEvent = FKMTable_Drop_InGameEventRow::FindRowPtr(dropItemDataId))
	{
		CheckDropInGameEvent(dropInGameEvent);
	}
	else if (auto dropSkill = FKMTable_Drop_InGameSkillRow::FindRowPtr(dropItemDataId))
	{
		CheckDropInGameSkill(dropSkill);
	}
}

void UKMDropSubsystem::CheckDropItem(const FKMTable_Drop_ItemRow* dropItemData)
{
}

void UKMDropSubsystem::CheckDropInGameEvent(const FKMTable_Drop_InGameEventRow* dropEvent)
{
}

void UKMDropSubsystem::CheckDropInGameSkill(const FKMTable_Drop_InGameSkillRow* dropSkill)
{
}
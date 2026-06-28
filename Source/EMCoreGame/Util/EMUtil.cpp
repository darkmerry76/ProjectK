#include "EMUtil.h"

bool UEMUtil::IsGameWorld(UObject* worldContextObject)
{
	if (!IsValid(worldContextObject) || !IsValid(worldContextObject->GetWorld()))
	{
		return false;
	}
	return worldContextObject->GetWorld()->IsGameWorld();
}
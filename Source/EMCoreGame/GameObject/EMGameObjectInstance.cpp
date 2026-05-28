#include "EMGameObjectInstance.h"

UEMGameObjectInstance::UEMGameObjectInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UEMGameObjectInstance::SetId(int32 newId)
{
	Id = newId;
}

int32 UEMGameObjectInstance::GetId() const
{
	return Id;
}
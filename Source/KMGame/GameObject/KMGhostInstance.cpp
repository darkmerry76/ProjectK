#include "KMGhostInstance.h"
#include "Stat/KMGhostStatModifier.h"

UKMGhostInstance::UKMGhostInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	StatModifierClass = UKMGhostStatModifier::StaticClass();
}

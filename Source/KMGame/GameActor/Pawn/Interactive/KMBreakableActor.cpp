#include "KMBreakableActor.h"

AKMBreakableActor::AKMBreakableActor(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
}

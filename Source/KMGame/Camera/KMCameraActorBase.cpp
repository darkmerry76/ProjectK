#include "KMCameraActorBase.h"
#include "Layer/KMCameralayerBase.h"

AKMCameraActorBase::AKMCameraActorBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
}

void AKMCameraActorBase::BeginPlay()
{
	Super::BeginPlay();

	for (auto layer : CameraLayers)
	{
		layer.Value->SetCameraActor(this);
		layer.Value->Initalize();
	}
}

void AKMCameraActorBase::SetTargetActor(AActor* newTargetActor)
{
	TargetActor = newTargetActor;
}

AActor* AKMCameraActorBase::GetTargetActor() const
{
	if (!TargetActor.IsValid())
	{
		return nullptr;
	}
	return TargetActor.Get();
}

USceneComponent* AKMCameraActorBase::GetCameraOffset() const
{
	return Root;
}

UKMCameralayerBase* AKMCameraActorBase::GetCameraLayer(EKMCameralayerType cameraLayerType) const
{
	const TObjectPtr<UKMCameralayerBase>* existCameraLayer = CameraLayers.Find(cameraLayerType);
	if (!existCameraLayer || !*existCameraLayer)
	{
		return nullptr;
	}
	return *existCameraLayer;
}

void AKMCameraActorBase::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}
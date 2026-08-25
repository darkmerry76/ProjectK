#pragma once

#include "CoreMinimal.h"
#include "GameObject/KMGameObjectInstance.h"
#include "KMInteractiveInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API UKMInteractiveInstance : public UKMGameObjectInstance
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<class AKMInteractiveActorBase> Interactive;
	
public:
	void SetInteractiveActor(class AKMInteractiveActorBase* newInteractiveActor);
	class AKMInteractiveActorBase* GetInteractiveActor() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay() override;
};
#pragma once

#include "CoreMinimal.h"
#include "KMGameObjectInstance.h"
#include "KMActorInstance.generated.h"

DECLARE_DELEGATE_OneParam(FKMActorInstanceDestroyedDelegate, const class UKMActorInstance* destroyedActorInstance);
UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMActorInstance : public UKMGameObjectInstance
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay() override;

	void SetActor(class AActor* newActor);
	class AActor* GetActor() const;

	UFUNCTION(BlueprintCallable)
	void OnActorDestroyed(class AActor* destroyedActor);

	void ClearActor();

	FKMActorInstanceDestroyedDelegate ActorInstanceDestroyedDelegate; 

	int32 CreatedIndex;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "GameObjectInstance")
	TObjectPtr<class AActor> Actor = nullptr;
};
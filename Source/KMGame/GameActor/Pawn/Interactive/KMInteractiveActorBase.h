#pragma once

#include "CoreMinimal.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "KMInteractiveActorBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMInteractiveActorBase : public AActor, public IKMPawnInterface
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> RootScene;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMMartialArtsComponent> MartialArtsComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UKMInteractiveInstance> InteractiveInstance;

public:
	virtual void PossessedByGameObjectInstance(class UKMGameObjectInstance* newGameObjectInstance) override;

	UFUNCTION(BlueprintPure)
	class UKMInteractiveInstance* GetInteractiveInstance() const;

	UFUNCTION(BlueprintPure)
	class UKMMartialArtsComponent* GetMartialArtsComponent() const;

protected:
	virtual class UKMGameObjectInstance* GetGameObjectInstance() const override;
};
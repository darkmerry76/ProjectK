#pragma once

#include "CoreMinimal.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "KMInteractiveActorBase.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMInteractiveActorBase : public APawn, public IKMPawnInterface
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UKMMoveShapeComponent> MoveShapeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UKMPawnMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UKMAttachedBlendingComponent> AttachedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMCurveWarpingComponent> CurveWarpingComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMMartialArtsComponent> MartialArtsComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UKMInteractiveInstance> InteractiveInstance;

public:
	virtual void PossessedByGameObjectInstance(class UKMGameObjectInstance* newGameObjectInstance) override;
	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag) override;
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnImpact")
	void Receive_OnImpact(const FName& skillEffectName, const FVector& hitClosestPoint, const FName& hitTag);

	virtual void OnDeath() override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDeath")
	void Receive_OnDeath();

	UFUNCTION(BlueprintPure)
	class UKMInteractiveInstance* GetInteractiveInstance() const;

	UFUNCTION(BlueprintPure)
	virtual class UPrimitiveComponent* GetMovementShapeComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual class UKMMartialArtsComponent* GetMartialArtsComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual class UKMCurveWarpingComponent* GetCurveWarpingComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual class UKMGameObjectInstance* GetGameObjectInstance() const override;

	UFUNCTION(BlueprintPure)
	virtual FBoxSphereBounds GetMasterBounds() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMAttachedBlendingComponent* GetAttachedComponent() const;

	UFUNCTION(BlueprintCallable)
	virtual void Crarry(class UKMGameObjectInstance* carryedGameObjectInstance) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Crarried(class UKMGameObjectInstance* carryGameObjectInstance) override;

protected:
	virtual class UPawnMovementComponent* GetMovementComponent() const override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnCarry")
	void Receive_OnCarry(class UKMGameObjectInstance* carryedGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnCarried")
	void Receive_OnCarried(class UKMGameObjectInstance* carryedGameObjectInstance);
};
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UMeshComponent> MasterMeshComponent;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<class UKMInteractiveInstance> InteractiveInstance;

	ECollisionResponse PawnResponse = ECollisionResponse::ECR_Block;

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
	virtual class UMeshComponent* GetMasterMeshComponent() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMAttachedBlendingComponent* GetAttachedBlendingComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual FTransform GetCarryOffsetTransform() const override;

	UFUNCTION(BlueprintPure)
	virtual class UKMCurveWarpingComponent* GetCurveWarpingComponent() const override;

	UFUNCTION(BlueprintPure)
	virtual class UKMGameObjectInstance* GetGameObjectInstance() const override;

	UFUNCTION(BlueprintPure)
	virtual FBoxSphereBounds GetMasterBounds() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMAttachedBlendingComponent* GetAttachedComponent() const;

	UFUNCTION(BlueprintCallable)
	virtual void StartCrarry(class UKMGameObjectInstance* carriedGameObjectInstance) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void StartCrarried(class UKMGameObjectInstance* carryGameObjectInstance) override;

	UFUNCTION(BlueprintCallable)
	virtual void ComplatePutdown(class UKMGameObjectInstance* putDownedGameObjectInstance) override;

	UFUNCTION(BlueprintCallable)
	virtual void ComplatePutdowned(class UKMGameObjectInstance* putDownGameObjectInstance) override;

protected:
	virtual class UPawnMovementComponent* GetMovementComponent() const override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnStartCarry")
	void Receive_OnStartCarry(class UKMGameObjectInstance* carriedGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnCarried")
	void Receive_OnStartCarried(class UKMGameObjectInstance* carryGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnComplatePutdown")
	void Receive_OnComplatePutdown(class UKMGameObjectInstance* downedGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnComplatePutdowned")
	void Receive_OnComplatePutdowned(class UKMGameObjectInstance* downGameObjectInstance);
};
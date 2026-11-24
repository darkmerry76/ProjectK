#pragma once

#include "CoreMinimal.h"
#include "MoverSimulationTypes.h"
#include "GameObject/KMCharacterInstance.h"
#include "KMPawnBase.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API AKMPawnBase : public APawn, public IMoverInputProducerInterface
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UKMMoveShapeComponent> MoveShapeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UKMPawnMoverComponent> MoverComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UKMAttachedBlendingComponent> AttachedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMCurveWarpingComponent> CurveWarpingComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UKMMartialArtsComponent> MartialArtsComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UMeshComponent> PlacementMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag TakeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag TakenTag;

	UPROPERTY()
	TWeakObjectPtr<class UKMGameObjectInstance> GameObjectInstance;
	
	UPROPERTY(BlueprintReadOnly)
	FHitResult LastWallHitResult;

	UPROPERTY()
	FCharacterDefaultInputs DefaultInputs;

public:
	UFUNCTION(BlueprintPure)
	virtual class UKMMoveShapeComponent* GetMovementShapeComponent() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMMartialArtsComponent* GetMartialArtsComponent() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMWidgetComponent* GetWidgetComponent() const;
	
	UFUNCTION(BlueprintPure)
	virtual class UKMPawnMoverComponent* GetMoverComponent() const;

	UFUNCTION(BlueprintPure)
	virtual class UMeshComponent* GetPlacementMeshComponent() const;
	
	UFUNCTION(BlueprintPure)
	virtual class UKMAttachedBlendingComponent* GetAttachedBlendingComponent() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMCurveWarpingComponent* GetCurveWarpingComponent() const;

	UFUNCTION(BlueprintPure)
	virtual class UKMGameObjectInstance* GetGameObjectInstance() const;

	UFUNCTION(BlueprintPure)
	virtual bool IsAir() const;

	UFUNCTION(BlueprintPure)
	virtual bool IsLand() const;
	
	virtual FTransform GetCarryOffsetTransform_Implementation() const;

	virtual void SetMovementMode(EMovementMode newMovementMode, uint8 newCustomMode = 0);

	virtual const FHitResult GetLatestWallHitResult_Implementation() const;

	virtual void Landed(const FHitResult& hitResult);
	virtual void LandHit(const FHitResult& HitResult);
	virtual void WallHit(const FHitResult& HitResult);
	virtual void CeilingHit(const FHitResult& HitResult);

	virtual void OnImpact(const TSharedPtr<class FKMSkillEffectInstance>& skillEffectInstance, const FVector& hitClosestPoint, const FName& hitTag);

	UFUNCTION(BlueprintImplementableEvent)
	void Receive_OnImpact(const FName& skillEffectName, const FVector& hitClosestPoint, const FName& hitTag);
	
	virtual void OnDeath();
	
	UFUNCTION(BlueprintImplementableEvent)
	void Receive_OnDeath();

	UFUNCTION(BlueprintCallable)
	virtual void StartCrarry(class UKMGameObjectInstance* carriedGameObjectInstance);
	
	UFUNCTION(BlueprintCallable)
	virtual void StartCrarried(class UKMGameObjectInstance* carryGameObjectInstance);

	UFUNCTION(BlueprintCallable)
	virtual void ComplatePutdown(class UKMGameObjectInstance* putDownedGameObjectInstance);

	UFUNCTION(BlueprintCallable)
	virtual void ComplatePutdowned(class UKMGameObjectInstance* putDownGameObjectInstance);

	virtual FTransform GetApproachPullPoint(EKMApproachPullPointType approachPullPointType = EKMApproachPullPointType::Auto) const;

	virtual void PossessedByGameObjectInstance(class UKMGameObjectInstance* newGameObjectInstance);

	virtual void StartJump();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent)
	float GetScaledRadius() const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent)
	float GetScaledHalfHeight() const;

	UFUNCTION(BlueprintCallable)
	virtual void ApplyCapsuleFootAlignment(float addFootHeight);
	
	UFUNCTION(BlueprintCallable)
	virtual void RestoreCapsuleFootAlignment();

	UFUNCTION(BlueprintCallable)
	void SetGroundSpeed(float newSpeed);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void PostActorCreated() override;
	virtual void AddMovementInput(FVector worldDirection, float scaleValue = 1.f, bool bForce = false) override;

	virtual void ProduceInput_Implementation(int32 simTimeMs, FMoverInputCmdContext& inputCmdResult) override;

	FVector ConsumeInputVector();

	float PrevDirection = 0.f;
	
	UFUNCTION()
	virtual void OnMoverLanded(const FName& nextMovementModeName, const FHitResult& hitResult);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnStartCarry")
	void Receive_OnStartCarry(class UKMGameObjectInstance* carryGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnStartCarried")
	void Receive_OnStartCarried(class UKMGameObjectInstance* carryGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnComplatePutdown")
	void Receive_OnComplatePutdown(class UKMGameObjectInstance* downGameObjectInstance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnComplatePutdowned")
	void Receive_OnComplatePutdowned(class UKMGameObjectInstance* downGameObjectInstance);
};
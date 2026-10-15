#pragma once

#include "CoreMinimal.h"
#include "KMLockOnProcess.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMLockOnProcessBase : public UObject
{
	GENERATED_UCLASS_BODY()
protected:
	TSharedPtr<class FKMLockOnCluster> LockonTarget;
	TSharedPtr<class FKMLockOnCluster> InteractionTarget;

	bool bIsLockOnTagEnable = false;
	
public:
	virtual void Initalize();
	virtual void OnProcessing(const TArray<class AActor*>& resultActors);

	void SetLockOnTagEnable(bool bEnable);
	bool IsLockOnTagEnable() const;

	class UKMGameObjectInstance* GetOwnerGamneObject() const;

	TSharedPtr<class FKMLockOnCluster> GetLockonTarget() const;
	TSharedPtr<class FKMLockOnCluster> GetInteractionTarget() const;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMLockOnProcess : public UKMLockOnProcessBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Initalize() override;
	virtual void OnProcessing(const TArray<class AActor*>& resultActors) override;
	
	float GetScoreTarget(const class UKMGameObjectInstance* prevTarget, const class UKMGameObjectInstance* currTarget) const;

protected:
	void SetLockOnTagVisiblity(const class UKMGameObjectInstance* targetObjectInstance, bool bVisible);
};
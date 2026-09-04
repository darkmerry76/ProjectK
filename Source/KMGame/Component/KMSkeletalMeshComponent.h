#pragma once

#include "CoreMinimal.h"
#include "KMSkeletalMeshComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class KMGAME_API UKMSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<class UKMAttachedBlendingComponent>> BlendingComponentChilds;
	
public:
	void AttachBlendingComponent(class UKMAttachedBlendingComponent* newBlendingComponent);
	
protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void OnUnregister() override;
	
	virtual void FinalizeBoneTransform() override;
	virtual void SetMaterial(int32 elementIndex, UMaterialInterface* material) override;

	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
};
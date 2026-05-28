#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_NiagaraFX.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAnimNotifyState_NiagaraFX : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "Niagara System"))
	TObjectPtr<class UNiagaraSystem> Template;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float TimeDilation = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "Attached"))
	bool bIsAttached = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (AnimNotifyBoneName = "true"))
	FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector LocationOffset = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FRotator RotationOffset = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FVector Scale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "AnimNotify")
	bool bAbsoluteScale = false;

	UPROPERTY()
	class UNiagaraComponent* SpawnedEffect;
	
	FQuat RotationOffsetQuat = FQuat::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FName ParamName_SkeletaMesh = NAME_None;
	
public:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	class UNiagaraComponent* GetSpawnedEffect() const;

protected:
	virtual class UNiagaraComponent* SpawnEffect(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation);
};

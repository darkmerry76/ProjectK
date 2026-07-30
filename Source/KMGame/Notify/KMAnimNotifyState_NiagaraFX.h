#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_NiagaraFX.generated.h"

USTRUCT()
struct KMGAME_API FKMAnimNotifyState_NiagaraFXData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	TArray<class UNiagaraComponent*> NiagaraComponents;
};

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Play NiagaraFX")
class KMGAME_API UKMAnimNotifyState_NiagaraFX : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(DisplayName = "Niagara System", AllowPrivateAccess=true, DisplayAfter="GroupType"))
	TObjectPtr<class UNiagaraSystem> Template;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(AllowPrivateAccess=true, DisplayAfter="Template"))
	float TimeDilation = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(DisplayName = "Attached", AllowPrivateAccess=true, DisplayAfter="TimeDilation"))
	bool bIsAttached = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(DisplayName = "Continue", AllowPrivateAccess=true, DisplayAfter="TimeDilation"))
	bool bIsContinue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(AnimNotifyBoneName = "true", AllowPrivateAccess=true, DisplayAfter="bIsContinue"))
	FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(AllowPrivateAccess=true, DisplayAfter="SocketName"))
	FVector LocationOffset = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta=(AllowPrivateAccess=true, DisplayAfter="LocationOffset"))
	FRotator RotationOffset = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta=(AllowPrivateAccess=true, DisplayAfter="RotationOffset"))
	FVector Scale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "AnimNotify", meta=(AllowPrivateAccess=true, DisplayAfter="Scale"))
	bool bAbsoluteScale = false;

	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta=(AllowPrivateAccess=true, DisplayAfter="bAbsoluteScale"))
	TArray<FName> Tags;

	UPROPERTY()
	TMap<class USkeletalMeshComponent*, FKMAnimNotifyState_NiagaraFXData> SpawnedEffects;
	
	FQuat RotationOffsetQuat = FQuat::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FName ParamName_SkeletaMesh = NAME_None;
	
public:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual class UNiagaraComponent* SpawnEffect(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation);

protected:
	virtual FString GetNotifyName_Implementation() const override;
};

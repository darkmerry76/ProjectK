#pragma once

#include "CoreMinimal.h"
#include "KMAnimInstance.generated.h"

UCLASS(Abstract)
class KMGAME_API UKMAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	float CurrentDirection = 1.f;

	UPROPERTY(BlueprintReadWrite)
	float PrevActorDirection = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AddPelvisDirection = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LerpDirectionSpeed = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> HiddenBones;

protected:
	float NextDirection = 0.f;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaSeconds) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent) override;
#endif
	
public:
	void SetCurrentDirection(float newCurrentDirection);
	float GetCurrentDirection() const;

	void SetNextDirection(float newNextDirection);
	float GetNextDirection() const;

	const TArray<FName>& GetHiddenBones() const;	
};
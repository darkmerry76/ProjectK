#pragma once

#include "CoreMinimal.h"
#include "KMSensor.generated.h"

DECLARE_DELEGATE_OneParam(FKMSensorResultDelegate, const TArray<AActor*>& resultActors)

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class KMGAME_API UKMSensor : public UObject
{
	GENERATED_UCLASS_BODY()

protected:
	FTimerHandle TickHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IntervalSeconds = 0.3f;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	FTransform CenterTransform;

public:
	FKMSensorResultDelegate ResultDelegate;
	
public:
	virtual void Init();
	virtual void BeginDestroy() override;
	
	void CleatTimer();
	
	UFUNCTION(BlueprintCallable)
	virtual void DoSearch();

	UFUNCTION(BlueprintCallable)
	void SetCenterTransform(const FTransform& newCenterTransform);

	UFUNCTION(BlueprintPure)
	const FTransform& GetCenterTransform() const;

};
	
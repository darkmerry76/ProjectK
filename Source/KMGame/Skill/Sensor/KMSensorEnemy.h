#pragma once

#include "CoreMinimal.h"
#include "KMSensor.h"
#include "KMSensorEnemy.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMSensorEnemy : public UKMSensor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Radius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UClass* ActorClassFilter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float ViewAngle = 90.f;
	
public:
	virtual void DoSearch() override;
	
	void SetRadius(float newRadius);
	float GetRadius() const;
};
	
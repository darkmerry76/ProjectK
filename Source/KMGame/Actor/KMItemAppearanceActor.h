#pragma  once

#include "CoreMinimal.h"
#include "KMItemAppearanceActor.generated.h"

UCLASS()
class KMGAME_API UKMItemAppearanceInstance : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	TSubclassOf<class AKMItemAppearanceActor> AppearanceClass;

protected:
	UPROPERTY()
	TObjectPtr<class AKMItemAppearanceActor> SpawnedActor;

public:
	virtual void Spawn();
	virtual void Despawn();

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintPure)
	virtual class AKMCharacter* GetCharacter() const;
	
	UFUNCTION(BlueprintPure)
	virtual class UKMCharacterInstance* GetCharacterInstance() const;
	
	virtual void Equip(class USkeletalMeshComponent* meshComponent, FName socketName);
	virtual void Unequip(class USkeletalMeshComponent* meshComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Launch();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Launching(float alpha);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LaunchStop();

	UFUNCTION(BlueprintPure)
	class AKMItemAppearanceActor* GetSpawnedActor() const;

	UFUNCTION(BlueprintPure)
	class USkeletalMeshComponent* GetSpawnedkeletalMeshComponent() const;
};

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMItemAppearanceActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	TObjectPtr<UKMItemAppearanceInstance> CreateInstance(UObject* ownerObject) const;
};
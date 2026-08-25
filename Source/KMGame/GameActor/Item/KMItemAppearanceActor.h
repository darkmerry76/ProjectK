#pragma  once

#include "CoreMinimal.h"
#include "KMItemAppearanceActor.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMItemAppearanceInstance
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, Abstract)
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

	UFUNCTION(BlueprintCallable)
	virtual void Equip(class USkeletalMeshComponent* meshComponent, FName socketName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEquip(class USkeletalMeshComponent* meshComponent, FName socketName);

	UFUNCTION(BlueprintCallable)
	virtual void Unequip(class USkeletalMeshComponent* meshComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequip(class USkeletalMeshComponent* meshComponent);

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AKMItemAppearanceActor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMItemAppearanceActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual TObjectPtr<UKMItemAppearanceInstance> CreateInstance(UObject* ownerObject, TSubclassOf<UKMItemAppearanceInstance> instanceClass) const;

	UFUNCTION(BlueprintCallable)
	void SetVisbility(bool bIsVisibility);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Launch();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LaunchStop();

	UFUNCTION(BlueprintPure)
	class UKMCharacterInstance* GetCharacterInstance() const;

	UFUNCTION(BlueprintPure)
	class AKMCharacter* GetCharacter() const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AKMItemAppearanceChainActor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMItemAppearanceChainActor : public AKMItemAppearanceActor
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ChainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrowDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GetOverDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ChainLengthWeight = 0.99f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RingSocketName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RingRadius = 10.f;

	bool bIsCollisionCheck = false;

	FTransform PreviousTransform;

protected:
	virtual TObjectPtr<UKMItemAppearanceInstance> CreateInstance(UObject* ownerObject, TSubclassOf<UKMItemAppearanceInstance> instanceClass) const override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Launch_Implementation() override;
	virtual void LaunchStop_Implementation() override;
};
#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "Account/KMPlayerAccount.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KMUtil.generated.h"

UCLASS(Blueprintable,BlueprintType)
class KMGAME_API UKMUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static class UKMCharacterInstance* GetCharacterInstance(const class AKMCharacter* character);

	UFUNCTION(BlueprintPure)
	static class UKMCharacterInstance* GetCharacterInstanceByController(const class AAIController* controller);

	UFUNCTION(BlueprintPure)
	static void GetMeshSnapShot(class USkeletalMeshComponent* skeletalMeshComponent, FPoseSnapshot& outSnapShot);

	static bool IsInTargetType(const TArray<EKMSkillTargetType>& TargetType,
		const class UKMCharacterInstance* ownerInstance, const class UKMCharacterInstance* targetInstance);

	UFUNCTION(BlueprintCallable)
	static void PutAllRigidBodiesToSleep(class USkeletalMeshComponent* skeletalMeshComponent);

	UFUNCTION(BlueprintCallable)
	static void WakeAllRigidBodies(class USkeletalMeshComponent* skeletalMeshComponent);

	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static float GetWorldSeconds(const UObject* worldContextObject);

	UFUNCTION(BlueprintPure)
	static FString SecondsToHMSString(float seconds);

	UFUNCTION(BlueprintPure)
	static FVector2D GetCameraToDirection2D(const FVector2D direction, const class AController* controller);

	UFUNCTION(BlueprintPure)
	static float GetCircularAngle2D(const FVector2D direction);

	UFUNCTION(BlueprintPure)
	static float GetCircularAngle2D8Way(const FVector2D direction);

	UFUNCTION(BlueprintPure)
	static float FInterpToCircular(float currentValue, float targetValue, float deltaTime, float interpSpeed);

	UFUNCTION(BlueprintPure)
	static float InverseCircularDirection(float circularDirection);

	UFUNCTION(BlueprintPure)
	static float GetRelativeCircularDirectionDegrees(float baseCircularDirection, float targetCircularDirection);

	UFUNCTION(BlueprintPure)
	static EKM8WayDirection ConvertDegreesTo8WayDirection(float baseCircularDirection, float targetCircularDirection);

	UFUNCTION(BlueprintPure)
	static float Get8WayDirectionYaw(EKM8WayDirection direction);

	UFUNCTION(BlueprintPure)
	static FRotator GetYawRotation(const FVector& baseDirection, float yawAngle);

	UFUNCTION(BlueprintCallable)
	static class UKMCharacterInstance* SpawnCharacterObjectById(UObject* worldContextObject, FName characterTableId, const FTransform transform = FTransform());
	
	static class UKMCharacterInstance* SpawnCharacterObjectByTable(UObject* worldContextObject, const FKMTable_CharacterRow* characterTable, const FTransform& transform = FTransform::Identity); 
};
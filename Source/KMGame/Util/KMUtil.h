#pragma once

#include "CoreMinimal.h"
#include "Core/KMDefine.h"
#include "Account/KMPlayerAccount.h"
#include "Core/KMGameViewportClient.h"
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

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static class UKMCharacterInstance* SpawnCharacterObjectById(UObject* worldContextObject, FName characterTableId, const FTransform transform = FTransform());
	
	static class UKMCharacterInstance* SpawnCharacterObjectByTable(UObject* worldContextObject, const struct FKMTable_CharacterRow* characterTable, const FTransform& transform = FTransform::Identity);

	static class UKMCharacterInstance* SpawnBeastObjectByTable(UObject* worldContextObject, const struct FKMTable_CharacterRow* characterTable, const struct FKMTable_BeastRow* beastTable, const FTransform& transform);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void PlaySlateFade(const UObject* worldContextObject, float startAlpha = 0, float endAlpha = 1.f, float duration = 1.f, FLinearColor fadeColor = FLinearColor::Black);
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
   	static bool PlayLoadingScreen(const UObject* worldContextObject);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"), DisplayName="StopLoadingScreen")
	static bool StopLoadingScreenDynamic(const UObject* worldContextObject, FKMLoadingScreenCompleteDynamicDelegate completeDelegate, float minDelyedSeconds = 2.f);
	
	static bool StopLoadingScreen(const UObject* worldContextObject, FKMLoadingScreenCompleteDelegate completeDelegate, float minDelyedSeconds = 2.f);

	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static bool IsPlayingLoadingScreen(const UObject* worldContextObject);

	UFUNCTION(BlueprintPure, meta=(WorldContext="worldContextObject"))
	static FString GetBuildInfo(const UObject* worldContextObject);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="worldContextObject"))
	static void OpenMap(const UObject* worldContextObject, FName levelName, bool bAbsolute, FString options);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="worldContextObject"))
	static void Shutdown(const UObject* worldContextObject);

	UFUNCTION(BlueprintPure)
	static void GetMinMaxValueBlendSpace1D(const class UBlendSpace1D* blendSpace1D, float& outMin, float& outMax);

	UFUNCTION(BlueprintPure)
	static class UAnimSequence* GetAnimSequenceWithBlendSpace1D(const class UBlendSpace1D* blendSpace1D, float nearDistance);

	static bool ParseIndexedName(const FName& name, TCHAR openDelim, TCHAR closeDelim, FName& outName, FName& outValue);

	UFUNCTION(BlueprintPure)
	static FName GetAnimSlotName(EKMAnimSlotType slotType);

protected:
	static double GameElipsedStartTime;
};
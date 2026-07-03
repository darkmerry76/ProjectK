#include "KMUtil.h"
#include <Tables/Generated/KMTable_Chapter.h>
#include "AIController.h"
#include "Character/KMCharacter.h"
#include "Core/KMGameInstance.h"
#include "Core/KMGameViewportClient.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMCharacterPDA.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameObject/KMGhostInstance.h"
#include "GameObject/KMHeroInstance.h"
#include "GameObject/KMMonsterInstance.h"

double UKMUtil::GameElipsedStartTime = 0.f;

UKMCharacterInstance* UKMUtil::GetCharacterInstance(const AKMCharacter* character)
{
	check(IsValid(character));

	return character->GetCharacterInstance();
}

UKMCharacterInstance* UKMUtil::GetCharacterInstanceByController(const AAIController* controller)
{
	check(IsValid(controller) == true);

	return GetCharacterInstance(Cast<AKMCharacter>(controller->GetPawn()));
}

void UKMUtil::GetMeshSnapShot(USkeletalMeshComponent* skeletalMeshComponent, FPoseSnapshot& outSnapShot)
{
	skeletalMeshComponent->SnapshotPose(outSnapShot);
}

bool UKMUtil::IsInTargetType(const TArray<EKMSkillTargetType>& TargetType,
	const UKMCharacterInstance* ownerInstance, const UKMCharacterInstance* targetInstance)
{
	for (int32 targetTypeIndex = 0; targetTypeIndex < TargetType.Num(); ++targetTypeIndex)
	{
		switch (TargetType[targetTypeIndex])
		{
		case EKMSkillTargetType::Self:
			if(ownerInstance == targetInstance)
			{
				return true;
			}
			break;
		case EKMSkillTargetType::Hero:
			if(IsValid(targetInstance) == true && targetInstance->IsA<UKMHeroInstance>() == true)
			{
				return true;
			}
			break;

		case EKMSkillTargetType::Monster:
			if(IsValid(targetInstance) == true && targetInstance->IsA<UKMMonsterInstance>() == true)
			{
				return true;
			}
			break;
		case EKMSkillTargetType::Ghost:
			if(IsValid(targetInstance) == true && targetInstance->IsA<UKMGhostInstance>() == true)
			{
				return true;
			}
			break;
		default:break;
		}
	}
	return false;
}

void UKMUtil::PutAllRigidBodiesToSleep(USkeletalMeshComponent* skeletalMeshComponent)
{
	skeletalMeshComponent->PutAllRigidBodiesToSleep();
}

void UKMUtil::WakeAllRigidBodies(USkeletalMeshComponent* skeletalMeshComponent)
{
	skeletalMeshComponent->WakeAllRigidBodies();
}

float UKMUtil::GetWorldSeconds(const UObject* worldContextObject)
{
	check(IsValid(worldContextObject));

	return worldContextObject->GetWorld()->GetTimeSeconds();
}

FString UKMUtil::SecondsToHMSString(float seconds)
{
	int32 secondsint = static_cast<int32>(seconds);
	
	int32 hours = secondsint / 3600;
	int32 min = (secondsint % 3600) / 60;
	int32 sec = secondsint % 60;

	return FString::Printf(TEXT("%02d:%02d:%02d"), hours,min,sec);
}

FVector2D UKMUtil::GetCameraToDirection2D(const FVector2D direction, const AController* controller)
{
	if (direction.IsNearlyZero())
	{
		return FVector2D::ZeroVector;
	}

	if (!IsValid(controller))
	{
		return FVector2D::ZeroVector;
	}
	
	const FRotator controlRotation = controller->GetControlRotation();
	const FRotator yawRotation(0.f, -90.f, 0.f);

	const FVector forward = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector right = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
	const FVector moveDirection = (forward * direction.Y) + (right * direction.X);
	return FVector2D(moveDirection.GetSafeNormal2D());
}

float UKMUtil::GetCircularAngle2D(const FVector2D direction)
{
	float angle = FMath::Atan2(direction.Y, direction.X);
	float movementCircularDirection = FMath::Fmod((angle / (PI * 2.f)) + 1.f,1.f);
	return movementCircularDirection;
}

float UKMUtil::GetCircularAngle2D8Way(const FVector2D direction)
{
	float circularAngle = GetCircularAngle2D(direction);
	const float step = 1.f / 8.f;

	float snappedCircularAngle = FMath::RoundToFloat(circularAngle / step) * step;

	if (FMath::IsNearlyEqual(snappedCircularAngle, 1.f))
	{
		snappedCircularAngle = 0.f;
	}

	return snappedCircularAngle;
}

float UKMUtil::FInterpToCircular(float currentValue, float targetValue, float deltaTime, float interpSpeed)
{
	if (interpSpeed <= 0.f)
	{
		return targetValue;
	}

	float currentYaw = currentValue * 360.f;
	float targetYaw = targetValue * 360.f;

	float deltaYaw = FMath::FindDeltaAngleDegrees(currentYaw, targetYaw);
	float desiredYaw = currentYaw + deltaYaw;

	float resultYaw = FMath::FInterpTo(currentYaw, desiredYaw, deltaTime, interpSpeed);
	return FMath::Fmod((resultYaw / 360.f) + 1.f,1.f);
}

float UKMUtil::InverseCircularDirection(float circularDirection)
{
	return FMath::Fmod(circularDirection + 0.5f,1.f);
}

float UKMUtil::GetRelativeCircularDirectionDegrees(float baseCircularDirection, float targetCircularDirection)
{
	const float baseYaw = baseCircularDirection * 360.f;
	const float targetYaw = targetCircularDirection * 360.f;
	return FMath::FindDeltaAngleDegrees(baseYaw, targetYaw);
}

EKM8WayDirection UKMUtil::ConvertDegreesTo8WayDirection(float baseCircularDirection, float targetCircularDirection)
{
	const float relativeDegrees = GetRelativeCircularDirectionDegrees(baseCircularDirection, targetCircularDirection);

	switch (FMath::RoundToInt(relativeDegrees / 45.f))
	{
	case 0: return EKM8WayDirection::Angle_0;
	case 1: return EKM8WayDirection::Angle_L45;
	case 2: return EKM8WayDirection::Angle_L90;
	case 3: return EKM8WayDirection::Angle_L135;
	case 4: return EKM8WayDirection::Angle_180;
	case -1: return EKM8WayDirection::Angle_R45;
	case -2: return EKM8WayDirection::Angle_R90;
	case -3: return EKM8WayDirection::Angle_R135;
	case -4: return EKM8WayDirection::Angle_180;
		default:break;
	}
	return EKM8WayDirection::Angle_0;
}

float UKMUtil::Get8WayDirectionYaw(EKM8WayDirection direction)
{
	switch (direction)
	{
	case EKM8WayDirection::Angle_0: return 0.f;
	case EKM8WayDirection::Angle_L45: return -45.f;
	case EKM8WayDirection::Angle_L90: return -90.f;
	case EKM8WayDirection::Angle_L135: return -135.f;
	case EKM8WayDirection::Angle_180: return 180.f;
	case EKM8WayDirection::Angle_R45: return 45;
	case EKM8WayDirection::Angle_R90: return 90;
	case EKM8WayDirection::Angle_R135: return 135;
	default : break;
	}
	return 0.f;
}

FRotator UKMUtil::GetYawRotation(const FVector& baseDirection, float yawAngle)
{
	FVector normalizedDirection = baseDirection;
	normalizedDirection.Z = 0.f;
	normalizedDirection.Normalize();

	float baseYaw = normalizedDirection.Rotation().Yaw;

	float finalYaw = baseYaw + yawAngle;

	return FRotator(0.f, finalYaw, 0.f);
}

UKMCharacterInstance* UKMUtil::SpawnCharacterObjectById(UObject* worldContextObject, FName characterTableId, const FTransform transform)
{
	const FKMTable_CharacterRow* characterTable = FKMTable_CharacterRow::FindRowPtr(characterTableId);
	check (characterTable != nullptr);

	return SpawnCharacterObjectByTable(worldContextObject, characterTable, transform);
}

UKMCharacterInstance* UKMUtil::SpawnCharacterObjectByTable(UObject* worldContextObject, const FKMTable_CharacterRow* characterTable, const FTransform& transform)
{
	if (!characterTable)
	{
		return nullptr;
	}
	
	if (!IsValid(worldContextObject) || !worldContextObject->GetWorld())
	{
		return nullptr;
	}
	
	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(assetManager) == true);
	
	UKMCharacterPDA* characterPDA = Cast<UKMCharacterPDA>(assetManager->GetAsset(characterTable->pdaKey));
	check(IsValid(characterPDA) == true);
	
	AKMCharacter* newCharacter = worldContextObject->GetWorld()->SpawnActorDeferred<AKMCharacter>(
		characterPDA->CharacterClass, transform, nullptr, nullptr);
	if(!IsValid(newCharacter))
	{
		return nullptr;
	}

	UKMCharacterInstance* newCharacterInstance = NewObject<UKMCharacterInstance>(newCharacter, characterPDA->InstanceClass);
	newCharacterInstance->SetDepthSort(transform.GetLocation().X);
	newCharacterInstance->SetTable(characterTable);
	newCharacterInstance->SetTransform(transform);


	newCharacter->bIsEditorPreviewActor = false;
	newCharacter->PossessedByCharacterInstance(newCharacterInstance);
	newCharacter->FinishSpawning(transform, false);

#if WITH_EDITOR
	if (!newCharacter->HasActorBegunPlay())
	{
		newCharacter->DispatchBeginPlay();
	}
#endif
	
	newCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	
	return newCharacterInstance;
}

FString UKMUtil::GetBuildInfo(const UObject* worldContextObject)
{
	FString buildType;

#if UE_BUILD_DEBUG
	buildType = TEXT("Debug");
#elif UE_BUILD_DEVELOPMENT
	buildType = TEXT("Dev");
#elif UE_BUILD_TEST
	buildType = TEXT("Test");
#elif UE_BUILD_SHIPPING
	buildType = TEXT("Shipping");
#else
	buildType = TEXT("Unknown");
#endif

	const FString runType =
#if WITH_EDITOR
	TEXT("Editor");
#else
	TEXT("Game");
#endif

	FString projectVersion;
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), projectVersion, GGameIni);

	FString viewportSizeString;
	FString GameElipsedTimeString;
	if (IsValid(worldContextObject))
	{
		if (UKMGameInstance* gameInstance = UKMGameInstance::GetGameInstance(worldContextObject))
		{
			if (UGameViewportClient* gameViewportClient = gameInstance->GetGameViewportClient())
			{
				FVector2D resolution;
				gameViewportClient->GetViewportSize(resolution);

				viewportSizeString = FString::Printf(TEXT("%dwx%dh"), static_cast<int32>(resolution.X), static_cast<int32>(resolution.Y));
			}
		}
		if (IsValid(worldContextObject->GetWorld()))
		{
			double finalGameElipsedTime = worldContextObject->GetWorld()->GetTimeSeconds() - GameElipsedStartTime;
			
			FTimespan timeSpan = FTimespan::FromSeconds(finalGameElipsedTime);
			GameElipsedTimeString = timeSpan.ToString(TEXT("%h:%m:%s"));
		}
	}
	
	return FString::Printf(TEXT("%s %s v%s %s %s %s %s"), *viewportSizeString, *GameElipsedTimeString, *projectVersion, *runType, *buildType, TEXT(__DATE__), TEXT(__TIME__));
}

void UKMUtil::PlaySlateFade(const UObject* worldContextObject, float startAlpha, float endAlpha, float duration, FLinearColor fadeColor)
{
	UKMGameInstance* gameInstance = UKMGameInstance::GetGameInstance(worldContextObject);
	if (!IsValid(gameInstance))
	{
		return;
	}
	
	UKMGameViewportClient* gameViewportClient = Cast<UKMGameViewportClient>(gameInstance->GetGameViewportClient());
	if (!IsValid(gameViewportClient))
	{
		return;
	}
	gameViewportClient->PlayFade(startAlpha, endAlpha, duration, fadeColor);
}
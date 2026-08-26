#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "KMDefine.generated.h"

#define UNDEFINED_KEY NAME_None
#define FKMObjectKey int32

#define ONE_FRAME_SECONDS 0.01666666666666666666666666666667
#define FRAME_TO_SECONDS(f) (f * ONE_FRAME_SECONDS)

#define ECC_Damage ECC_GameTraceChannel3

UENUM(BlueprintType)
enum class EKMGameState : uint8
{
	None = 0,
	Login,
	MainLobby,
	InGame
};

UENUM(BlueprintType)
enum class EKMBattleState : uint8
{
	None,
	Ready,
	Playing,
	Clear,
	End,
};

UENUM(BlueprintType)
enum class EKMGameSpeedType : uint8
{
	Pause = 0,
	Normal = 1,
	Double = 2,
	Triple = 3
};

USTRUCT(BlueprintType)
struct FKMInBattleHeroInst
{
	GENERATED_BODY()

public:
	int32 GameObjectId;
	int32 CurrentLevel;
	FName StatKey;
};

UENUM(Blueprintable, BlueprintType)
enum class EKMSkillPase : uint8
{
	Start,
	Loop,
	End,
	Max,
};

UENUM(BlueprintType)
enum class EKMCommbatMessageType : uint8
{
	None,
	Critical,
	GoodCancel,
	GreatCancel,
	PerfectCancel,
	GoodParry,
	GreatParry,
	PerfectParry,
};

UENUM(BlueprintType)
enum class EKMDamageEventContext : uint8
{
	Attacker,
	Target,
};

struct FKMDamageEvent
{
	FKMObjectKey Attacker;
	FKMObjectKey Target;

	float Damage = 0.f;

	bool bIsCritical = false;
	bool bIsDot = false;

	EKMDamageEventContext Context;
};

UENUM(BlueprintType)
enum class EKM8WayDirection : uint8
{
	Angle_0,
	Angle_L45,
	Angle_L90,
	Angle_L135,
	Angle_180,
	Angle_R45,
	Angle_R90,
	Angle_R135,
};

UENUM()
enum class EKMAbilityFlag : int32
{
	None,
//	ComboAvailable,
};

UENUM(Blueprintable, BlueprintType)
enum class EKMPopupType : uint8
{
	Ok,
	Cancel,
	OkCancel,
	Yes,
	No,
	YesOrNo,
};

UENUM(Blueprintable, BlueprintType)
enum class EKMPopupButtonType : uint8
{
	Ok,
	Cancel,
	Yes,
	No
};

UENUM(Blueprintable, BlueprintType)
enum class EKMInputActionType : uint8
{
	Combat,
	Technique,
	Gaurd,
	Ultimate,
};

UENUM(Blueprintable, BlueprintType)
enum class EKMAnimSlotType : uint8
{
	DefaultSlot,
	OverrideSlot,
	UpperSlot,
};

USTRUCT(Blueprintable, BlueprintType)
struct KMGAME_API FKMProjectileEventData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> movementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class AActor> TargetActor;
};

struct KMGAME_API FKMProjectileArrivalData
{
	virtual ~FKMProjectileArrivalData() { };
	virtual FName GetType() const { return TypeName(); }
	static FName TypeName() { return TEXT("FKMProjectileArrivalData"); }
};

USTRUCT(Blueprintable, BlueprintType)
struct KMGAME_API FKMSkillKeyBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TableId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;

	const struct FKMTable_SkillRow* TableRecord = nullptr;
};

struct KMGAME_API FKMSkillKey : public FKMSkillKeyBase
{
	FKMSkillKey();
	FKMSkillKey(FName tableId, int32 level);
	FKMSkillKey(const FKMSkillKeyBase& otherSkillKey);
	
	virtual ~FKMSkillKey() { };
	
	static FKMSkillKey CreateKey(FName tableId, int32 level); 

	bool operator==(const FKMSkillKey& otherSkillKey) const
	{
		return TableId == otherSkillKey.TableId && Level == otherSkillKey.Level;
	}
	
	friend FORCEINLINE uint32 GetTypeHash(const FKMSkillKey& skillKey)
	{
		return HashCombine(GetTypeHash(skillKey.TableId), GetTypeHash(skillKey.Level));
	}

	bool IsValid() const
	{
		return TableId != NAME_None;
	}

	static FKMSkillKey Null()
	{
		return FKMSkillKey(NAME_None, 0);
	};
};

namespace KMGame
{
	KMGAME_API UEnum* GetObjectTypeEnum();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMAnimationSetTag
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct KMGAME_API FEMAnimationSetTag
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag Tag;

	UPROPERTY()
	TWeakObjectPtr<class UAnimMontage> Montage;
};

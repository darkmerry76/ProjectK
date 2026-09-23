#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_Hit.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EKMCollisonType : uint8
{
	Box,
	Sphere,
	Capsule,
};

USTRUCT()
struct FKMAnimNotifyState_Hit_Context
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform PreviousTransform;

	UPROPERTY()
	int32 HitCount = 0;

	UPROPERTY()
	float ElapsedTime = 0.f;
};

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Hit")
class KMGAME_API UKMAnimNotifyState_Hit : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	EKMCollisonType CollisonType = EKMCollisonType::Box;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, DisplayName="Once", meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	bool bIsOnce = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="bIsOnce"))
	FTransform HitTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="HitTransform"))
	FName HitTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AnimNotifyBoneName=true, AllowPrivateAccess=true, DisplayAfter="HitTag"))
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="SocketName"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="ObjectTypeQuery"))
	UClass* ActorClassFilter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="ActorClassFilter"))
	bool FollowSocketRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, DisplayName="HitCheckerClear", meta=(AllowPrivateAccess=true, DisplayAfter="FollowSocketRotation"))
	bool bIsHitCheckerClear = true;

	UPROPERTY(Transient)
	TMap<class AActor*, FKMAnimNotifyState_Hit_Context> Context;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TObjectPtr<class UKMEditorDrawDebugComponent> EditorDrawDebugComponent;
#endif

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

	virtual void NotifyBeginEx(class AActor* actor, class UEMMartialArts* martialArts, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTickEx(class AActor* actor, class UEMMartialArts* martialArts, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEndEx(class AActor* actor, class UEMMartialArts* martialArts, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
	void GetFinalTransform(const class USceneComponent* ownerComponent, FTransform& outTransform) const;

	void DoHit(const class USceneComponent* ownerComponent, const FAnimNotifyEventReference& eventReference);

	void HitCheckClear(AActor* actor);

#if WITH_EDITOR
	virtual void DrawInEditor(FPrimitiveDrawInterface* pDI, USkeletalMeshComponent* meshComp, const UAnimSequenceBase* animation, const FAnimNotifyEvent& notifyEvent) const override;
#endif
	
};

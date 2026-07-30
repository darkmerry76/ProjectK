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

USTRUCT(BlueprintType)
struct KMGAME_API FKMHitCheckData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TSet<class AActor*> Actors;
};

UCLASS(Blueprintable, BlueprintType, DisplayName="[KM] Hit")
class KMGAME_API UKMAnimNotifyState_Hit : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="GroupType"))
	EKMCollisonType CollisonType = EKMCollisonType::Box;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AnimNotify, meta=(AllowPrivateAccess=true, DisplayAfter="CollisonType"))
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

	UPROPERTY()
	TMap<class USkeletalMeshComponent*, FTransform> HitPreviousTransforms;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UKMEditorDrawDebugComponent> EditorDrawDebugComponent;
#endif

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float totalDuration, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, float frameDeltaTime, const FAnimNotifyEventReference& eventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* meshComp, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& eventReference) override;

protected:
	virtual FString GetNotifyName_Implementation() const override;
	void GetFinalTransform(const class AKMCharacter* ownerCharacter, const class USkeletalMeshComponent* meshComp, FTransform& outTransform) const;

#if WITH_EDITOR
	virtual void DrawInEditor(FPrimitiveDrawInterface* pDI, USkeletalMeshComponent* meshComp, const UAnimSequenceBase* animation, const FAnimNotifyEvent& notifyEvent) const override;
#endif
	
};

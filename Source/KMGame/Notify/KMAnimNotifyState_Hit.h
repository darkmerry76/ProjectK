#pragma once

#include "CoreMinimal.h"
#include "KMAnimNotifyState.h"
#include "KMAnimNotifyState_Hit.generated.h"

USTRUCT(BlueprintType)
struct KMGAME_API FKMHitCheckData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TSet<class AActor*> Actors;
};

UCLASS(Blueprintable, BlueprintType)
class KMGAME_API UKMAnimNotifyState_Hit : public UKMAnimNotifyState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform HitTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AnimNotifyBoneName = "true"))
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UClass* ActorClassFilter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	bool FollowSocketRotation = true;

	UPROPERTY()
	TMap<class USkeletalMeshComponent*, FKMHitCheckData> HitCheckPair;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UKMEditorDrawDebugComponent> EditorDrawDebugComponent;
#endif

protected:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual void DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const override;
#endif
	
};

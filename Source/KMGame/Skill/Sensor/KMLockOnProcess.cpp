#include "KMLockOnProcess.h"

#include "Component/KMWidgetComponent.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "GameObject/KMGameObjectInstance.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_Object.h"
#include "Tables/Generated/KMTable_Object_Interactive.h"
#include "Ui/Component/KMHeadTagWidget.h"

UKMLockOnProcessBase::UKMLockOnProcessBase(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMLockOnProcessBase::Initalize()
{
	LockonTarget = MakeShared<FKMLockOnCluster>(this);
	InteractionTarget = MakeShared<FKMLockOnCluster>(this);
}

void UKMLockOnProcessBase::SetLockOnTagEnable(bool bEnable)
{
	bIsLockOnTagEnable = bEnable;
}

bool UKMLockOnProcessBase::IsLockOnTagEnable() const
{
	return bIsLockOnTagEnable;
}

UKMGameObjectInstance* UKMLockOnProcessBase::GetOwnerGamneObject() const
{
	return Cast<UKMGameObjectInstance>(GetOuter());
}

TSharedPtr<FKMLockOnCluster> UKMLockOnProcessBase::GetLockonTarget() const
{
	return LockonTarget;
}

TSharedPtr<FKMLockOnCluster> UKMLockOnProcessBase::GetInteractionTarget() const
{
	return LockonTarget;
}

void UKMLockOnProcessBase::OnProcessing(const TArray<AActor*>& actors)
{
}

UKMLockOnProcess::UKMLockOnProcess(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMLockOnProcess::Initalize()
{
	Super::Initalize();
}

void UKMLockOnProcess::SetLockOnTagVisiblity(const UKMGameObjectInstance* targetObjectInstance, bool bVisible)
{
	if (!bIsLockOnTagEnable || !IsValid(targetObjectInstance))
	{
		return;
	}

	IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(targetObjectInstance->GetOwnerActor());
	if (!pawnInterface)
	{
		return;
	}

	UKMWidgetComponent* widgetComponent = pawnInterface->GetWidgetComponent();
	if (!IsValid(widgetComponent))
	{
		return;
	}

	UKMHeadTagWidget* headTagWidget = Cast<UKMHeadTagWidget>(widgetComponent->GetWidget());
	if (!IsValid(headTagWidget))
	{
		return;
	}
	
	headTagWidget->SetVisibileLockOnImage(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

float UKMLockOnProcess::GetScoreTarget(const UKMGameObjectInstance* prevTarget, const UKMGameObjectInstance* currTarget) const
{
	float score = 0.0f;
	
	FVector ownerForward = GetOwnerGamneObject()->GetOwnerActor()->GetActorForwardVector();
	FVector targetToDirection = currTarget->GetOwnerActor()->GetActorLocation() - GetOwnerGamneObject()->GetOwnerActor()->GetActorLocation();

	FVector targetToNormal = targetToDirection.GetSafeNormal();
	float dot = FVector::DotProduct(ownerForward, targetToNormal);
	float angleDeg = FMath::RadiansToDegrees(FMath::Acos(dot));
	if (FMath::Abs(angleDeg) < 90.f)
	{
		score += (1.f - (FMath::Abs(angleDeg) / 90.f)) * 5.f;
		score += static_cast<float>(prevTarget == currTarget) * 5.f;
	}

	score += (1.f - (targetToDirection.Length() / 1000.f)) * 5.f;
	
	return score;
}

void UKMLockOnProcess::OnProcessing(const TArray<AActor*>& actors)
{
	const UKMGameObjectInstance* prevTargetGameInstance = LockOnGameObjectInstance;
	
	LockonTarget->Targets.Empty();
	InteractionTarget->Targets.Empty();

	UKMGameObjectInstance* bestTargetGameInstance = nullptr;
	UKMGameObjectInstance* bestInteractionGameInstance = nullptr;
	
	float bestScore = 0.0f;
	float bestInteractionScore = 0.0f;
	for (auto actorItr = actors.CreateConstIterator(); actorItr; ++actorItr)
	{
		IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(*actorItr);
		if (!pawnInterface)
		{
			continue;
		}

		UKMGameObjectInstance* targetGameObjectInstance = pawnInterface->GetGameObjectInstance();
		if (!IsValid(targetGameObjectInstance))
		{
			continue;
		}

		const FKMTable_ObjectRow* objectTableRow = targetGameObjectInstance->GetTable();
		if (!objectTableRow)
		{
			continue;
		}
		
		if (targetGameObjectInstance->IsDead() ||
			targetGameObjectInstance->HasGameplayTag(FKMGameplayTagName::State_Carried_Tag))
		{
			continue;
		}

		float currScore = GetScoreTarget(prevTargetGameInstance, targetGameObjectInstance);
		//if (objectTableRow->IsDestroy)
		{
			if (currScore > bestScore)
			{
				bestScore = currScore;
				bestTargetGameInstance = targetGameObjectInstance;
			}
		}
		
/*		if (IsRowA<FKMTable_Object_InteractiveRow>(objectTableRow))
		{
			if (currScore > bestInteractionScore)
			{
				bestInteractionScore = currScore;
				bestInteractionGameInstance = targetGameObjectInstance;
			}
		}*/
	}
	
	if (IsValid(bestTargetGameInstance))
	{
		LockonTarget->Targets.Emplace(bestTargetGameInstance->GetId());
		LockOnGameObjectInstance = bestTargetGameInstance;
	}

	if (IsValid(bestInteractionGameInstance))
	{
		InteractionTarget->Targets.Emplace(bestInteractionGameInstance->GetId());
		LockOnGameObjectInstance = bestInteractionGameInstance;
	}

	if (prevTargetGameInstance != LockOnGameObjectInstance)
	{
		SetLockOnTagVisiblity(prevTargetGameInstance, false);
		SetLockOnTagVisiblity(LockOnGameObjectInstance, true);
	}

	
}
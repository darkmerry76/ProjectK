#include "KMGameObjectInstance.h"

#include "Component/KMMoveShapeComponent.h"
#include "Tables/Generated/KMTable_Object.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/KMSkillTypes.h"
#include "Skill/Ability/KMAbilityEffect.h"
#include "Skill/Ability/KMAbilitySkillDirectionTag.h"
#include "Skill/Parry/KMTiming.h"
#include "Skill/Sensor/KMSensor.h"
#include "Stat/KMStatModifierBase.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_SkillEffect_Normal.h"
#include "Util/KMUtil.h"

UKMGameObjectInstance::UKMGameObjectInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SkillHandlerClass = UKMSkillHandler::StaticClass();
	StatModifierClass = UKMStatModifierBase::StaticClass();
}

void UKMGameObjectInstance::BeginPlay()
{
	Super::BeginPlay();

	StatModifier = NewObject<UKMStatModifierBase>(this, StatModifierClass, TEXT("StatModifier"));
	StatModifier->GetEffectiveStat().StatChange.AddUObject(this, &ThisClass::OnStatChange);
	StatModifier->Init();

	SkillHandler = NewObject<UKMSkillHandler>(this, SkillHandlerClass, TEXT("SkillHandler"));

	if (IsValid(SensorClass))
	{
		SensorInstance = NewObject<UKMSensor>(this, SensorClass, TEXT("Sensor"));
		SensorInstance->ResultDelegate.BindUObject(this, &ThisClass::OnSensorResult);
		SensorInstance->Init();
	}
	
	LockonTarget = MakeShared<FKMLockOnCluster>(this);
}

void UKMGameObjectInstance::EndPlay()
{
	Super::EndPlay();
}

void UKMGameObjectInstance::SetTable(const FKMTable_ObjectRow* newObjectTable)
{
	ObjectTable = newObjectTable;
}

const FKMTable_ObjectRow* UKMGameObjectInstance::GetTable() const
{
	return ObjectTable;
}

FName UKMGameObjectInstance::GetTableId() const
{
	if (!ObjectTable)
	{
		return NAME_None;
	}
	return ObjectTable->Id;
}

FName UKMGameObjectInstance::GetStatTableId() const
{
	if (!ObjectTable)
	{
		return NAME_None;
	}
	return ObjectTable->StatId;
}

FString UKMGameObjectInstance::GetObjectName() const
{
	if (!ObjectTable)
	{
		return TEXT("");
	}
	return ObjectTable->Name;
}

FTransform UKMGameObjectInstance::GetTransform() const
{
	if (!IsValid(GetOwnerActor()))
	{
		return FTransform::Identity;
	}
	return GetOwnerActor()->GetTransform();
}

void UKMGameObjectInstance::SetOwnerActor(AActor* newOwnerActor)
{
	OwnerActor = newOwnerActor;
}

AActor* UKMGameObjectInstance::GetOwnerActor() const
{
	if (!OwnerActor.IsValid())
	{
		return nullptr;
	}
	return OwnerActor.Get();
}

void UKMGameObjectInstance::AddGameplayTag(FGameplayTag newTag)
{
	GameplayTagContainer.AddTag(newTag);

	OnAddGameplayTag(newTag);
}

void UKMGameObjectInstance::OnAddGameplayTag_Implementation(const FGameplayTag& newTag)
{
}

void UKMGameObjectInstance::RemoveGameplayTag(FGameplayTag removedTag)
{
	GameplayTagContainer.RemoveTag(removedTag);

	OnRemoveGameplayTag(removedTag);
}

void UKMGameObjectInstance::OnRemoveGameplayTag_Implementation(const FGameplayTag& removedTag)
{
}

bool UKMGameObjectInstance::HasGameplayTag(FGameplayTag tag) const
{
	return GameplayTagContainer.HasTag(tag);
}

int32 UKMGameObjectInstance::GetGameplayTagCount(FGameplayTag tag) const
{
	return GameplayTagContainer.GetRefCount(tag);
}

UKMSkillHandler* UKMGameObjectInstance::GetSkillHandler() const
{
	return SkillHandler;
}

UKMStatModifierBase* UKMGameObjectInstance::GetStatModifier() const
{
	return StatModifier;
}

void UKMGameObjectInstance::HitCheckClear()
{
	HitCheckData.Actors.Empty();	
}

bool UKMGameObjectInstance::IsDead() const
{
	if (!IsValid(StatModifier) || StatModifier->GetEffectiveStat().GetHpCurr() > 0.f)
	{
		return false;
	}

	return true;
}

bool UKMGameObjectInstance::IsAir() const
{
	return false;
}

void UKMGameObjectInstance::Inflict(UKMGameObjectInstance* victimGameObjectInstance)
{
}

void UKMGameObjectInstance::Hit(UKMGameObjectInstance* attackerGameObjectInstance, TSharedPtr<FKMSkillInstance> latestSkillInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	if (HasGameplayTag(FKMGameplayTagName::State_Invincible_Tag))
	{
		return;
	}
	SkillHandler->ClearActiveSkills();

	TArray<TSharedPtr<FKMSkillEffectInstance>> skillEffectInstances = SkillHandler->ApplyEffects(latestSkillInstance, FKMGameplayTagName::Event_Hit_Tag, hitTag);
	for (auto skillEffectItr : skillEffectInstances)
	{
		if (skillEffectItr->GetType() != FKMSkillEffectAbnormalInstance::TypeName())
		{
			continue;
		}
		TSharedPtr<FKMSkillEffectInstance> skillEffectInstance = StaticCastSharedPtr<FKMSkillEffectInstance>(skillEffectItr);
		for (auto ability : skillEffectInstance->GetAbilitieAssets())
		{
			UKMAbilityEffect* abilityEffect = Cast<UKMAbilityEffect>(ability);
			if (!IsValid(abilityEffect))
			{
				continue;
			}
	
			FTransform impactTransform;
			impactTransform.SetLocation(hitClosestPoint);
			abilityEffect->Impact(impactTransform);

			if (IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(abilityEffect->GetOwnerActor()))
			{
				pawnInterface->OnImpact(skillEffectInstance, hitClosestPoint, hitTag);
			}
		}
		
		if (HitPowerType < skillEffectInstance->GetEffectTableRecord()->PowerEventType)
		{
			HitPowerType = skillEffectInstance->GetEffectTableRecord()->PowerEventType;
		}
		if (attackerGameObjectInstance->InflectPowerType < skillEffectInstance->GetEffectTableRecord()->PowerEventType)
		{
			attackerGameObjectInstance->InflectPowerType = skillEffectInstance->GetEffectTableRecord()->PowerEventType;
		}
	}
}

void UKMGameObjectInstance::HitCollection(const TWeakPtr<FKMSkillInstance>& adjustSkillInstance, AActor* hitActor, const FVector& hitLocation, const FVector& hitNormal, const FName& hitTag)
{
	if (OwnerActor == hitActor)
	{
		return;
	}

	if (!adjustSkillInstance.IsValid())
	{
		return;
	}

	if (HitCheckData.Actors.Contains(hitActor))
	{
		return;
	}

	IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(hitActor);
	if (!pawnInterface)
	{
		return;
	}

	UKMGameObjectInstance* hitGameObjectInstance = pawnInterface->GetGameObjectInstance();
	check(IsValid(hitGameObjectInstance));

	if (hitGameObjectInstance->IsDead() || hitGameObjectInstance->HasGameplayTag(FKMGameplayTagName::State_Intangible_Tag))
	{
		return;
	}

	HitCheckData.Actors.FindOrAdd(hitActor);

	Inflict(hitGameObjectInstance);
	
	if (adjustSkillInstance.IsValid())
	{
		TSharedPtr<FKMSkillInstance> duplicatSkillInstance = MakeShared<FKMSkillInstance>(*adjustSkillInstance.Pin().Get()); 
		duplicatSkillInstance->Target = MakeShared<FKMLockOnCluster>(this);
		duplicatSkillInstance->Target->Targets.Emplace(hitGameObjectInstance->GetId());
				
		UKMSkillHandler* hitCharacterSkillHandler = hitGameObjectInstance->GetSkillHandler();
		check(IsValid(hitCharacterSkillHandler));

		hitGameObjectInstance->Hit(this, duplicatSkillInstance, hitLocation, hitTag);
	}
}

void UKMGameObjectInstance::HitCollections(const TWeakPtr<FKMSkillInstance>& adjustSkillInstance, TArray<FHitResult> hitResults, UClass* actorClassFilter, const FName& hitTag)
{
	for (const FHitResult& hitResult : hitResults)
	{
		if (AActor* actor = hitResult.GetActor())
		{
			if (!actor->IsA(actorClassFilter))
			{
				continue;
			}
			HitCollection(adjustSkillInstance, actor, hitResult.ImpactPoint, hitResult.ImpactNormal, hitTag);
		}
	}
}

void UKMGameObjectInstance::BoxHitImpact(const TWeakPtr<FKMSkillInstance>& adjustSkillInstance,
	const FTransform& startOrientationTransform, const FTransform& endOrientationTransform,
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter, const FName& hitTag)
{
	if (objectTypeQuery.IsEmpty())
	{
		objectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECC_Damage));
	}
	if (!IsValid(actorClassFilter))
	{
		actorClassFilter = AActor::StaticClass();
	}
	
	TArray<FHitResult> hitResults;
	FCollisionQueryParams queryParams;
	if (OwnerActor.IsValid())
	{
		queryParams.AddIgnoredActor(OwnerActor.Get());
	}
	
	if (GetWorld()->SweepMultiByObjectType(hitResults,startOrientationTransform.GetLocation(),
	endOrientationTransform.GetLocation(),endOrientationTransform.GetRotation(),objectTypeQuery, FCollisionShape::MakeBox(endOrientationTransform.GetScale3D()), queryParams))
	{
		HitCollections(adjustSkillInstance, hitResults, actorClassFilter, hitTag);
	}
}

void UKMGameObjectInstance::SphereHitImpact(
	const TWeakPtr<FKMSkillInstance>& adjustSkillInstance,
	const FTransform& startOrientationTransform, const FTransform& endOrientationTransform,
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter, const FName& hitTag)
{
	if (objectTypeQuery.IsEmpty())
	{
		objectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Damage));
	}
	if (!IsValid(actorClassFilter))
	{
		actorClassFilter = AActor::StaticClass();
	}

	TArray<FHitResult> hitResults;
	FCollisionQueryParams queryParams;
	if (OwnerActor.IsValid())
	{
		queryParams.AddIgnoredActor(OwnerActor.Get());
	}
	
	if (GetWorld()->SweepMultiByObjectType(hitResults,startOrientationTransform.GetLocation(),endOrientationTransform.GetLocation(),
	FQuat::Identity,objectTypeQuery, FCollisionShape::MakeSphere(endOrientationTransform.GetScale3D().X * 100.f), queryParams))
	{
		HitCollections(adjustSkillInstance, hitResults, actorClassFilter, hitTag);
	}
}

void UKMGameObjectInstance::OnStatChange(EKMStatFactorType factorType, float prevValue, float newValue)
{
	if (GetGlobalStatChangeEvent().IsBound())
	{
		GetGlobalStatChangeEvent().Broadcast(this, factorType, prevValue, newValue);
	}

	if (StatChangeEvent.IsBound())
	{
		StatChangeEvent.Broadcast(this, factorType, prevValue, newValue);
	}
	
	if (factorType == EKMStatFactorType::HpCurr)
	{
		if (!IsDead())
		{
			if (newValue <= 0)
			{
				OnDeath();
			}
		}
	}

	if (EKMStatFactorType::Damage <= factorType && factorType <= EKMStatFactorType::DamageMax)
	{
		ShowDamage(factorType, newValue);
	}
}

void UKMGameObjectInstance::ShowDamage(EKMStatFactorType factorType, int32 damage)
{
	if (damage <= 0) return;
}

void UKMGameObjectInstance::OnDeath()
{
	GameplayTagContainer.AddTag(FKMGameplayTagName::State_Dead_Tag);
	GameplayTagContainer.AddTag(FKMGameplayTagName::Block_Control_Tag);
	if (!GameplayTagContainer.HasTag(FKMGameplayTagName::State_Blow_Tag))
	{
		if (const FKMTable_SkillEffect_NormalRow* skillEffectDieTableRow = FKMTable_SkillEffect_NormalRow::FindRowPtr(TEXT("eff_die")))
		{
			//if (UKMAbility* deathAbility = Cast<UKMAbility>(GetStatModifier()->ApplyEffectiveAnimation(skillEffectDieTableRow->Ability.PdaKey)))
			//{
			//deathAbility->Activate();
			//}
		}
	}

	if (IKMPawnInterface* pawnInterface = Cast<IKMPawnInterface>(GetOwnerActor()))
	{
		pawnInterface->OnDeath();			
	}

	GetDeathDelegate().Broadcast(this);
}

void UKMGameObjectInstance::BroadCastDamageEvent(const FKMDamageEvent& newDamageEvent)
{
	if (DamageDelegate.IsBound())
	{
		DamageDelegate.Broadcast(newDamageEvent);
	}
}

void UKMGameObjectInstance::Stiff(float duration, bool bReset)
{
	if (FMath::IsNearlyZero(duration))
	{
		return;
	}

	float newDuration = duration;
	if (StiffTimerHandle.IsValid())
	{
		if (!bReset)
		{
			newDuration = duration - GetWorld()->GetTimerManager().GetTimerRemaining(StiffTimerHandle);
		}
		if (newDuration <= 0.f)
		{
			return;
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(StiffTimerHandle);
		}
	}
	else
	{
		SetTimeDilation(TEXT("HitStop"), 0.f);
	}
	GetWorld()->GetTimerManager().SetTimer(StiffTimerHandle, FTimerDelegate::CreateUObject(this, &UKMGameObjectInstance::OnStiffRelease), newDuration, false);
}

void UKMGameObjectInstance::OnStiffRelease()
{
	RemoveTimeDilation(TEXT("HitStop"));
	GetWorld()->GetTimerManager().ClearTimer(StiffTimerHandle);
}

float UKMGameObjectInstance::GetTimeDilation() const
{
	float timeDilation = 1.f;
	for (auto timeDilationItr : TimeDilations)
	{
		timeDilation *= timeDilationItr.Value;
	}

	return timeDilation;
}

void UKMGameObjectInstance::SetTimeDilation(const FName& layerName, float newTimeDilation)
{
	TimeDilations.Add(layerName, newTimeDilation);
}

void UKMGameObjectInstance::RemoveTimeDilation(const FName& layerName)
{
	TimeDilations.Remove(layerName);
}

float UKMGameObjectInstance::GetHorizontalDistanceTo(const UKMGameObjectInstance* toGameObjectInstance) const
{
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwnerActor());
	check(ownerPawnInterface);
	
	AActor* targetActor = toGameObjectInstance->GetOwnerActor();
	if (!IsValid(targetActor))
	{
		return 0.f;
	}
	
	const IKMPawnInterface* pawnTargetInterface = Cast<IKMPawnInterface>(toGameObjectInstance->GetOwnerActor());
	if (!pawnTargetInterface)
	{
		return 0.f;
	}

	UPrimitiveComponent* ownerShapeComponent = ownerPawnInterface->GetMovementShapeComponent();
	UPrimitiveComponent* targetShapeComponent = pawnTargetInterface->GetMovementShapeComponent();

	if (UKMMoveShapeComponent* moveShapeComponent = Cast<UKMMoveShapeComponent>(targetShapeComponent))
	{
		return moveShapeComponent->GetHorizontalDistance(ownerShapeComponent);
	}
	else if(UCapsuleComponent* targetCapsuleComponent = Cast<UCapsuleComponent>(targetShapeComponent))
	{
		if (UKMMoveShapeComponent* ownerMoveShapeComponent = Cast<UKMMoveShapeComponent>(ownerShapeComponent))
		{
			return ownerMoveShapeComponent->GetHorizontalDistance(targetCapsuleComponent);
		}
		else if (UCapsuleComponent* ownerCapsuleComponent = Cast<UCapsuleComponent>(ownerShapeComponent))
		{
			return (ownerCapsuleComponent->GetComponentLocation() - targetCapsuleComponent->GetComponentLocation()).Size2D() - ownerCapsuleComponent->GetScaledCapsuleRadius() - targetCapsuleComponent->GetScaledCapsuleRadius();
		} 
	}
	return (GetOwnerActor()->GetActorLocation() - targetActor->GetActorLocation()).Size2D();
}

float UKMGameObjectInstance::GetVerticalDistanceTo(const UKMGameObjectInstance* toGameObjectInstance) const
{
	IKMPawnInterface* ownerPawnInterface = Cast<IKMPawnInterface>(GetOwnerActor());
	check(ownerPawnInterface);

	const IKMPawnInterface* pawnTargetInterface = Cast<IKMPawnInterface>(toGameObjectInstance->GetOwnerActor());
	if (!pawnTargetInterface)
	{
		return 0.f;
	}

	UPrimitiveComponent* ownerShapeComponent = ownerPawnInterface->GetMovementShapeComponent();
	UPrimitiveComponent* targetShapeComponent = pawnTargetInterface->GetMovementShapeComponent();

	if (UKMMoveShapeComponent* targetMoveShapeComponent = Cast<UKMMoveShapeComponent>(targetShapeComponent))
	{
		return targetMoveShapeComponent->GetVerticalDistance(ownerShapeComponent);
	}
	else if(UCapsuleComponent* targetCapsuleComponent = Cast<UCapsuleComponent>(targetShapeComponent))
	{
		if (UKMMoveShapeComponent* ownerMoveShapeComponent = Cast<UKMMoveShapeComponent>(ownerShapeComponent))
		{
			return ownerMoveShapeComponent->GetVerticalDistance(targetCapsuleComponent);
		}
		else if (UCapsuleComponent* ownerCapsuleComponent = Cast<UCapsuleComponent>(ownerShapeComponent))
		{
			if (targetCapsuleComponent)
			{
				const float ownerBottomLocationZ = ownerCapsuleComponent->GetComponentLocation().Z - ownerCapsuleComponent->GetScaledCapsuleHalfHeight();
				const float targetBottomLocationZ = targetCapsuleComponent->GetComponentLocation().Z - targetCapsuleComponent->GetScaledCapsuleHalfHeight();
				return FMath::Abs(ownerBottomLocationZ - targetBottomLocationZ);
			}
		}
	}

	return FMath::Abs(ownerShapeComponent->GetComponentLocation().Z - targetShapeComponent->GetComponentLocation().Z);
}

void UKMGameObjectInstance::SetDirectionVisual(float newDirection, bool bForceRotate, USkeletalMeshComponent* otherSkeletalMeshComp)
{
	
}
	
void UKMGameObjectInstance::SetDirection(float newDirection, bool bForceRotate)
{
	Direction = newDirection;
}

float UKMGameObjectInstance::GetDirection() const
{
	return Direction;
}

void UKMGameObjectInstance::SetInteractionDirection(float newDirection)
{
	InteractionDirection = newDirection;	
}

float UKMGameObjectInstance::GetInteractionDirection() const
{
	return InteractionDirection;
}

void UKMGameObjectInstance::AddAggroTarget(UKMGameObjectInstance* attackerGameObjectInstance)
{
	if (!AggroTarget.Contains(attackerGameObjectInstance))
	{
		AggroTarget.Add(attackerGameObjectInstance);
	}
}

const UKMGameObjectInstance* UKMGameObjectInstance::GetBestAggroTarget() const
{
	if (AggroTarget.IsEmpty())
	{
		return nullptr;
	}
	return AggroTarget.begin()->Get();
}

bool UKMGameObjectInstance::UseSkillParam(const FName skillName, int32 skillLevel, const TArray<FKMAssistSkillData> assistSkillData)
{
	TSharedPtr<FKMSkillInstance> skillInstance = GetSkillHandler()->UseSkill(FKMSkillKey(skillName,skillLevel), LockonTarget);
	if (skillInstance.IsValid())
	{
		for (int32 assistSkillIndex = 0; assistSkillIndex < assistSkillData.Num(); ++assistSkillIndex)
		{
			TSharedPtr<FKMSkillInstance> assistSkillInstance =
				GetSkillHandler()->UseAssistSkill(FKMSkillKey(assistSkillData[assistSkillIndex].SkillName,assistSkillData[assistSkillIndex].SkillLevel));
					
			skillInstance->AddAssistSkill(assistSkillInstance);

			assistSkillInstance->Tags = assistSkillData[assistSkillIndex].Tags;
		}
		return true;
	}
	return false;
}

void UKMGameObjectInstance::UseCombatSkill()
{
	GetSkillHandler()->UseCombatSkill(MakeShared<FKMLockOnCluster>(*LockonTarget.Get()));
}

void UKMGameObjectInstance::UseUltimateSkill()
{
	GetSkillHandler()->UseUltimateSkill();
}

void UKMGameObjectInstance::UseTechniqueSkill()
{
	GetSkillHandler()->UseTechniqueSkill(MakeShared<FKMLockOnCluster>(*LockonTarget.Get()));
}

void UKMGameObjectInstance::UseTechniqueSkill_Release()
{
	GetSkillHandler()->UseSkill_Release();
}

bool UKMGameObjectInstance::UseParrySkill()
{
	const FKMSkillKey guardSkillKey(TEXT("sk_stand_guard"), 0);
	if (SkillHandler->IsSkillActivated(guardSkillKey))
	{
		FKMSkillKey parrySkillKey = {};
		if (TimingParry.IsValid())
		{
			TimingParry->SetUsed(true);
			EKMTimingResult parryResult = TimingParry->GetResult();
			if(parryResult == EKMTimingResult::Perfect)
			{
				parrySkillKey = FKMSkillKey(TEXT("sk_front_perfect_parry"), 0);
				CombatMessageDelegate.Broadcast(this, EKMCommbatMessageType::PerfectParry, TEXT(""));
			}
			else if (parryResult == EKMTimingResult::Great)
			{
				parrySkillKey = FKMSkillKey(TEXT("sk_front_great_parry"), 0);
				CombatMessageDelegate.Broadcast(this, EKMCommbatMessageType::GreatParry, TEXT(""));
			}
			else
			{
				parrySkillKey = FKMSkillKey(TEXT("sk_front_good_parry"), 0);
			}
			TWeakPtr<FKMSkillInstance> guardSkillInstance = TimingParry->GetGuardSkillInstance();
			if (guardSkillInstance.IsValid())
			{
				guardSkillInstance.Pin()->SetForceComplete(true);
			}
			if(SkillHandler->CanUseSkill(parrySkillKey, LockonTarget))
			{
				SkillHandler->UseSkill(parrySkillKey, LockonTarget);
			}
			TimingParry = nullptr;
			return true;
		}
	}
	return false;
}

bool UKMGameObjectInstance::UseSkill(const FName skillName, int32 skillLevel)
{
	return UseSkillParam(skillName, skillLevel, TArray<FKMAssistSkillData>());
}

void UKMGameObjectInstance::UseSkillDash(float dashDirection)
{
	if (HasGameplayTag(FKMGameplayTagName::Block_Control_Dash_Tag))
	{
		return;
	}
	
	if (TimingCancel.IsValid() && TimingCancel->IsComplete())
	{
		TimingCancel = nullptr;
	}
	
	if (TimingCancel.IsValid() && !TimingCancel->IsUsed())
	{
		EKM8WayDirection direction8way = UKMUtil::ConvertDegreesTo8WayDirection(dashDirection, GetDirection());

		FKMSkillKey dashSkillKey;
		EKMTimingResult cancelResult = TimingCancel->GetResult();
		cancelResult = EKMTimingResult::Great;
		if (cancelResult == EKMTimingResult::Perfect)
		{
			dashSkillKey = FKMSkillKey(TEXT("sk_perfect_cancel_dash"), 0);
			CombatMessageDelegate.Broadcast(this, EKMCommbatMessageType::PerfectCancel, TEXT(""));
		}
		else if (cancelResult == EKMTimingResult::Great)
		{
			dashSkillKey = FKMSkillKey(TEXT("sk_great_cancel_dash"), 0);
			CombatMessageDelegate.Broadcast(this, EKMCommbatMessageType::GreatCancel, TEXT(""));
		}
		else if (cancelResult == EKMTimingResult::Good)
		{
			dashSkillKey = FKMSkillKey(TEXT("sk_good_cancel_dash"), 0);
			CombatMessageDelegate.Broadcast(this, EKMCommbatMessageType::GoodCancel, TEXT(""));
		}
		TimingCancel->SetUsed(true);
		if (dashSkillKey.IsValid())
		{
			if (SkillHandler->CanUseSkill(dashSkillKey, nullptr))
			{
				SkillHandler->ClearActiveSkills(true);
				TSharedPtr<FKMSkillInstance> cancelSkillInstance = SkillHandler->UseSkill(dashSkillKey, nullptr);
				if (cancelSkillInstance.IsValid())
				{
					for (auto& ability : cancelSkillInstance->GetAbilitieAssets())
					{
						if (UKMAbilitySkillDirectionTag* abilitySkillDirectionTag = Cast<UKMAbilitySkillDirectionTag>(ability))
						{
							abilitySkillDirectionTag->ApplyAngle(direction8way, 150.f, 0.35f);
						}
					}
				}
			}
			TimingCancel = nullptr;
		}
	}
	else if(!HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		SetDirection(dashDirection);
		SkillHandler->UseSkill(FKMSkillKey(TEXT("sk_front_dash"), 0), nullptr);
	}
}

bool UKMGameObjectInstance::UseGuardSkill()
{
	const FKMSkillKey guardSkillKey(TEXT("sk_stand_guard"), 0);
	if(!HasGameplayTag(FKMGameplayTagName::Block_Control_Tag) || SkillHandler->CanUseSkill(guardSkillKey, LockonTarget))
	{
		TSharedPtr<FKMSkillInstance> guardSkillInstance = SkillHandler->UseSkill(guardSkillKey, LockonTarget);
		if (guardSkillInstance.IsValid())
		{
			TimingParry = MakeShared<FKMTimingParry>(this);
			TimingParry->SetGuardSkillInstance(guardSkillInstance);
		}
	}

	return true;
}

bool UKMGameObjectInstance::UseGuardSkill_Release()
{
	const FKMSkillKey guardSkillKey(TEXT("sk_stand_guard"), 0);
	TSharedPtr<FKMSkillInstance> guardSkillInstance = SkillHandler->GetSkillInstance(guardSkillKey);
	if (guardSkillInstance.IsValid())
	{
		guardSkillInstance->RequestEnd();
	}
	
	TimingParry = nullptr;
	
	return true;
}

void UKMGameObjectInstance::OnSensorResult(const TArray<AActor*>& resultActors)
{
	check(LockonTarget.IsValid());
	
	LockonTarget->Targets.Empty();
	for (auto actorItr = resultActors.CreateConstIterator(); actorItr; ++actorItr)
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

		if (targetGameObjectInstance->IsDead() ||
			targetGameObjectInstance->HasGameplayTag(FKMGameplayTagName::State_Blow_Bound_Tag) ||
			targetGameObjectInstance->HasGameplayTag(FKMGameplayTagName::State_Blow_Down_Tag))
		{
			continue;
		}
		
		LockonTarget->Targets.Emplace(targetGameObjectInstance->GetId());
		break;
	}
}

void UKMGameObjectInstance::Tick(float deltaSeconds)
{
	StatModifier->ComputePreEffectStat();
	SkillHandler->Tick(deltaSeconds);
	StatModifier->ComputePostEffectStat();
}
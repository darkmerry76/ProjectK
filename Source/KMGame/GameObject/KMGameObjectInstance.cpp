#include "KMGameObjectInstance.h"
#include "Tables/Generated/KMTable_Object.h"
#include "GameActor/Pawn/KMPawnInterface.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/KMSkillTypes.h"
#include "Skill/Ability/KMAbilityEffect.h"
#include "Stat/KMStatModifierBase.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_SkillEffect_Normal.h"

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
		objectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECC_Destructible));
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
		objectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECC_Destructible));
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
		if (IsDead() == false)
		{
			if (newValue <= 0)
			{
				check(IsDead() == false);
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

void UKMGameObjectInstance::Tick(float deltaSeconds)
{
	StatModifier->ComputePreEffectStat();
	SkillHandler->Tick(deltaSeconds);
	StatModifier->ComputePostEffectStat();
}
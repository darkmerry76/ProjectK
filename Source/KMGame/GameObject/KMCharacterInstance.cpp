#include "KMCharacterInstance.h"
#include "EMCurveWarpingComponent.h"
#include "Actor/KMItemAppearanceActor.h"
#include "Animation/KMAnimInstance.h"
#include "Character/KMCharacter.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMCurveWarpingComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMBeastPDA.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/Ability/KMAbility.h"
#include "Skill/Ability/KMAbilityEffect.h"
#include "Skill/Ability/KMAbilitySkillDirectionTag.h"
#include "Skill/Parry/KMTiming.h"
#include "Skill/Sensor/KMSensor.h"
#include "Stat/KMStatModifierBase.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_BaseStat_Beast.h"
#include "Tables/Generated/KMTable_Beast.h"
#include "Tables/Generated/KMTable_Character.h"
#include "Tables/Generated/KMTable_SkillEffect_Normal.h"
#include "Tables/Generated/KMTable_SkillSet.h"
#include "Util/KMUtil.h"

const FName LeaveSkeletalMeshCompTag = TEXT("LeaveSkeletalMeshComp");

UKMCharacterInstance::UKMCharacterInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SkillHandlerClass = UKMSkillHandler::StaticClass();
	StatModifierClass = UKMStatModifierBase::StaticClass();
	SensorClass = UKMSensor::StaticClass();
}

FName UKMCharacterInstance::GetRecordKey() const
{
	check(Table != nullptr);
	return Table->Id;	
}

FName UKMCharacterInstance::GetRecordStatKey() const
{
	if (IsBeast())
	{
		return BeastTableRow->StatId;
	}
	check(Table != nullptr);
	return Table->StatId;	
}

UKMStatModifierBase* UKMCharacterInstance::GetStatModifier() const
{
	return StatModifier;
}

void UKMCharacterInstance::ChangeSkillSet(const FName& ownerId)
{
	if (!IsValid(StatModifier))
	{
		return;
	}
	
	SkillHandler->ClearResisterSkillSet();
	SkillHandler->ClearAllSkills();
	
	const TMap<FName, FKMTable_SkillSetRow*>& skillSetTable = FEMDataTableHelper::Get().GetRowMap<FKMTable_SkillSetRow>();
	for (auto skillSetItr = skillSetTable.CreateConstIterator(); skillSetItr; ++skillSetItr)
	{
		const FKMTable_SkillSetRow* skillSetRow = skillSetItr->Value;
		
		if (!skillSetRow->OwnerCharacter.IsEmpty() && !skillSetRow->OwnerCharacter.Contains(ownerId))
		{
			continue;
		}
		SkillHandler->RegisterSkillSets(skillSetRow);
	}
}

void UKMCharacterInstance::BeginPlay()
{
	Super::BeginPlay();

	StatModifier = NewObject<UKMStatModifierBase>(this, StatModifierClass, TEXT("StatModifier"));
	StatModifier->GetEffectiveStat().StatChange.AddUObject(this, &ThisClass::OnStatChange);
	StatModifier->Init();

	SkillHandler = NewObject<UKMSkillHandler>(this, SkillHandlerClass, TEXT("SkillHandler"));

	ChangeSkillSet(Table->Id);
	
	SensorInstance = NewObject<UKMSensor>(this, SensorClass, TEXT("Sensor"));
	SensorInstance->ResultDelegate.BindUObject(this, &ThisClass::OnSensorResult);
	SensorInstance->Init();
	LockonTarget = MakeShared<FKMLockOnCluster>(this);

	SetBeastTableId(GetTable()->DefaultBeast);
}

void UKMCharacterInstance::EndPlay()
{
	Super::EndPlay();

	if (IsValid(StatModifier))
	{
		StatModifier->GetEffectiveStat().StatChange.RemoveAll(this);
	}

	if (Character.IsValid())
	{
		Character->Destroy();
		Character = nullptr;
	}
}

void UKMCharacterInstance::SetCharacter(AKMCharacter* newCharacter)
{
	Character = newCharacter;
}

AKMCharacter* UKMCharacterInstance::GetCharacter() const
{
	if (Character.IsValid() == false)
	{
		return nullptr;
	}
	
	return Character.Get();
}

void UKMCharacterInstance::SetBeastTableId(FName newBeatId)
{
	BeastId = newBeatId;

	BeastTableRow = FKMTable_BeastRow::FindRowPtr(BeastId);
	if (!BeastTableRow)
	{
		return;
	}

	BeastStatTableRow = FKMTable_BaseStat_BeastRow::FindRowPtr(BeastTableRow->StatId);
	if(!BeastStatTableRow)
	{
		return;
	}

	UKMAssetManager* assetManager = UKMAssetManager::GetAssetManager();
	if(!IsValid(assetManager))
	{
		return;
	}

	BeastPDA = Cast<UKMBeastPDA>(assetManager->GetAsset(BeastTableRow->AssetPda));
}

FName UKMCharacterInstance::GetBeatId() const
{
	return BeastId;
}

void UKMCharacterInstance::SpawnBeast()
{
}

void UKMCharacterInstance::ToggleBeast()
{
	if (IsBeast())
	{
		RevertFromBest();
	}
	else
	{
		TransformToBeast();
	}
}

void UKMCharacterInstance::RevertFromBest()
{
	if (!IsBeast())
	{
		return;
	}

	if (BeastTableRow && IsValid(SkillHandler))
	{
		if (SkillHandler->IsSkillActivated(FKMSkillKey(BeastTableRow->TransformSkill, 0)))
		{
			return;
		}
	}

	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	AKMCharacter* ownerCharacterCDO = GetCharacter()->GetClass()->GetDefaultObject<AKMCharacter>();
	if (!IsValid(ownerCharacterCDO))
	{
		return;
	}

	const FKMTable_CharacterRow* characterTableRow = GetTable();
	if (!characterTableRow)
	{
		return;
	}

	ChangeSkillSet(Table->Id);

	UKMSkeletalMeshComponent* leaveSkeletalMeshComponnent = Cast<UKMSkeletalMeshComponent>(ownerCharacter->FindComponentByTag(UKMSkeletalMeshComponent::StaticClass(), LeaveSkeletalMeshCompTag));
	if (!IsValid(leaveSkeletalMeshComponnent))
	{
		leaveSkeletalMeshComponnent = Cast<UKMSkeletalMeshComponent>(
			ownerCharacter->AddComponentByClass(UKMSkeletalMeshComponent::StaticClass(), false, FTransform::Identity, false));
	}
	if (IsValid(leaveSkeletalMeshComponnent))
	{
		leaveSkeletalMeshComponnent->SetSkeletalMeshAsset(ownerCharacter->GetMesh()->GetSkeletalMeshAsset());
		leaveSkeletalMeshComponnent->SetAnimInstanceClass(ownerCharacter->GetMesh()->GetAnimClass());
		leaveSkeletalMeshComponnent->SetRelativeTransform(ownerCharacter->GetMesh()->GetRelativeTransform());
		SetCharacterDirectionVisual(GetCharacterDirection(), true, leaveSkeletalMeshComponnent);
		leaveSkeletalMeshComponnent->SetReceivesDecals(false);
		leaveSkeletalMeshComponnent->SetRenderCustomDepth(true);
		leaveSkeletalMeshComponnent->SetCustomDepthStencilValue(1);
		leaveSkeletalMeshComponnent->ComponentTags.Emplace(LeaveSkeletalMeshCompTag);
	}
	
	ownerCharacter->GetMesh()->EmptyOverrideMaterials();
	ownerCharacter->GetMesh()->SetAnimInstanceClass(ownerCharacterCDO->GetMesh()->GetAnimClass());
	ownerCharacter->GetMesh()->SetSkeletalMesh(ownerCharacterCDO->GetMesh()->GetSkeletalMeshAsset());
	ownerCharacter->GetMesh()->SetRelativeScale3D(FVector(characterTableRow->scale));
	SetCharacterDirection(GetCharacterDirection(), true);

	bIsBeast = false;
	if (GetWorld()->IsGameWorld())
	{
		if (characterTableRow->TransformSkill != NAME_None)
		{
			SkillHandler->UseSkill(FKMSkillKey(characterTableRow->TransformSkill, 0), nullptr);
		}
		if (StatModifier)
		{
			StatModifier->Compact();
		}

		UKMCurveWarpingComponent* curveWarping = Cast<UKMCurveWarpingComponent>(ownerCharacter->GetCurveWarping());
		if (IsValid(curveWarping))
		{
			curveWarping->ClearCustomMovementAnimation();
			curveWarping->DisableCustomMovement();
		}
		OnRevertFromBest();
	}
}

void UKMCharacterInstance::OnRevertFromBest_Implementation()
{
}

void UKMCharacterInstance::TransformToBeast()
{
	if (IsBeast())
	{
		return;
	}

	if (Table && IsValid(SkillHandler))
	{
		if (SkillHandler->IsSkillActivated(FKMSkillKey(Table->TransformSkill, 0)))
		{
			return;
		}
	}

	if (!IsValid(BeastPDA))
	{
		return;
	}

	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UKMSkeletalMeshComponent* leaveSkeletalMeshComponnent = Cast<UKMSkeletalMeshComponent>(ownerCharacter->FindComponentByTag(UKMSkeletalMeshComponent::StaticClass(), LeaveSkeletalMeshCompTag));
	if (!IsValid(leaveSkeletalMeshComponnent))
	{
		leaveSkeletalMeshComponnent = Cast<UKMSkeletalMeshComponent>(
			ownerCharacter->AddComponentByClass(UKMSkeletalMeshComponent::StaticClass(), false, FTransform::Identity, false));
	}
	if (IsValid(leaveSkeletalMeshComponnent))
	{
		leaveSkeletalMeshComponnent->SetSkeletalMeshAsset(ownerCharacter->GetMesh()->GetSkeletalMeshAsset());
		leaveSkeletalMeshComponnent->SetAnimInstanceClass(ownerCharacter->GetMesh()->GetAnimClass());
		leaveSkeletalMeshComponnent->SetRelativeTransform(ownerCharacter->GetMesh()->GetRelativeTransform());
		leaveSkeletalMeshComponnent->SetReceivesDecals(false);
		leaveSkeletalMeshComponnent->SetRenderCustomDepth(true);
		leaveSkeletalMeshComponnent->SetCustomDepthStencilValue(1);
		SetCharacterDirectionVisual(GetCharacterDirection(), true, leaveSkeletalMeshComponnent);

		if (UKMAnimInstance* beforeAnimInstance = Cast<UKMAnimInstance>(ownerCharacter->GetMesh()->GetAnimInstance()))
		{
			FAnimMontageInstance* animMontageInstance = beforeAnimInstance->GetActiveMontageInstance();
			if (animMontageInstance)
			{
				if (UKMAnimInstance* newAnimInstance = Cast<UKMAnimInstance>(leaveSkeletalMeshComponnent->GetAnimInstance()))
				{
					newAnimInstance->Montage_Play(animMontageInstance->Montage, animMontageInstance->GetPlayRate(), EMontagePlayReturnType::MontageLength, animMontageInstance->GetPosition());
				}
			}
		}
		
		
		leaveSkeletalMeshComponnent->ComponentTags.Emplace(LeaveSkeletalMeshCompTag);
	}

	ownerCharacter->GetMesh()->EmptyOverrideMaterials();
	ownerCharacter->GetMesh()->SetAnimInstanceClass(BeastPDA->AnimInstanceClass);
	ownerCharacter->GetMesh()->SetSkeletalMesh(BeastPDA->Mesh);
	ownerCharacter->GetMesh()->SetRelativeScale3D(FVector(BeastTableRow->scale));
	ownerCharacter->GetMesh()->SetRenderInMainPass(false);
	ownerCharacter->GetMesh()->SetRenderInDepthPass(false);
	SetCharacterDirection(GetCharacterDirection(), true);

	ChangeSkillSet(BeastTableRow->Id);
	
	bIsBeast = true;
	if (GetWorld()->IsGameWorld())
	{
		if (BeastTableRow->TransformSkill != NAME_None)
		{
			SkillHandler->UseSkill(FKMSkillKey(BeastTableRow->TransformSkill, 0), nullptr);
		}

		if (StatModifier)
		{
			StatModifier->Compact();
		}

		UKMCurveWarpingComponent* curveWarping = Cast<UKMCurveWarpingComponent>(ownerCharacter->GetCurveWarping());
		if (IsValid(curveWarping))
		{
			UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(ownerCharacter->GetMesh()->GetAnimInstance());
			if (IsValid(animInstance))
			{
				float minRange = 0.f, maxRange = 0.f;
				UKMUtil::GetMinMaxValueBlendSpace1D(animInstance->MoveBlend, minRange, maxRange);
				UAnimSequence* animSequence = UKMUtil::GetAnimSequenceWithBlendSpace1D(animInstance->MoveBlend, maxRange);
				curveWarping->SetCustomMovementAnimation(animSequence);
				curveWarping->EnableCustomMovement();
			}
		}
		OnTransformToBeast();
	}
}

void UKMCharacterInstance::OnTransformToBeast_Implementation()
{
	
}

void UKMCharacterInstance::SetTable(const FKMTable_CharacterRow* newTable)
{
	Table = newTable;
}

const FKMTable_CharacterRow* UKMCharacterInstance::GetTable() const
{
	return Table;
}

FName UKMCharacterInstance::GetCharacterId() const
{
	if (IsBeast())
	{
		return BeastTableRow->Id;
	}
	return Table->Id;
}

UKMAnimationSetTag* UKMCharacterInstance::GetAnimsetTag() const
{
	if (IsBeast())
	{
		if(IsValid(BeastPDA))
		{
			return BeastPDA->AnimSet;
		}
	}
	return GetCharacter()->GetAnimsetTag();
}

void UKMCharacterInstance::SetDepthSort(float newDepthSort)
{
	DepthSort = newDepthSort;
}

float UKMCharacterInstance::GetDepthSort() const
{
	return DepthSort;
}

void UKMCharacterInstance::SetLocation(const FVector& newLocation)
{
	Transform.SetLocation(newLocation);
	UpdateTransform();
}

void UKMCharacterInstance::SetScale3D(const FVector& newScale3D)
{
	Transform.SetScale3D(newScale3D);
	UpdateTransform();
}

void UKMCharacterInstance::SetRotation(const FRotator& newRotation)
{
	Transform.SetRotation(newRotation.Quaternion());
	UpdateTransform();
}

void UKMCharacterInstance::SetTransform(const FTransform& newTransform)
{
	Transform = newTransform;
}

const FTransform& UKMCharacterInstance::GetTransform() const
{
	return Transform;
}

void UKMCharacterInstance::UpdateTransform()
{
	AKMCharacter* character = Cast<AKMCharacter>(Character);
	if (IsValid(character) == false)
	{
		return;
	}

	FTransform newTransform = Transform;
	newTransform.SetLocation(FVector(DepthSort, Transform.GetLocation().Y, Transform.GetLocation().Z));

	float depthScale = 1.f - FMath::Clamp(FMath::Abs((Transform.GetLocation().X - 200.f) / 5000.f), 0.0f, 0.75f);
	newTransform.SetScale3D(FVector(depthScale, 1.f, depthScale));

	character->SetActorRelativeTransform(newTransform);
}

void UKMCharacterInstance::StartForceMove(const float& newDirection)
{
	AKMCharacter* character = Cast<AKMCharacter>(Character);
	if (IsValid(character) == false)
	{
		return;
	}
	MoveAccelate = newDirection;
}

void UKMCharacterInstance::HitCheckClear()
{
	HitCheckData.Actors.Empty();	
}

void UKMCharacterInstance::HitCollection(AActor* hitActor,const FTransform& orientationTransform)
{
	if (GetCharacter() == hitActor)
	{
		return;
	}
	if (HitCheckData.Actors.Contains(hitActor))
	{
		return;
	}

	AKMCharacter* hitharacter = Cast<AKMCharacter>(hitActor);
	if (!IsValid(hitharacter))
	{
		return;
	}

	UKMCharacterInstance* hitCharacterInstance = hitharacter->GetCharacterInstance();
	check(IsValid(hitCharacterInstance));

	if (hitCharacterInstance->IsDead() ||
		//hitCharacterInstance->HasGameplayTag(FKMGameplayTagName::State_Blow_Tag) ||
		hitCharacterInstance->HasGameplayTag(FKMGameplayTagName::State_Intangible_Tag))
	{
		return;
	}

	HitCheckData.Actors.FindOrAdd(hitActor);

	Inflict(hitCharacterInstance);
	if (!SkillHandler->GetLatestActiveSkillInstance().IsValid())
	{
		return;
	}
	
	TSharedPtr<FKMSkillInstance> latestSkillInstance = MakeShared<FKMSkillInstance>(*SkillHandler->GetLatestActiveSkillInstance().Get());
	UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(hitharacter->GetRootComponent());
	FVector closestPoint;
	rootComp->GetClosestPointOnCollision(orientationTransform.GetLocation(), closestPoint);

	if (latestSkillInstance.IsValid())
	{
		latestSkillInstance->Target = MakeShared<FKMLockOnCluster>(this);
		latestSkillInstance->Target->Targets.Emplace(hitharacter->GetCharacterInstance()->GetId());
				
		UKMSkillHandler* hitCharacterSkillHandler = hitCharacterInstance->GetSkillHandler();
		check(IsValid(hitCharacterSkillHandler));

		hitCharacterInstance->Hit(this, latestSkillInstance, closestPoint);
	}
}

void UKMCharacterInstance::BoxHitImpact(const FTransform& orientationTransform, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter)
{
	if (objectTypeQuery.IsEmpty())
	{
		objectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	}
	if (!IsValid(actorClassFilter))
	{
		actorClassFilter = ACharacter::StaticClass();
	}

	TArray<AActor*> overlapActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Emplace(GetCharacter());
	if (UKismetSystemLibrary::BoxOverlapActorsWithOrientation(this, orientationTransform.GetLocation(),
		orientationTransform.GetScale3D(), orientationTransform.GetRotation().Rotator(), objectTypeQuery, actorClassFilter, ActorsToIgnore, overlapActors))
	{
		for (auto actorItr = overlapActors.CreateIterator(); actorItr; ++actorItr)
		{
			HitCollection(*actorItr, orientationTransform);
		}
	}
}

void UKMCharacterInstance::SphereHitImpact(const FTransform& orientationTransform, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeQuery, UClass* actorClassFilter)
{
	if (objectTypeQuery.IsEmpty())
	{
		objectTypeQuery.Emplace(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	}
	if (!IsValid(actorClassFilter))
	{
		actorClassFilter = ACharacter::StaticClass();
	}

	TArray<AActor*> overlapActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Emplace(GetCharacter());
	if (UKismetSystemLibrary::SphereOverlapActors(this, orientationTransform.GetLocation(),
		orientationTransform.GetScale3D().X * 100.f, objectTypeQuery, actorClassFilter, ActorsToIgnore, overlapActors))
	{
		for (auto actorItr = overlapActors.CreateIterator(); actorItr; ++actorItr)
		{
			HitCollection(*actorItr, orientationTransform);
		}
	}
}

void UKMCharacterInstance::Inflict(UKMCharacterInstance* victimCharacter)
{
	if (IsValid(victimCharacter) && !victimCharacter->HasGameplayTag(FKMGameplayTagName::State_Parry_Tag))
	{
		Stiff(0.15f);
	}

	TimingCancel = nullptr;
	if (SkillHandler->GetLatestActiveSkillInstance().IsValid())
	{
		TimingCancel = MakeShared<FKMTimingCancel>(this);
		TimingCancel->SetLatestSkillInstance(SkillHandler->GetLatestActiveSkillInstance());
	}
	InflictDelegate.Broadcast(++ComboCount, victimCharacter);
}

void UKMCharacterInstance::Hit(UKMCharacterInstance* attackerCharacterInstance, TSharedPtr<FKMSkillInstance> latestSkillInstance, const FVector& hitClosestPoint)
{
	if (UseParrySkill())
	{
		return;
	}

	if (HasGameplayTag(FKMGameplayTagName::State_Invincible_Tag))
	{
		return;
	}
	
	//SetCharacterDirection(UKMUtil::InverseCircularDirection(attackerCharacterInstance->GetCharacterDirection()));
	SkillHandler->ClearActiveSkills();

	TArray<TSharedPtr<FKMSkillEffectInstance>> skillEffectInstances = SkillHandler->ApplyEffects(latestSkillInstance, FKMGameplayTagName::Event_Hit_Tag);
	for (auto skillEffectItr : skillEffectInstances)
	{
		const FName typeName = skillEffectItr->GetType();
		if (skillEffectItr->GetType() != FKMSkillEffectAbnormalInstance::TypeName())
		{
			continue;
		}
		TSharedPtr<FKMSkillEffectInstance> skillEffectInstance = StaticCastSharedPtr<FKMSkillEffectInstance>(skillEffectItr);
		for (auto abilityItr : skillEffectInstance->GetUsedEffectAbilities())
		{
			UKMAbilityEffect* abilityEffect = Cast<UKMAbilityEffect>(abilityItr.Value);
			if (!IsValid(abilityEffect))
			{
				continue;
			}
	
			FTransform impactTransform;
			impactTransform.SetLocation(hitClosestPoint);
			abilityEffect->Impact(impactTransform);
		}
	}	
}

void UKMCharacterInstance::Stiff(float duration)
{
	if (StiffTimerHandle.IsValid())
	{
		return;
	}
	
	GetCharacter()->GetMesh()->bPauseAnims = true;
	GetCharacter()->GetMesh()->SuspendClothingSimulation();

	GetCharacter()->CustomTimeDilation = 0.f;
	GetWorld()->GetTimerManager().SetTimer(StiffTimerHandle, FTimerDelegate::CreateUObject(this, &UKMCharacterInstance::OnStiffRelease), duration, false);
}

void UKMCharacterInstance::OnStiffRelease()
{
	GetCharacter()->CustomTimeDilation = 1.f;
	GetCharacter()->GetMesh()->bPauseAnims = false;
	GetCharacter()->GetMesh()->ResumeClothingSimulation();
	GetWorld()->GetTimerManager().ClearTimer(StiffTimerHandle);
}

float UKMCharacterInstance::GetMoveAccelate() const
{
	if (IsDead())
	{
		return 0.f;
	}
	return MoveAccelate;
}

void UKMCharacterInstance::OnStatChange(EKMStatFactorType factorType, float prevValue, float newValue)
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

void UKMCharacterInstance::BroadCastDamageEvent(const FKMDamageEvent& newDamageEvent)
{
	if (DamageDelegate.IsBound())
	{
		DamageDelegate.Broadcast(newDamageEvent);
	}

	if (CombatMessageDelegate.IsBound())
	{
		if (newDamageEvent.bIsCritical)
		{
			CombatMessageDelegate.Broadcast(this, EKMCommbatMessageType::Critical, TEXT(""));
		}
	}
}

void UKMCharacterInstance::OnDeath()
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetCharacter());
	if (IsValid(ownerCharacter) == true)
	{
		GameplayTagContainer.AddTag(FKMGameplayTagName::State_Dead_Tag);
		GameplayTagContainer.AddTag(FKMGameplayTagName::Block_Control_Tag);
		if (!GameplayTagContainer.HasTag(FKMGameplayTagName::State_Blow_Tag))
		{
			if (const FKMTable_SkillEffect_NormalRow* skillEffectDieTableRow = FKMTable_SkillEffect_NormalRow::FindRowPtr(TEXT("eff_die")))
			{
				if (UKMAbility* deathAbility = Cast<UKMAbility>(GetStatModifier()->ApplyEffectiveAnimation(skillEffectDieTableRow->Ability.PdaKey)))
				{
					deathAbility->Activate();
				}
			}
		}
		ownerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
	StartForceMove(0.f);

	GetCharacterDeathDelegate().Broadcast(this);
}

bool UKMCharacterInstance::IsDead() const
{
	if (!IsValid(StatModifier) || StatModifier->GetEffectiveStat().GetHpCurr() > 0.f)
	{
		return false;
	}

	return true;
}

void UKMCharacterInstance::Tick(float deltaSeconds)
{
	AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetCharacter());
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	deltaSeconds *= ownerCharacter->CustomTimeDilation;
	
	Super::Tick(deltaSeconds);
	
	if (IsValid(GetCharacter()))
	{
		if (IsRun())
		{
			ownerCharacter->GetCharacterMovement()->MaxWalkSpeed = GetStatModifier()->GetEffectiveStat().GetRun();
		}
		else
		{
			ownerCharacter->GetCharacterMovement()->MaxWalkSpeed = GetStatModifier()->GetEffectiveStat().GetMov();
		}
	}

	StatModifier->ComputePreEffectStat();
	SkillHandler->Tick(deltaSeconds);
	StatModifier->ComputePostEffectStat();
	SensorInstance->SetCenterTransform(ownerCharacter->GetActorTransform());
}

bool UKMCharacterInstance::UseSkill(const FName skillName, int32 skillLevel)
{
	return UseSkillParam(skillName, skillLevel, TArray<FKMAssistSkillData>());
}

void UKMCharacterInstance::UseSkillDash(float dashDirection)
{
	if (TimingCancel.IsValid() && TimingCancel->IsComplete())
	{
		TimingCancel = nullptr;
	}
	
	if (TimingCancel.IsValid() && !TimingCancel->IsUsed())
	{
		EKM8WayDirection direction8way = UKMUtil::ConvertDegreesTo8WayDirection(dashDirection, GetCharacterDirection());

		FKMSkillKey dashSkillKey;
		EKMTimingResult cancelResult = TimingCancel->GetResult();
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
				SkillHandler->ClearActiveSkills();
				TSharedPtr<FKMSkillInstance> cancelSkillInstance = SkillHandler->UseSkill(dashSkillKey, nullptr);
				if (cancelSkillInstance.IsValid())
				{
					if (UKMAbilitySkillDirectionTag* abilitySkillDirectionTag = Cast<UKMAbilitySkillDirectionTag>(cancelSkillInstance->GetAbility()))
					{
						abilitySkillDirectionTag->ApplyAngle(direction8way, 150.f, 0.35f);
					}
				}
			}
			TimingCancel = nullptr;
		}
	}
	else if(!HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		SetCharacterDirection(dashDirection);
		SkillHandler->UseSkill(FKMSkillKey(TEXT("sk_front_dash"), 0), nullptr);
	}
}

bool UKMCharacterInstance::UseSkillParam(const FName skillName, int32 skillLevel, const TArray<FKMAssistSkillData> assistSkillData)
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

void UKMCharacterInstance::UseCombatSkill()
{
	GetSkillHandler()->UseCombatSkill(MakeShared<FKMLockOnCluster>(*LockonTarget.Get()));
}

void UKMCharacterInstance::UseUltimateSkill()
{
	GetSkillHandler()->UseUltimateSkill();
}

void UKMCharacterInstance::UseTechniqueSkill()
{
	GetSkillHandler()->UseTechniqueSkill(MakeShared<FKMLockOnCluster>(*LockonTarget.Get()));
}

bool UKMCharacterInstance::UseParrySkill()
{
	const FKMSkillKey guardSkillKey(TEXT("sk_stand_guard"), 0);
	if (SkillHandler->IsSkillActivated(guardSkillKey))
	{
		FKMSkillKey parrySkillKey;
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

bool UKMCharacterInstance::UseGuardSkill()
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

bool UKMCharacterInstance::UseGuardSkill_Release()
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

void UKMCharacterInstance::ShowDamage(EKMStatFactorType factorType, int32 damage)
{
	if (damage <= 0) return;
}

UKMSkillHandler* UKMCharacterInstance::GetSkillHandler() const
{
	return SkillHandler;
}

void UKMCharacterInstance::AddAggroTarget(UKMCharacterInstance* attacker)
{
	if (!AggroTarget.Contains(attacker))
	{
		AggroTarget.Add(attacker);
	}
}
const UKMCharacterInstance* UKMCharacterInstance::GetBestAggroTarget() const
{
	if (AggroTarget.IsEmpty())
	{
		return nullptr;
	}
	
	return AggroTarget.begin()->Get();
}

bool UKMCharacterInstance::IsBeast() const
{
	return bIsBeast;
}

void UKMCharacterInstance::Run()
{
	bIsRun = true;
}

bool UKMCharacterInstance::IsRun() const
{
	if (bIsBeast)
	{
		return true;
	}
	return bIsRun;
}

void UKMCharacterInstance::Walk()
{
	bIsRun = false;
}

bool UKMCharacterInstance::IsWalk() const
{
	if (bIsBeast)
	{
		return false;
	}
	return !bIsRun;
}

void UKMCharacterInstance::SetCharacterDirectionVisual(float direction, bool bForceRotate, USkeletalMeshComponent* otherSkeletalMeshComp)
{
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetCharacter()))
	{
		USkeletalMeshComponent* adjustSkeletalMeshComp = IsValid(otherSkeletalMeshComp) ? otherSkeletalMeshComp : ownerCharacter->GetMesh();
		if (!IsValid(adjustSkeletalMeshComp))
		{
			return;
		}
		
		if (UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(adjustSkeletalMeshComp->GetAnimInstance()))
		{
			animInstance->SetNextDirection(direction);
			if (bForceRotate == true)
			{
				animInstance->SetCurrentDirection(direction);
			}
			animInstance->UpdateAnimation(0.f, true);
		}
	}
}

void UKMCharacterInstance::SetCharacterDirection(float direction, bool bForceRotate)
{
	if (IsDead() && HasGameplayTag(FKMGameplayTagName::Block_Control_Tag))
	{
		return;
	}
	
	if (FMath::IsNearlyEqual(CharacterDirection, direction) && !bForceRotate)
	{
		return;
	}

	CharacterDirection = direction;
	
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetCharacter()))
	{
		float angle = CharacterDirection * PI * 2.f;

		FVector newForwardVector(FMath::Cos(angle), FMath::Sin(angle),0.f);

		FRotator newRotation = newForwardVector.Rotation();
		ownerCharacter->SetActorRotation(newRotation);
		ownerCharacter->GetRootComponent()->UpdateComponentToWorld();

		SetCharacterDirectionVisual(direction, bForceRotate);
		if (!ownerCharacter->GetMesh()->IsPostEvaluatingAnimation())
		{
			ownerCharacter->GetMesh()->RefreshBoneTransforms();
		}
	}
}

float UKMCharacterInstance::GetCharacterDirection() const
{
	return CharacterDirection;
}

void UKMCharacterInstance::OnSensorResult(const TArray<AActor*>& resultActors)
{
	check(LockonTarget.IsValid());
	
	LockonTarget->Targets.Empty();
	for (auto actorItr = resultActors.CreateConstIterator(); actorItr; ++actorItr)
	{
		AKMCharacter* targetCharacter = Cast<AKMCharacter>(*actorItr);
		if (!IsValid(targetCharacter))
		{
			continue;
		}

		UKMCharacterInstance* targetCharacterInstance = targetCharacter->GetCharacterInstance();
		if (!IsValid(targetCharacterInstance))
		{
			continue;
		}

		if (targetCharacterInstance->IsDead() || targetCharacterInstance->HasGameplayTag(FKMGameplayTagName::State_Blow_Tag))
		{
			continue;
		}
		
		LockonTarget->Targets.Emplace(targetCharacterInstance->GetId());
		break;
	}
}


void UKMCharacterInstance::RemoveGameplayTag(FGameplayTag Tag)
{
	Super::RemoveGameplayTag(Tag);

	if (Tag == FKMGameplayTagName::Event_Cancel_Combo_Available_Tag)
	{
		if (IsValid(SkillHandler))
		{
			SkillHandler->ActivatedNextComboSkill();
		}
	}
}

void UKMCharacterInstance::OnUpdatePawnThrowOverlapResults(const TArray<FHitResult>& hitResults)
{
	
}

void UKMCharacterInstance::OnAddGameplayTag_Implementation(const FGameplayTag& newTag)
{
	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	if (newTag == FKMGameplayTagName::State_Thrown_OverlapDamage_Tag)
	{
		UKMCharacterMovementComponent* characterMoveComponent = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
		if (!characterMoveComponent)
		{
			return;
		}
		
		if (GetGameplayTagCount(newTag) == 1)
		{
			ThrowOverlapActors.Reset();
			characterMoveComponent->SweepPawnHitDelegate.RemoveAll(this);
			characterMoveComponent->SweepPawnHitDelegate.AddUObject(this, &ThisClass::OnUpdatePawnThrowOverlapResults);
		}
	}
	else if (newTag == FKMGameplayTagName::Event_Item_Launch)
	{
		if (IsValid(ownerCharacter->WeaponInstance))
		{
			ownerCharacter->WeaponInstance->Launch();
		}
	}
}

void UKMCharacterInstance::OnRemoveGameplayTag_Implementation(const FGameplayTag& removedTag)
{
	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}
	
	if (removedTag == FKMGameplayTagName::State_Thrown_OverlapDamage_Tag)
	{
		UKMCharacterMovementComponent* characterMoveComponent = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
		if (!characterMoveComponent)
		{
			return;
		}
		
		if (GetGameplayTagCount(removedTag) == 0)
		{
			ThrowOverlapActors.Reset();
			characterMoveComponent->SweepPawnHitDelegate.RemoveAll(this);
		}
	}
	else if (removedTag == FKMGameplayTagName::Event_Item_Launch)
	{
		if (IsValid(ownerCharacter->WeaponInstance))
		{
			ownerCharacter->WeaponInstance->LaunchStop();
		}
	}
}
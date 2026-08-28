#include "KMCharacterInstance.h"
#include <Tables/Generated/KMTable_Object_Beast.h>
#include "Animation/KMAnimInstance.h"
#include "Camera/KMPlayerCameraManager.h"
#include "Component/KMCharacterMovementComponent.h"
#include "Component/KMSkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/KMAssetManager.h"
#include "DataAsset/KMBeastPDA.h"
#include "GameActor/Item/KMItemAppearanceActor.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Skill/KMSkillHandler.h"
#include "Skill/Ability/KMAbility.h"
#include "Skill/Ability/KMAbilitySkillDirectionTag.h"
#include "Skill/Parry/KMTiming.h"
#include "Skill/Sensor/KMSensor.h"
#include "Stat/KMStatModifierBase.h"
#include "System/KMTargetSubsystem.h"
#include "Tables/Generated/KMTable_BaseStat_Beast.h"
#include "Tables/Generated/KMTable_Object.h"
#include "Tables/Generated/KMTable_Object_Character.h"
#include "Tables/Generated/KMTable_SkillSet.h"
#include "Util/KMUtil.h"

const FName LeaveSkeletalMeshCompTag = TEXT("LeaveSkeletalMeshComp");

UKMCharacterInstance::UKMCharacterInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SensorClass = UKMSensor::StaticClass();
}

const FKMTable_Object_CharacterRow* UKMCharacterInstance::GetCharacterTable() const
{
	return CastRow<FKMTable_Object_CharacterRow>(GetTable());
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

	const FKMTable_Object_CharacterRow* chracterTableRow = GetCharacterTable();
	check(chracterTableRow);
	
	ChangeSkillSet(chracterTableRow->Id);
	
	SensorInstance = NewObject<UKMSensor>(this, SensorClass, TEXT("Sensor"));
	SensorInstance->ResultDelegate.BindUObject(this, &ThisClass::OnSensorResult);
	SensorInstance->Init();
	LockonTarget = MakeShared<FKMLockOnCluster>(this);

	SetBeastTableId(chracterTableRow->DefaultBeast);
}

void UKMCharacterInstance::EndPlay()
{
	if (IsValid(StatModifier))
	{
		StatModifier->GetEffectiveStat().StatChange.RemoveAll(this);
	}

	if (AKMCharacter* ownerCharacter = GetCharacter())
	{
		ownerCharacter->Destroy();
	}

	Super::EndPlay();
}

AKMCharacter* UKMCharacterInstance::GetCharacter() const
{
	return Cast<AKMCharacter>(OwnerActor);
}

void UKMCharacterInstance::SetBeastTableId(FName newBeastId)
{
	BeastId = newBeastId;

	BeastTableRow = FKMTable_Object_BeastRow::FindRowPtr(BeastId);
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

	if (AKMCharacter* ownerCharacter = GetCharacter())
	{
		ownerCharacter->SetBeastPDA(Cast<UKMBeastPDA>(assetManager->GetAsset(BeastTableRow->AssetPda)));	
	}
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

	UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
	if (!IsValid(characterMovement))
	{
		return;
	}

	AKMCharacter* ownerCharacterCDO = GetCharacter()->GetClass()->GetDefaultObject<AKMCharacter>();
	if (!IsValid(ownerCharacterCDO))
	{
		return;
	}

	const FKMTable_Object_CharacterRow* characterTableRow = GetCharacterTable();
	if (!characterTableRow)
	{
		return;
	}

	bIsBeast = false;
	ChangeSkillSet(GetTableId());

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
		SetDirectionVisual(GetDirection(), true, leaveSkeletalMeshComponnent);
		leaveSkeletalMeshComponnent->SetReceivesDecals(false);
		leaveSkeletalMeshComponnent->SetRenderCustomDepth(true);
		leaveSkeletalMeshComponnent->SetCustomDepthStencilValue(1);
		leaveSkeletalMeshComponnent->ComponentTags.Emplace(LeaveSkeletalMeshCompTag);
	}
	
	ownerCharacter->GetMesh()->EmptyOverrideMaterials();
	ownerCharacter->GetMesh()->SetAnimInstanceClass(ownerCharacterCDO->GetMesh()->GetAnimClass());
	ownerCharacter->GetMesh()->SetSkeletalMesh(ownerCharacterCDO->GetMesh()->GetSkeletalMeshAsset());
	ownerCharacter->GetMesh()->SetRelativeScale3D(FVector(characterTableRow->scale));
	SetDirection(GetDirection(), true);
	
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

		characterMovement->ClearCustomWalkingAnimation();
		characterMovement->DisableCustomWalking();

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

	const FKMTable_Object_CharacterRow* characterTableRow = GetCharacterTable();
	if (!characterTableRow)
	{
		return;
	}

	if (characterTableRow && IsValid(SkillHandler))
	{
		if (SkillHandler->IsSkillActivated(FKMSkillKey(characterTableRow->TransformSkill, 0)))
		{
			return;
		}
	}
	
	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	const UKMBeastPDA* beastPDA = ownerCharacter->GetBeastPDA();
	if (!IsValid(beastPDA))
	{
		return;
	}

	UKMCharacterMovementComponent* characterMovement = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
	if (!IsValid(characterMovement))
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
		SetDirectionVisual(GetDirection(), true, leaveSkeletalMeshComponnent);

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
	ownerCharacter->GetMesh()->SetAnimInstanceClass(beastPDA->AnimInstanceClass);
	ownerCharacter->GetMesh()->SetSkeletalMesh(beastPDA->Mesh);
	ownerCharacter->GetMesh()->SetRelativeScale3D(FVector(BeastTableRow->scale));
	ownerCharacter->GetMesh()->SetRenderInMainPass(false);
	ownerCharacter->GetMesh()->SetRenderInDepthPass(false);
	SetDirection(GetDirection(), true);

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

		UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(ownerCharacter->GetMesh()->GetAnimInstance());
		if (IsValid(animInstance))
		{
			float minRange = 0.f, maxRange = 0.f;
			UKMUtil::GetMinMaxValueBlendSpace1D(animInstance->MoveBlend, minRange, maxRange);
			UAnimSequence* animSequence = UKMUtil::GetAnimSequenceWithBlendSpace1D(animInstance->MoveBlend, maxRange);
			characterMovement->SetCustomWalkingAnimation(animSequence);
			characterMovement->EnableCustomWalking();
		}
		OnTransformToBeast();
	}
}

void UKMCharacterInstance::OnTransformToBeast_Implementation()
{
	
}

FName UKMCharacterInstance::GetTableId() const
{
	if (IsBeast())
	{
		if (!BeastStatTableRow)
		{
			return NAME_None;
		}
		return BeastTableRow->Id;
	}
	return Super::GetTableId();
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

FTransform UKMCharacterInstance::GetTransform() const
{
	return Transform;
}

void UKMCharacterInstance::UpdateTransform()
{
	AKMCharacter* character = Cast<AKMCharacter>(GetCharacter());
	if (!IsValid(character))
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
	AKMCharacter* character = Cast<AKMCharacter>(OwnerActor);
	if (!IsValid(character))
	{
		return;
	}
	
	MoveAccelate = newDirection;
}

void UKMCharacterInstance::HitCollection(const TWeakPtr<FKMSkillInstance>& adjustSkillInstance,
	AActor* hitActor,const FVector& hitLocation, const FVector& hitNormal, const FName& hitTag)
{
	Super::HitCollection(adjustSkillInstance, hitActor, hitLocation, hitNormal, hitTag);
}

TSubclassOf<UCameraShakeBase> UKMCharacterInstance::GetCameraShakeByPowerType(EKMDamagePowerType powerType) const
{
	const TSubclassOf<UCameraShakeBase>* existCameraShakeClass = InflictCameraShakes.Find(powerType);
	if (!existCameraShakeClass || !IsValid(*existCameraShakeClass))
	{
		return nullptr;
	}
	return *existCameraShakeClass; 
}

void UKMCharacterInstance::Inflict(UKMGameObjectInstance* victimGameObject)
{
	if (IsValid(victimGameObject) && !victimGameObject->HasGameplayTag(FKMGameplayTagName::State_Parry_Tag))
	{
		Stiff(0.1f);
	}

	TimingCancel = nullptr;
	if (SkillHandler->GetLatestActiveSkillInstance().IsValid())
	{
		TimingCancel = MakeShared<FKMTimingCancel>(this);
		TimingCancel->SetLatestSkillInstance(SkillHandler->GetLatestActiveSkillInstance());
	}
	InflictDelegate.Broadcast(++ComboCount, victimGameObject);
}

void UKMCharacterInstance::Hit(UKMGameObjectInstance* attackerGameObjectInstance, TSharedPtr<FKMSkillInstance> latestSkillInstance, const FVector& hitClosestPoint, const FName& hitTag)
{
	if (UseParrySkill())
	{
		return;
	}
	
	Super::Hit(attackerGameObjectInstance, latestSkillInstance, hitClosestPoint, hitTag);
}

void UKMCharacterInstance::ShakeRoot(float newDistance, float newFrequency, float newDuration)
{
	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return;
	}

	UKMAnimInstance* animInstance = Cast<UKMAnimInstance>(ownerCharacter->GetMesh()->GetAnimInstance());
	if (!IsValid(animInstance))
	{
		return;
	}

	animInstance->StartShake(newDistance, newFrequency, newDuration);
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
	Super::OnStatChange(factorType, newValue, prevValue);
}

void UKMCharacterInstance::BroadCastDamageEvent(const FKMDamageEvent& newDamageEvent)
{
	Super::BroadCastDamageEvent(newDamageEvent);

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
	Super::OnDeath();

	if (AKMCharacter* ownerCharacter = GetCharacter())
	{
		ownerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
	StartForceMove(0.f);
}

bool UKMCharacterInstance::IsDead() const
{
	if (!IsValid(StatModifier) || StatModifier->GetEffectiveStat().GetHpCurr() > 0.f)
	{
		return false;
	}

	return true;
}

bool UKMCharacterInstance::IsAir() const
{
	AKMCharacter* ownerCharacter = GetCharacter();
	if (!IsValid(ownerCharacter))
	{
		return false;
	}
	UKMCharacterMovementComponent* ownerCharacterMovementComponent = Cast<UKMCharacterMovementComponent>(ownerCharacter->GetCharacterMovement());
	if (!IsValid(ownerCharacterMovementComponent))
	{
		return false;
	}
	return ownerCharacterMovementComponent->IsAir();
}

void UKMCharacterInstance::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	
	AKMCharacter* ownerCharacter = GetCharacter();
	if (IsValid(ownerCharacter))
	{
		if (IsRun())
		{
			ownerCharacter->GetCharacterMovement()->MaxWalkSpeed = GetStatModifier()->GetEffectiveStat().GetRun();
		}
		else
		{
			ownerCharacter->GetCharacterMovement()->MaxWalkSpeed = GetStatModifier()->GetEffectiveStat().GetMov();
		}
		SensorInstance->SetCenterTransform(ownerCharacter->GetActorTransform());

		if (InflectPowerType != EKMDamagePowerType::None)
		{
			if (AKMPlayerCameraManager* playerCameraManager = AKMPlayerCameraManager::GetActiveCameraManager(this))
			{
				TSubclassOf<UCameraShakeBase> cameraShakeClass = GetCameraShakeByPowerType(InflectPowerType);
				if (!IsValid(cameraShakeClass))
				{
					cameraShakeClass = GetCameraShakeByPowerType(EKMDamagePowerType::Default);
				}
				
				if (IsValid(cameraShakeClass))
				{
					playerCameraManager->PlayWorldCameraShake(GetWorld(), cameraShakeClass, ownerCharacter->GetActorLocation(), 1500.f, 1500.f, false);
				}
			}
		}
	}

	if (HitPowerType != EKMDamagePowerType::None)
	{
		ShakeRoot(5.f, 10.f, 0.2f);
	}

	if (InflectPowerType != EKMDamagePowerType::None || HitPowerType != EKMDamagePowerType::None)
	{
		Stiff(0.1f);
	}

	InflectPowerType = EKMDamagePowerType::None;
	HitPowerType = EKMDamagePowerType::None;
}

bool UKMCharacterInstance::UseSkill(const FName skillName, int32 skillLevel)
{
	return UseSkillParam(skillName, skillLevel, TArray<FKMAssistSkillData>());
}

void UKMCharacterInstance::UseSkillDash(float dashDirection)
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
	if (HasGameplayTag(FKMGameplayTagName::Block_Control_Run_Tag))
	{
		return false;
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
	if (HasGameplayTag(FKMGameplayTagName::Block_Control_Walk_Tag))
	{
		return false;
	}
	return !bIsRun;
}

void UKMCharacterInstance::SetDirectionVisual(float direction, bool bForceRotate, USkeletalMeshComponent* otherSkeletalMeshComp)
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
			if (bForceRotate)
			{
				animInstance->SetCurrentDirection(direction);
			}
			animInstance->UpdateAnimation(0.f, true);
		}
	}
}

void UKMCharacterInstance::SetDirection(float direction, bool bForceRotate)
{
	if (IsDead())
	{
		return;
	}

	if (HasGameplayTag(FKMGameplayTagName::Block_Control_Rotation_Tag))
	{
		return;
	}

	if (FMath::IsNearlyEqual(Direction, direction) && !bForceRotate)
	{
		return;
	}

	Direction = direction;
	
	if (AKMCharacter* ownerCharacter = Cast<AKMCharacter>(GetCharacter()))
	{
		float angle = Direction * PI * 2.f;

		FVector newForwardVector(FMath::Cos(angle), FMath::Sin(angle),0.f);

		FRotator newRotation = newForwardVector.Rotation();
		ownerCharacter->SetActorRotation(newRotation);
		ownerCharacter->GetRootComponent()->UpdateComponentToWorld();

		SetDirectionVisual(direction, bForceRotate);
		if (!ownerCharacter->GetMesh()->IsPostEvaluatingAnimation())
		{
			ownerCharacter->GetMesh()->RefreshBoneTransforms();
		}
	}
}

float UKMCharacterInstance::GetDirection() const
{
	return Direction;
}

void UKMCharacterInstance::OnSensorResult(const TArray<AActor*>& resultActors)
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
		if (UKMItemAppearanceInstance* weaponInstance = ownerCharacter->GetWeaponInstance())
		{
			weaponInstance->Launch();
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
		if (UKMItemAppearanceInstance* weaponInstance = ownerCharacter->GetWeaponInstance())
		{
			weaponInstance->LaunchStop();
		}
	}
}
#include "KMSpawnSubsystem.h"
#include "KMGameObjectSubsystem.h"
#include "KMPlayerSubsystem.h"
#include "KMTargetSubsystem.h"
#include "DataAsset/KMAssetManager.h"
#include "GameActor/Pawn/Character/KMCharacter.h"
#include "GameActor/Projectile/KMProjectileActorBase.h"
#include "GameObject/KMActorInstance.h"
#include "GameObject/KMGameObjectInstance.h"
#include "GameObject/KMHeroInstance.h"
#include "GameObject/KMMonsterInstance.h"
#include "Skill/KMSkillHandler.h"
#include "Tables/Generated/KMTable_Object_Character_Hero.h"
#include "Tables/Generated/KMTable_Skill.h"
#include "Tables/Generated/KMTable_Skill_Projectile.h"

UKMSpawnSubsystem* UKMSpawnSubsystem::GetSpawnSubsystem(const UObject* worldContextObject)
{
	return Cast<UKMSpawnSubsystem>(GetSubsystem(worldContextObject, UKMSpawnSubsystem::StaticClass()));
}

void UKMSpawnSubsystem::Initialize()
{
	Super::Initialize();

	UKMSkillHandler::GetProjectileTriggerDelegate().AddUObject(this, &UKMSpawnSubsystem::OnSkillProjectileTrigger);
}

void UKMSpawnSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UKMMonsterInstance::GetSpawnPurifyDelegate().RemoveAll(this);
	UKMGameObjectInstance::GetDeathDelegate().RemoveAll(this);
	UKMSkillHandler::GetProjectileTriggerDelegate().RemoveAll(this);
}

UKMHeroInstance* UKMSpawnSubsystem::SpawnHero(const FName& heroId)
{
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));
	
	const FKMTable_Object_Character_HeroRow* heroTable = FKMTable_Object_Character_HeroRow::FindRowPtr(heroId);
	check(heroTable);

	UKMHeroInstance* heroInstance = Cast<UKMHeroInstance>(gameObjectSubsystem->SpawnCharacterObject(heroId, FTransform::Identity));
	check(IsValid(heroInstance));
	
	return heroInstance;
}

UKMActorInstance* UKMSpawnSubsystem::SpawnProjectile(TSubclassOf<AActor> actorClass, const FTransform& transform)
{
	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UKMActorInstance* actorInstance = Cast<UKMActorInstance>(gameObjectSubsystem->SpawnActorObject(actorClass, transform));

	return actorInstance;
}

void UKMSpawnSubsystem::OnSkillProjectileTrigger(const TSharedPtr<FKMSkillInstance>& skillInstance)
{
	if (!skillInstance.IsValid())
	{
		return;
	}

	const FKMTable_Skill_ProjectileRow* projectileSkillTable = CastRow<FKMTable_Skill_ProjectileRow>(skillInstance->SkillKey.TableRecord);
	if (!projectileSkillTable)
	{
		return;
	}
	
	UKMAssetManager* AssetManager = UKMAssetManager::GetAssetManager();
	check(IsValid(AssetManager));

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UClass* projectileClass = Cast<UClass>(AssetManager->GetAsset(projectileSkillTable->ProjectileBp));
	if (!IsValid(projectileClass))
	{
		return;
	}

	UKMCharacterInstance* casterCharacterInstance = Cast<UKMCharacterInstance>(gameObjectSubsystem->GetGameObject(skillInstance->Caster));
	check(IsValid(casterCharacterInstance));

	AKMCharacter* character = casterCharacterInstance->GetCharacter();
	check(IsValid(character));

	float pitchAngle = 0.f;
	if (FName* tagValue = skillInstance->Tags.Find(TEXT("ThrowAngle")))
	{
		pitchAngle = FCString::Atof(*tagValue->ToString());
	}

	FTransform startTransform = character->GetMesh()->GetSocketTransform(projectileSkillTable->TransformSocket);
	startTransform.SetTranslation(FVector(startTransform.GetLocation().X, 0.f, startTransform.GetLocation().Z));
	startTransform.SetRotation(FRotator(pitchAngle, character->GetActorRotation().Yaw, 0.f).Quaternion());
	
	AKMProjectileActorBase* newProjectileActor = GetWorld()->SpawnActorDeferred<AKMProjectileActorBase>(
		projectileClass, startTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	check(IsValid(newProjectileActor));

	newProjectileActor->FinishSpawning(startTransform);
	newProjectileActor->SkillInstance = skillInstance;
	newProjectileActor->TriggerDeletgate.AddDynamic(this, &ThisClass::OnSkillProjectileEvent);
}

void UKMSpawnSubsystem::OnSkillProjectileEvent(FGameplayTag eventTag, const FKMProjectileEventData& eventData)
{
	check(eventData.movementComponent.IsValid());

	AKMProjectileActorBase* projectileActor = Cast<AKMProjectileActorBase>(eventData.movementComponent->GetOwner());
	check(IsValid(projectileActor));

	check(projectileActor->SkillInstance);
	check(projectileActor->SkillInstance->GetType() == FKMAssistSkillInstance::TypeName());

	TSharedPtr<FKMAssistSkillInstance> assistSkillInstance = StaticCastSharedPtr<FKMAssistSkillInstance>(projectileActor->SkillInstance); 

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	check(IsValid(gameObjectSubsystem));

	UKMCharacterInstance* characterInstance =Cast<UKMCharacterInstance>(gameObjectSubsystem->GetGameObject(projectileActor->SkillInstance->Caster));
	check(IsValid(characterInstance));

	UKMSkillHandler* skillHandler = characterInstance->GetSkillHandler();
	check(IsValid(skillHandler));

	AKMCharacter* targetCharacter = Cast<AKMCharacter>(eventData.TargetActor);
	if (IsValid(targetCharacter))
	{
		if (!targetCharacter->GetCharacterInstance()->IsDead())
		{
			assistSkillInstance->Target  = MakeShared<FKMLockOnCluster>(skillHandler);
			assistSkillInstance->Target->Targets.Emplace(targetCharacter->GetCharacterInstance()->GetId());
		
			skillHandler->ApplyEffects(projectileActor->SkillInstance, FKMGameplayTagName::Event_Hit_Projectile_Tag);
		}
	}
	assistSkillInstance->SetForceComplete(true);
}
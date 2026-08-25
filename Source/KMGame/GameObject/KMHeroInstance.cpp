#include "KMHeroInstance.h"
#include "Engine/GameInstance.h"
#include "GameActor/Pawn/Character/KMCharacterHero.h"
#include "Skill/KMSkillHandler.h"
#include "Stat/KMHeroStatModifier.h"
#include "System/KMSkillSubsystem.h"
#include "Util/KMUtil.h"

UKMHeroInstance::UKMHeroInstance(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	StatModifierClass = UKMHeroStatModifier::StaticClass();
}

void UKMHeroInstance::BeginPlay()
{
	Super::BeginPlay();

	if (const auto& skillSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UKMSkillSubsystem>())
	{
		//skillSubsystem->RegisterHeroInstance(this);
	}
}
void UKMHeroInstance::EndPlay()
{
	Super::EndPlay();
	
}

bool UKMHeroInstance::CanLockOn() const
{
	return true;
}

bool UKMHeroInstance::CanBeTargeted() const
{
	return true;
}

void UKMHeroInstance::ApplyPlayerSkill(const FName& skillId, int32 skillLevel)
{
	if (SkillHandler == nullptr) return;
	
	SkillHandler->RegisterSkill(FKMSkillKey::CreateKey(skillId, skillLevel));
}

void UKMHeroInstance::OnHeroSelected_Implementation()
{
	if (AKMCharacterHero* hero = Cast<AKMCharacterHero>(GetOwnerActor()))
	{
		hero->OnHeroSelected();
	}
}

void UKMHeroInstance::OnEnterGame_Implementation()
{
	if (AKMCharacterHero* hero = Cast<AKMCharacterHero>(GetOwnerActor()))
	{
		hero->OnEnterGame();
	}
}

void UKMHeroInstance::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

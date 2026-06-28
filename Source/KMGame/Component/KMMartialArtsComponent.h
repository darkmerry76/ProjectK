#pragma once

#include "CoreMinimal.h"
#include "EMMartialArtsComponent.h"
#include "KMMartialArtsComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMMartialArtsSkillContextData
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class KMGAME_API  FKMMartialArtsSkillContextData : public FEMMartialArtsContextData
{
public:
	FKMMartialArtsSkillContextData(class UEMAbility* ability);
	virtual ~FKMMartialArtsSkillContextData() override { };
	
	virtual FName GetTypeName() const override { return FKMMartialArtsSkillContextData::GetStaticTypeName(); };
	inline static FName GetStaticTypeName() { return TEXT("KMMartialArtsSkillContextData"); };

	class UEMAbility* GetAbility() const;
	
protected:
	TWeakObjectPtr<class UEMAbility> Ability;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMMartialArtsComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS()
class KMGAME_API UKMMartialArtsComponent : public UEMMartialArtsComponent
{
	GENERATED_UCLASS_BODY()
};
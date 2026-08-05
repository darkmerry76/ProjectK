#pragma once

#include "CoreMinimal.h"
#include "EMMartialArtsComponent.h"
#include "KMMartialArtsComponent.generated.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMMartialArtsSkillContextData
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class KMGAME_API FKMMartialArtsSkillContextData : public UE::Anim::IAnimNotifyEventContextDataInterface
{
public:
	FKMMartialArtsSkillContextData(class UEMAbility* ability);
	virtual ~FKMMartialArtsSkillContextData() override { };

public:
	static FName GetStaticTypeName() { return FKMMartialArtsSkillContextData::SkillTypeName; }
	virtual FName GetTypeName() const override { return FKMMartialArtsSkillContextData::GetStaticTypeName(); }
	
private:
	inline static const FName SkillTypeName = TEXT("FKMMartialArtsSkillContextData");
	
public:
	class UEMAbility* GetAbility() const;
	
protected:
	TWeakObjectPtr<class UEMAbility> Ability;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FKMMartialArtsInstance
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class KMGAME_API FKMMartialArtsInstance : public FEMMartialArtsInstance
{
public:
	FKMMartialArtsInstance(class UEMMartialArtsComponent* ownerComponent, int32 id, class UEMMartialArts* newMartialArts);
	virtual ~FKMMartialArtsInstance() override { };
	
protected:
	virtual void OnAddContextData(FAnimNotifyEventReference& eventReference, const TSharedPtr<const UE::Anim::IAnimNotifyEventContextDataInterface>& newContextData) override;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UKMMartialArtsComponent
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCLASS()
class KMGAME_API UKMMartialArtsComponent : public UEMMartialArtsComponent
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual TSharedPtr<FEMMartialArtsInstance> CreateInstance(class UEMMartialArts* martialArts, int32 newId) override;
};
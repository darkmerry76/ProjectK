#pragma once

#include "CoreMinimal.h"
#include "KMTiming.generated.h"

class KMGAME_API FKMTimingControlBase : public FGCObject
{
public:
	FKMTimingControlBase(class UKMGameObjectInstance* ownerGameObjectInstance);
	virtual ~FKMTimingControlBase() override;

public:
	float GetElapsedTime() const;

protected:
	virtual void AddReferencedObjects(FReferenceCollector& collector)  override;
	virtual FString GetReferencerName() const override;

protected:
	double StartTimeSeconds = 0.f; 
	TWeakObjectPtr<class UKMGameObjectInstance> OwnerGameObjectInstance;
};

UENUM(BlueprintType, Blueprintable)
enum class EKMTimingResult : uint8
{
	None,
	Good,
	Great,
	Perfect,
};

class KMGAME_API FKMTimingParry : public FKMTimingControlBase
{
public:
	FKMTimingParry(class UKMGameObjectInstance* ownerGameObjectInstance);
	virtual ~FKMTimingParry() override;

public:
	EKMTimingResult GetResult() const;

	void SetGuardSkillInstance(const TSharedPtr<class FKMSkillInstance>& newGuardSkillInstance);
	TWeakPtr<class FKMSkillInstance> GetGuardSkillInstance() const;

	void SetUsed(bool bUsed);
	bool IsUsed() const;

protected:
	virtual FString GetReferencerName() const override;

protected:
	TWeakPtr<class FKMSkillInstance> GuardSkillInstance;
	bool bIsUsed = false;
};

class KMGAME_API FKMTimingCancel : public FKMTimingControlBase
{
public:
	FKMTimingCancel(class UKMCharacterInstance* ownerCharacterInstance);
	virtual ~FKMTimingCancel() override;

protected:
	virtual FString GetReferencerName() const override;
	
public:
	EKMTimingResult GetResult() const;
	void SetLatestSkillInstance(const TWeakPtr<class FKMSkillInstance>& newLatestSkillInstance);
	TWeakPtr<class FKMSkillInstance> GetLatestSkillInstance() const;

	bool IsComplete() const;

	void SetUsed(bool bUsed);
	bool IsUsed() const;

protected:
	TWeakPtr<class FKMSkillInstance> LatestSkillInstance;
	bool bIsUsed = false;
};
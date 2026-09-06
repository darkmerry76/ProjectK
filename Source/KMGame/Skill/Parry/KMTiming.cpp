#include "KMTiming.h"
#include "GameObject/KMCharacterInstance.h"

FKMTimingControlBase::FKMTimingControlBase(UKMGameObjectInstance* ownerGameObjectInstance) : OwnerGameObjectInstance(ownerGameObjectInstance)
{
	StartTimeSeconds = ownerGameObjectInstance->GetWorld()->GetTimeSeconds();
}

FKMTimingControlBase::~FKMTimingControlBase()
{
}

void FKMTimingControlBase::AddReferencedObjects(FReferenceCollector& collector)
{
	collector.AddReferencedObject(OwnerGameObjectInstance);
}

FString FKMTimingControlBase::GetReferencerName() const
{
	return TEXT("FKMTimingControlBase");	
}

float FKMTimingControlBase::GetElapsedTime() const
{
	return OwnerGameObjectInstance->GetWorld()->GetTimeSeconds() - StartTimeSeconds;
}

FKMTimingParry::FKMTimingParry(UKMGameObjectInstance* ownerGameObjectInstance) : FKMTimingControlBase(ownerGameObjectInstance)
{
}

FKMTimingParry::~FKMTimingParry()
{
	
}

FString FKMTimingParry::GetReferencerName() const
{
	return TEXT("FKMTimingParry");	
}

EKMTimingResult FKMTimingParry::GetResult() const
{
	if (!OwnerGameObjectInstance.IsValid())
	{
		return EKMTimingResult::None;
	}
	
	float elapsedTime = OwnerGameObjectInstance->GetWorld()->GetTimeSeconds() - StartTimeSeconds;
	
	if (elapsedTime < OwnerGameObjectInstance->GetStatModifier()->GetEffectiveStat().GetPerfectParry())
	{
		return EKMTimingResult::Perfect;
	}
	else if (elapsedTime < OwnerGameObjectInstance->GetStatModifier()->GetEffectiveStat().GetGreatParry())
	{
		return EKMTimingResult::Great;
	}

	return EKMTimingResult::Good;
}

void FKMTimingParry::SetGuardSkillInstance(const TSharedPtr<FKMSkillInstance>& newGuardSkillInstance)
{
	GuardSkillInstance = newGuardSkillInstance;
}

TWeakPtr<FKMSkillInstance> FKMTimingParry::GetGuardSkillInstance() const
{
	return GuardSkillInstance;
}

void FKMTimingParry::SetUsed(bool bUsed)
{
	bIsUsed = bUsed;
}

bool FKMTimingParry::IsUsed() const
{
	return bIsUsed;
}

FKMTimingCancel::FKMTimingCancel(UKMCharacterInstance* ownerCharacterInstance) : FKMTimingControlBase(ownerCharacterInstance)
{
}

FKMTimingCancel::~FKMTimingCancel()
{
}

FString FKMTimingCancel::GetReferencerName() const
{
	return TEXT("FKMTimingCancel");	
}

void FKMTimingCancel::SetLatestSkillInstance(const TWeakPtr<FKMSkillInstance>& newLatestSkillInstance)
{
	LatestSkillInstance = newLatestSkillInstance;
}

TWeakPtr<FKMSkillInstance> FKMTimingCancel::GetLatestSkillInstance() const
{
	return LatestSkillInstance;
}

bool FKMTimingCancel::IsComplete() const
{
	return !LatestSkillInstance.IsValid();
}

void FKMTimingCancel::SetUsed(bool bUsed)
{
	bIsUsed = bUsed;
}

bool FKMTimingCancel::IsUsed() const
{
	return bIsUsed;
}

EKMTimingResult FKMTimingCancel::GetResult() const
{
	if (!OwnerGameObjectInstance.IsValid())
	{
		return EKMTimingResult::None;
	}
	
	float elapsedTime = OwnerGameObjectInstance->GetWorld()->GetTimeSeconds() - StartTimeSeconds;

	if (elapsedTime < OwnerGameObjectInstance->GetStatModifier()->GetEffectiveStat().GetPerfectCancel())
	{
		return EKMTimingResult::Perfect;
	}
	else if (elapsedTime < OwnerGameObjectInstance->GetStatModifier()->GetEffectiveStat().GetGreatCancel())
	{
		return EKMTimingResult::Great;
	}
	else if (elapsedTime < OwnerGameObjectInstance->GetStatModifier()->GetEffectiveStat().GetGoodCancel())
	{
		return EKMTimingResult::Good;
	}
	return EKMTimingResult::None;
}
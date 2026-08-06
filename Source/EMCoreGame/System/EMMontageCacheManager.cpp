#include "EMMontageCacheManager.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMMontageCacheManager
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FEMMontageCacheManager& FEMMontageCacheManager::Get()
{
	static FEMMontageCacheManager singletonInstance;
	return singletonInstance;
}
UAnimMontage* FEMMontageCacheManager::GetMontageBySlot(UAnimMontage* originMontage, FName slotName) const
{
	if (!IsValid(originMontage) || slotName == NAME_None || originMontage->SlotAnimTracks.IsEmpty() || originMontage->IsValidSlot(slotName))
	{
		return originMontage;
	}
	FMontageCacheKey montageCacheKey(slotName, originMontage);
	
	if (TObjectPtr<UAnimMontage>* existMontage = Montages.Find(montageCacheKey))
	{
		if (IsValid(*existMontage))
		{
			return *existMontage;
		}
		Montages.Remove(montageCacheKey);	
	}
	
	UAnimMontage* newMontage = Cast<UAnimMontage>(DuplicateObject(originMontage, GetTransientPackage()));
	if (!IsValid(newMontage))
	{
		return originMontage;
	}

	check(!newMontage->SlotAnimTracks.IsEmpty());

	//newMontage->Rename(*FString::Printf(TEXT("%s_slot_%s"), *originMontage->GetName(), *slotName.ToString()), GetTransientPackage());
	newMontage->SlotAnimTracks.SetNum(1);
	newMontage->SlotAnimTracks[0].SlotName = slotName;
	Montages.Emplace(montageCacheKey, newMontage);
	return newMontage;
}

void FEMMontageCacheManager::Clear()
{
	Montages.Empty();
}

void FEMMontageCacheManager::AddReferencedObjects(FReferenceCollector& collector)
{
	collector.AddReferencedObjects(Montages);
}
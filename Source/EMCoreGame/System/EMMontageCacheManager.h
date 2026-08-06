#pragma once

#include "CoreMinimal.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FMontageCacheKey
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FMontageCacheKey
{
	explicit FMontageCacheKey(FName slotName, class UObject* object ) : SlotName(slotName), Object(object) { };
	
	FName SlotName = NAME_None;
	TObjectPtr<UObject> Object = nullptr;

	bool operator==(const FMontageCacheKey& rhs) const
	{
		return SlotName == rhs.SlotName && Object == rhs.Object;
	}
	friend uint32 GetTypeHash(const FMontageCacheKey& key)
	{
		return HashCombine(GetTypeHash(key.SlotName), GetTypeHash(key.Object));
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FEMMontageCacheManager
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMCOREGAME_API FEMMontageCacheManager : public FGCObject
{
public:
	static FEMMontageCacheManager& Get();
	
	UAnimMontage* GetMontageBySlot(class UAnimMontage* originMontage, FName slotName = FAnimSlotGroup::DefaultSlotName) const;
	void Clear();

protected:
	virtual FString GetReferencerName() const override { return TEXT("FEMMontageCacheManager"); };
	virtual void AddReferencedObjects(FReferenceCollector& collector) override;

protected:
	mutable TMap<FMontageCacheKey, TObjectPtr<class UAnimMontage>> Montages;
};
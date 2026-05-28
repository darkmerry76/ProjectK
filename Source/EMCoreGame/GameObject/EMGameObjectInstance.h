#pragma once

#include "CoreMinimal.h"
#include "EMGameObjectInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class EMCOREGAME_API UEMGameObjectInstance : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	uint32 GetUniqueID() = delete;
	
	void SetId(int32 newId);
	int32 GetId() const;

	virtual bool IsClear() const { return false; }
	virtual void BeginPlay() { }
	virtual void EndPlay() { }
	
	virtual void Tick(float DeltaSeconds) { };

private:
	int32 Id;
};
#pragma once

#include "CoreMinimal.h"
#include "System/EMGameObjectSubsystem.h"
#include "KMNarrativeSubsystem.generated.h"

UCLASS(Blueprintable, BlueprintType, abstract)
class KMGAME_API UKMNarrativeSubsystem : public UEMGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMNarrativeSubsystem* GetNarrativeSubsystem(const UObject* worldContextObject);

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UKMVoicePDA> VoicePDA;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, TObjectPtr<class USoundWave>> VoiceWaves;

protected:
	UPROPERTY()
	TArray<TObjectPtr<class UKMNarrativeNode>> NodeInstances;
	
public:
	UFUNCTION(BlueprintCallable)
	bool StartDirector(const FName directorId);

	void BranchNode(class UKMNarrativeNode* fromNode, FName toNodeId);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBranch(class UKMNarrativeNode* fromNode, class UKMNarrativeNode* toNode);

	UFUNCTION(BlueprintCallable)
	void PlayVoice(FName speakerUniqueId, FName voiceName);

	UFUNCTION(BlueprintPure)
	class USoundWave* GetVoice(FName voiceName) const;

protected:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual void OnWorldCleanup(UWorld* cleaupWorld, bool bSessionEnded, bool bCleanupResources) override;

	template<typename _TL>
	_TL* CastNode(UKMNarrativeNode* node) const;

protected:
	virtual void Tick(float deltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;

protected:
	TMap<FName, TArray<const struct FKMTable_NarrativeRow*>> NarrativeTableGroups;
};
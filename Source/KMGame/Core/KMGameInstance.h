#pragma once

#include "CoreMinimal.h"
#include "KMDefine.h"
#include "Core/EMGameInstance.h"
#include "KMGameInstance.generated.h"

UCLASS()
class KMGAME_API UKMGameInstance : public UEMGameInstance
{
	GENERATED_BODY()

public:
	UKMGameInstance();

	UFUNCTION(BlueprintPure, meta=(WorldContext="WorldContextObject"))
	static UKMGameInstance* GetGameInstance(const UObject* worldContextObject);
	
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void SetFixedCamera(class AKMCameraActor* cameraActor);

	UFUNCTION(BlueprintPure)
	class UKMPlayerAccount* GetPlayerAccount() const;

	void OnViewportResized(FViewport* viewport, uint32 Unused);

	void OpenInitLevel(FSoftObjectPath mapPath);

	bool IsInitLevel() const { return bIsInitLevel; }

	UFUNCTION(BlueprintPure)
	const class UKMAbilityEffectSet* GetAnormalAbilitySet() const;

protected:
	UPROPERTY()
	TWeakObjectPtr<class AKMCameraActor> CameraActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGuid AuthPlayerGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsInitLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FKMSkillKeyBase> DefaultPassiveSkills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UKMAbilityEffectSet> AnormalAbilitySet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UKMPlayerAccount> PlayerAccount;
};

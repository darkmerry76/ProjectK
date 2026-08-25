#pragma once

#include "CoreMinimal.h"
#include "KMCharacter.h"
#include "InputActionValue.h"
#include "KMCharacterHero.generated.h"

UCLASS(Blueprintable, BlueprintType, Abstract)
class KMGAME_API AKMCharacterHero : public AKMCharacter
{
	GENERATED_BODY()
	
public:
	AKMCharacterHero();
	
public:
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* MovePressedAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* CombatSkillAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* TechniqueSkillAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* GuardAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* Debug1Action;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* Debug2Action;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* Debug3Action;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	bool ReadyJump = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	bool PressedJumpReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	float DoubleTapThreshold = 0.1f;

	float LastInputTime = 0.f;
	float LastMovementCircularDirection = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> NormalSkills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> AssistSkills;
	
protected:

	UFUNCTION(BlueprintNativeEvent)
	void OnDoubleTap(float direction);

	virtual  void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void StopJumping() override;
	
	void StartCustomJump();
	
	void Move(const FInputActionValue& Value);
	void MovePressed(const FInputActionValue& Value);
	void MoveReleased();

	void RunPressed();
	void RunReleased();

	void OnCombatSkillAction();
	void OnTachniqueSkillAction();

	void OnGuardSkillAction();
	void OnGuardSkillAction_Release();

	void OnDebug1Action();
	void OnDebug1Action_Release();

	void OnDebug2Action();
	void OnDebug2Action_Release();

	void OnDebug3Action();
	void OnDebug3Action_Release();
	
	UFUNCTION(BlueprintCallable)
	void PendingJump();

	bool bCombatTriggered = false;
	bool bTechniqueTriggered = false;

	FTimerHandle CombatSkillTimerHandle;
	FTimerHandle TechniqueSkillTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ComboInputWindow = 0.1f;

	void ExecuteCombatSkill();
	void ExecuteTechniqueSkill();
	void ExecuteUltimate();
	
public:
	friend class UKMHeroInstance;
	UFUNCTION(BlueprintNativeEvent)
	void OnEnterGame();

	friend class UKMHeroInstance;
	UFUNCTION(BlueprintNativeEvent)
	void OnHeroSelected();
};

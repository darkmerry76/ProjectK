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
	class UInputAction* SkillAction0;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* SkillAction1;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* SkillDebug0;

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

	void OnSkillAction0();
	void OnSkillAction1();
	
	void StartCustomJump();
	
	void Move(const FInputActionValue& Value);
	void MovePressed(const FInputActionValue& Value);
	void MoveReleased();

	void RunPressed();
	void RunReleased();

	void OnSkillDebug0();
	void OnSkillDebug0_Release();
	
	UFUNCTION(BlueprintCallable)
	void PendingJump();

	bool bSkill0Triggered = false;
	bool bSkill1Triggered = false;

	FTimerHandle Skill0TimerHandle;
	FTimerHandle Skill1TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ComboInputWindow = 0.1f;

	void ExecuteSkill0();
	void ExecuteSkill1();

	void ExecuteSkillCombo();
	
public:
	friend class UKMHeroInstance;
	UFUNCTION(BlueprintNativeEvent)
	void OnEnterGame();

	friend class UKMHeroInstance;
	UFUNCTION(BlueprintNativeEvent)
	void OnHeroSelected();
};

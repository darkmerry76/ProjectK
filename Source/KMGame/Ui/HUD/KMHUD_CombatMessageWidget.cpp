#include "KMHUD_CombatMessageWidget.h"
#include "GameObject/KMCharacterInstance.h"
#include "System/KMGameObjectSubsystem.h"
#include "Ui/HUD/KMHUD_CombatMessageItemWidget.h"
#include "UI/Component/EMScrollBox.h"

UKMHUD_CombatMessageWidget::UKMHUD_CombatMessageWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

void UKMHUD_CombatMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	if(IsValid(gameObjectSubsystem))
	{
		UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
		if(IsValid(authCharacterInstance))
		{
			authCharacterInstance->GetCombatMessageDelegate().AddUObject(this, &ThisClass::OnCommbatMessage);
		}
	}
	
	Refresh();
}

void UKMHUD_CombatMessageWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UKMGameObjectSubsystem* gameObjectSubsystem = UKMGameObjectSubsystem::GetGameObjectSubsystem(this);
	if(IsValid(gameObjectSubsystem))
	{
		UKMCharacterInstance* authCharacterInstance = gameObjectSubsystem->GetAuthCharacterInstance();
		if(IsValid(authCharacterInstance))
		{
			authCharacterInstance->GetCombatMessageDelegate().RemoveAll(this);
		}
	}
}

void UKMHUD_CombatMessageWidget::Refresh()
{
}

void UKMHUD_CombatMessageWidget::AddMessage(const FText& messageText, EKMCommbatMessageType messageType, const FLinearColor& fontColor, float fontSize, float duration)
{
	for (int32 childIndex = 0; childIndex < MessageScrollBox->GetChildrenCount(); ++childIndex)
	{
		UKMHUD_CombatMessageItemWidget* newMessageItemWidget = Cast<UKMHUD_CombatMessageItemWidget>(MessageScrollBox->GetChildAt(childIndex));
		if (!IsValid(newMessageItemWidget)) 
		{
			continue;			
		}
		
		if (newMessageItemWidget->GetMessageType() != messageType)
		{
			continue;
		}
		
		newMessageItemWidget->Refresh();

		return;
	};
	
	UKMHUD_CombatMessageItemWidget* newMessageItemWidget = Cast<UKMHUD_CombatMessageItemWidget>(CreateWidget<UKMHUD_CombatMessageItemWidget>(GetWorld(), MessageItemWidgetClass));
	check(newMessageItemWidget);

	newMessageItemWidget->SetDuration(duration);
	newMessageItemWidget->SetFontColor(fontColor);
	newMessageItemWidget->SetMessageSwitch(0);
	newMessageItemWidget->SetMessage(messageText);
	newMessageItemWidget->SetMessageType(messageType);
	newMessageItemWidget->SetFontSize(fontSize);

	MessageScrollBox->AddChild(newMessageItemWidget);
	
}

void UKMHUD_CombatMessageWidget::OnCommbatMessage(const UKMGameObjectInstance* gameObjectInstance, EKMCommbatMessageType messageType, const FString& newMessage)
{
	check(IsValid(gameObjectInstance));

	if (messageType == EKMCommbatMessageType::Critical)
	{
		AddMessage(FText::FromString(FString::Printf(TEXT("Critical"))), messageType, FLinearColor::Red, 40.f, 2.f);
	}
	else if (messageType == EKMCommbatMessageType::GreatCancel)
	{
		AddMessage(FText::FromString(FString::Printf(TEXT("Great Cancel"))), messageType, FLinearColor(0.5f, 0.5f, 1.f, 1.f),45.f,  2.f);
	}
	else if (messageType == EKMCommbatMessageType::PerfectCancel)
	{
		AddMessage(FText::FromString(FString::Printf(TEXT("Perfect Cancel"))), messageType, FLinearColor(0.5f, 0.5f, 1.f, 1.f),45.f,  2.f);
	}
	else if (messageType == EKMCommbatMessageType::GreatParry)
	{
		AddMessage(FText::FromString(FString::Printf(TEXT("Great Parry"))), messageType, FLinearColor(0.5f, 0.5f, 1.f, 1.f),45.f,  2.f);
	}
	else if (messageType == EKMCommbatMessageType::PerfectParry)
	{
		AddMessage(FText::FromString(FString::Printf(TEXT("Perfect Parry"))), messageType, FLinearColor(0.5f, 0.5f, 1.f, 1.f),60.f,  2.f);
	}
}

void UKMHUD_CombatMessageWidget::NativeTick(const FGeometry& myGeometry, float deltaTime)
{
	Super::NativeTick(myGeometry, deltaTime);

	if (IsValid(MessageScrollBox) == false)
	{
		return;
	}

	float currentOffset = MessageScrollBox->GetScrollOffset();
	float newOffset = currentOffset + ScrollSpeed * deltaTime;
	MessageScrollBox->SetScrollOffset(newOffset);

	const int32 childCount = MessageScrollBox->GetChildrenCount();
	for (int32 childIndex  = 0; childIndex  < childCount; ++childIndex )
	{
		UWidget* child = MessageScrollBox->GetChildAt(childIndex );
		if (IsValid(child))
		{
			continue;
		}

		const FGeometry& geo = child->GetCachedGeometry();
		const float bottomY = geo.GetAbsolutePosition().Y + geo.GetLocalSize().Y;
		if (bottomY < 0.f)
		{
			child->RemoveFromParent();
			--childIndex;
		}
	}
}
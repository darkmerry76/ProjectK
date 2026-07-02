#include "KMCharacterSelectItemWidget.h"
#include "System/EMTickerSubsystem.h"
#include "UI/Component/EMButton.h"

UKMCharacterSelectItemWidget::UKMCharacterSelectItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMCharacterSelectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(CharacterButton))
	{
		CharacterButton->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
		CharacterButton->OnHovered.AddDynamic(this, &ThisClass::OnHovered);
		CharacterButton->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhovered);
	}
}

void UKMCharacterSelectItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMCharacterSelectItemWidget::OnClicked()
{
	ClickedDelegate.Broadcast(this);
}

void UKMCharacterSelectItemWidget::OnHovered()
{
	HoverDelegate.Broadcast(this);
}

void UKMCharacterSelectItemWidget::OnUnhovered()
{
	UnhoverDelegate.Broadcast(this);
}

void UKMCharacterSelectItemWidget::HoveredAnimation()
{
	NextRenderScale = bIsSelected ? SelectedScale : HoveredScale;
}

void UKMCharacterSelectItemWidget::UnhoveredAnimation()
{
	NextRenderScale = bIsSelected ? SelectedScale : 1.f;
}

void UKMCharacterSelectItemWidget::NativeTick(const FGeometry& geometry, float deltaTime)
{
	Super::NativeTick(geometry, deltaTime);

	float currentScale = GetRenderTransform().Scale.X;
	
	float finalRenderScale = FMath::FInterpTo(currentScale, NextRenderScale, deltaTime, HoveredAnimatinTime);
	SetRenderScale(FVector2D(finalRenderScale));
}
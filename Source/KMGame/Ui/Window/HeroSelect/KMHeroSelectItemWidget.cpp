#include "KMHeroSelectItemWidget.h"
#include "UI/Component/EMButton.h"

UKMHeroSelectItemWidget::UKMHeroSelectItemWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
}

void UKMHeroSelectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HeroButton))
	{
		HeroButton->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
		HeroButton->OnHovered.AddDynamic(this, &ThisClass::OnHovered);
		HeroButton->OnUnhovered.AddDynamic(this, &ThisClass::OnUnhovered);
	}
}

void UKMHeroSelectItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKMHeroSelectItemWidget::OnClicked()
{
	ClickedDelegate.Broadcast(this);
}

void UKMHeroSelectItemWidget::OnHovered()
{
	HoverDelegate.Broadcast(this);
}

void UKMHeroSelectItemWidget::OnUnhovered()
{
	UnhoverDelegate.Broadcast(this);
}

void UKMHeroSelectItemWidget::HoveredAnimation()
{
	NextRenderScale = bIsSelected ? SelectedScale : HoveredScale;
}

void UKMHeroSelectItemWidget::UnhoveredAnimation()
{
	NextRenderScale = bIsSelected ? SelectedScale : 1.f;
}

void UKMHeroSelectItemWidget::NativeTick(const FGeometry& geometry, float deltaTime)
{
	Super::NativeTick(geometry, deltaTime);

	float currentScale = GetRenderTransform().Scale.X;
	
	float finalRenderScale = FMath::FInterpTo(currentScale, NextRenderScale, deltaTime, HoveredAnimatinTime);
	SetRenderScale(FVector2D(finalRenderScale));
}
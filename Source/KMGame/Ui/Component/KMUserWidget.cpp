#include "KMUserWidget.h"
#include "Animation/WidgetAnimation.h"

UKMUserWidget::UKMUserWidget(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	
}

UWidgetAnimation* UKMUserWidget::GetAnimationByName(FName animationName) const
{
	for(TFieldIterator<FObjectProperty> objectItr(GetClass()); objectItr; ++objectItr)
	{
		if(objectItr->PropertyClass->IsChildOf(UWidgetAnimation::StaticClass()))
		{
			UObject* selectedObject = objectItr->GetObjectPropertyValue_InContainer(this);
			UWidgetAnimation* widgetAnimation = Cast<UWidgetAnimation>(selectedObject);
			if(IsValid(widgetAnimation) && widgetAnimation->GetDisplayLabel() == animationName)
			{
				return widgetAnimation;
			}
		}
	}

	return nullptr;
}

bool UKMUserWidget::Initialize()
{
	bool bResult = Super::Initialize();

	return bResult;
}


FPrimaryAssetId UKMUserWidget::GetPrimaryAssetId() const
{
	const UClass* classPtr = GetClass();
	if (classPtr == nullptr)
	{
		return Super::GetPrimaryAssetId();
	}

	FString assetName = classPtr->GetName();
	assetName.RemoveFromEnd(TEXT("_C"));

	return FPrimaryAssetId("KMUserWidget", FName(*assetName));
}
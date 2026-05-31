#include "EMAnimationSetPropertyCustomization.h"
#include "DetailWidgetRow.h"
#include "EMOutlinerPublicTypes.h"
#include "IPropertyUtilities.h"
#include "Chooser/Tag/KMTagChooserOutlinerMode.h"
#include "Chooser/Tag/KMTagChooserOutlinerTreeItem.h"
#include "Chooser/Tag/SKMTagChooserOutliner.h"
#include "Notify/KMAnimNotifyState_Animation.h"

TSharedRef<IPropertyTypeCustomization> FKMChooserPropertyCustomization::MakeInstance()
{
	return MakeShared<FKMChooserPropertyCustomization>();
}

FKMChooserPropertyCustomization::~FKMChooserPropertyCustomization()
{
	if (ChooserWidget.IsValid())
	{
		if (ChooserWidget->GetDoubleClickEvent().IsBoundToObject(this))
		{
			ChooserWidget->GetDoubleClickEvent().RemoveAll(this);
		}
	}
}

void FKMChooserPropertyCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> propertyHandle, FDetailWidgetRow& headerRow, IPropertyTypeCustomizationUtils& customizationUtils)
{
	PropertyHandle = propertyHandle;
	PropertyUtilities = customizationUtils.GetPropertyUtilities();
	
	TSharedPtr<IPropertyHandle> tagHandle =	propertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FEMAnimationSetTag, Tag));

	FGameplayTag gameplayTag;
	void* tagValuePtr = nullptr;
	if (tagHandle->GetValueData(tagValuePtr) == FPropertyAccess::Success)
	{
		gameplayTag = *static_cast<FGameplayTag*>(tagValuePtr);
	}

	FText displayOpenButtonText = gameplayTag.IsValid() ? FText::FromString(gameplayTag.ToString()) : FText::FromString(TEXT("None"));

	FEMCreateOutlinerMode modeFactory = FEMCreateOutlinerMode::CreateLambda([](SEMOutliner* outliner)
	{
		return new FKMTagChooserOutlinerMode(outliner);
	});
	FEMOutlinerInitializationOptions initializationOptions;
	initializationOptions.ModeFactory = modeFactory;
	SAssignNew(ChooserWidget, SKMTagChooserOutliner, initializationOptions);

	ChooserWidget->GetDoubleClickEvent().AddRaw(this, &FKMChooserPropertyCustomization::OnTagSelected);

	SAssignNew(ChooserButton, SButton)
		.Text(displayOpenButtonText)
		.OnClicked(this, &FKMChooserPropertyCustomization::OnOpenChooser);
	
	
	headerRow
	.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
	.ValueContent()
		[
			ChooserButton.ToSharedRef()
		];
}

void FKMChooserPropertyCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder, IPropertyTypeCustomizationUtils& customizationUtils)
{
}

FReply FKMChooserPropertyCustomization::OnOpenChooser()
{
	TSharedRef<SWidget> menuContent =
		SNew(SBorder)
		.BorderBackgroundColor( FLinearColor(1.0f, 1.0f, 1.0f, 1.0f ) )
		.Padding(2.f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(6.f, 2.f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("ANIMATION SET TAGS")))
				.TextStyle(FAppStyle::Get(), "SmallText")
				.ColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.f))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				SNew(SBox)
				.WidthOverride(320.f)
				.HeightOverride(550.f)
				[
					ChooserWidget.ToSharedRef()
				]
			]
		];

	const FGeometry& geometry = ChooserButton->GetCachedGeometry();
	const FVector2D summonLocation = geometry.LocalToAbsolute(FVector2D(0.f, geometry.GetLocalSize().Y));
	FSlateApplication::Get().PushMenu(ChooserButton.ToSharedRef(),
		FWidgetPath(),
		menuContent,
		geometry.LocalToAbsolute(FVector2D(0.f, geometry.GetLocalSize().Y)),
		FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

	return FReply::Handled();
}

void FKMChooserPropertyCustomization::OnTagSelected(TSharedPtr<IEMOutlinerTreeItem> treeItem)
{
	if (!PropertyHandle.IsValid() || !treeItem.IsValid())
	{
		return;
	}

	FGameplayTag resultGameplayTag;
	if (treeItem->IsA<FKMTagChooserOutlinerTreeItem>())
	{
		resultGameplayTag = StaticCastSharedRef<FKMTagChooserOutlinerTreeItem>(treeItem.ToSharedRef())->GetTag();
	}
	else if (treeItem->IsA<FKMTagChooserOutlinerGroupTreeItem>())
	{
		resultGameplayTag = StaticCastSharedRef<FKMTagChooserOutlinerGroupTreeItem>(treeItem.ToSharedRef())->GetTag();
	}

	TSharedPtr<IPropertyHandle> tagHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FEMAnimationSetTag, Tag));
	if (tagHandle.IsValid() && tagHandle->SetValueFromFormattedString(resultGameplayTag.ToString()) == FPropertyAccess::Success)
	{
		if (PropertyUtilities.IsValid())
		{
			PropertyUtilities->ForceRefresh();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

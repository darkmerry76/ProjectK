#include "EMAnimationSetPropertyCustomization.h"
#include "DetailWidgetRow.h"
#include "EMMartialArts.h"
#include "EMOutlinerPublicTypes.h"
#include "IPropertyUtilities.h"
#include "Animation/AnimSet/KMAnimationSetTag.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Character/KMCharacter.h"
#include "Chooser/Tag/KMTagChooserOutlinerMode.h"
#include "Chooser/Tag/KMTagChooserOutlinerTreeItem.h"
#include "Chooser/Tag/SKMTagChooserOutliner.h"
#include "MartialArts/KMMartialArtsEditor.h"
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

void FKMChooserPropertyCustomization::OnChooserAddItem(TSharedPtr<IEMOutlinerTreeItem> item)
{
	
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	UKMCharacterInstance* ownerCharacterInstance = OwnerCharacter->GetCharacterInstance();
	if (!IsValid(ownerCharacterInstance))
	{
		return;
	}

	if (!item->IsA<FKMTagChooserOutlinerTreeItem>())
	{
		return;
	}

	TSharedPtr<FKMTagChooserOutlinerTreeItem> treeItem = StaticCastSharedPtr<FKMTagChooserOutlinerTreeItem>(item);
	UAnimMontage* animMontage = OwnerCharacter->GetAnimationTag(treeItem->GetTag());
	if (!IsValid(animMontage))
	{
		return;
	}
	FAssetData assetData(animMontage);
	treeItem->SetAssetData(assetData);
}

void FKMChooserPropertyCustomization::CreateChooser()
{
	TArray<UObject*> outerObjects;
	PropertyHandle->GetOuterObjects(outerObjects);

	UAssetEditorSubsystem* assetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	TArray<UObject*> editedAssets = assetEditorSubsystem->GetAllEditedAssets();
	for (UObject* object : outerObjects)
	{
		UAnimNotifyState* notifyState = Cast<UAnimNotifyState>(object);
		if (IsValid(notifyState))
		{
			IAssetEditorInstance* assetEditor =
				assetEditorSubsystem->FindEditorForAsset(notifyState->GetOuter(), false);

			if (assetEditor->GetEditorName() == TEXT("KMMartialArtsEditor"))
			{
				if (FKMMartialArtsEditor* martialArtsEditor = static_cast<FKMMartialArtsEditor*>(assetEditor))
				{
					OwnerCharacter = martialArtsEditor->GetOwnerCharacter();
					OwnerCharacterInstance = martialArtsEditor->GetOwnerCharacterInstance();
					break;
				}
			}
		}
	}
	
	ChooserWidget = MakeShared<SKMTagChooserOutliner>();
	ChooserWidget->AddItemDelegate.BindRaw(this, &FKMChooserPropertyCustomization::OnChooserAddItem);
	
	FEMCreateOutlinerMode modeFactory = FEMCreateOutlinerMode::CreateLambda([](SEMOutliner* outliner)
	{
		return new FKMTagChooserOutlinerMode(outliner);
	});
	FEMOutlinerInitializationOptions initializationOptions;
	initializationOptions.ModeFactory = modeFactory;

	SEMOutliner::FArguments args;
	ChooserWidget->Construct(args, initializationOptions);
	ChooserWidget->GetDoubleClickEvent().AddRaw(this, &FKMChooserPropertyCustomization::OnTagSelected);
}

void FKMChooserPropertyCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> propertyHandle, FDetailWidgetRow& headerRow, IPropertyTypeCustomizationUtils& customizationUtils)
{
	PropertyHandle = propertyHandle;
	PropertyUtilities = customizationUtils.GetPropertyUtilities();

	TSharedPtr<IPropertyHandle> tagHandle =	PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FEMAnimationSetTag, Tag));
	FGameplayTag gameplayTag;
	void* tagValuePtr = nullptr;
	if (tagHandle->GetValueData(tagValuePtr) == FPropertyAccess::Success)
	{
		gameplayTag = *static_cast<FGameplayTag*>(tagValuePtr);
	}

	FText displayOpenButtonText = gameplayTag.IsValid() ? FText::FromString(gameplayTag.ToString()) : FText::FromString(TEXT("None"));
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
	CreateChooser();
	
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
	UAnimMontage* animMontage = nullptr;
	if (treeItem->IsA<FKMTagChooserOutlinerTreeItem>())
	{
		resultGameplayTag = StaticCastSharedRef<FKMTagChooserOutlinerTreeItem>(treeItem.ToSharedRef())->GetTag();
		if (IsValid(OwnerCharacter))
		{
			animMontage = OwnerCharacter->GetAnimationTag(resultGameplayTag);
		}
	}
	else if (treeItem->IsA<FKMTagChooserOutlinerGroupTreeItem>())
	{
		resultGameplayTag = StaticCastSharedRef<FKMTagChooserOutlinerGroupTreeItem>(treeItem.ToSharedRef())->GetTag();
	}

	TSharedPtr<IPropertyHandle> montageHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FEMAnimationSetTag, Montage));
	if (montageHandle.IsValid())
	{
		if (animMontage)
		{
			montageHandle->SetValue(animMontage);
		}
	}

	TSharedPtr<IPropertyHandle> tagHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FEMAnimationSetTag, Tag));
	if (tagHandle.IsValid())
	{
		tagHandle->SetValueFromFormattedString(resultGameplayTag.ToString());
	}
	
	if (PropertyUtilities.IsValid())
	{
		PropertyUtilities->ForceRefresh();
	}
	FSlateApplication::Get().DismissAllMenus();
}

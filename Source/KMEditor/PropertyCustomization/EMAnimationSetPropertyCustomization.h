#pragma once

#include "CoreMinimal.h"

class FKMChooserPropertyCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual ~FKMChooserPropertyCustomization() override;
	
protected:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> propertyHandle, FDetailWidgetRow& headerRow, IPropertyTypeCustomizationUtils& customizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder, IPropertyTypeCustomizationUtils& customizationUtils) override;

	FReply OnOpenChooser();
	void OnTagSelected(TSharedPtr<struct IEMOutlinerTreeItem> treeItem);

	void CreateChooser();
	void OnChooserAddItem(TSharedPtr<IEMOutlinerTreeItem> item);

protected:
	TSharedPtr<class SButton> ChooserButton;
	TSharedPtr<class SKMTagChooserOutliner> ChooserWidget;
	TSharedPtr<class IPropertyHandle> PropertyHandle;
	TSharedPtr<IPropertyUtilities> PropertyUtilities;

	class AKMCharacter* OwnerCharacter = nullptr;
	class UKMCharacterInstance* OwnerCharacterInstance = nullptr;
};
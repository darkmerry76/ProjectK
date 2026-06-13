#include "KMEditor.h"
#include "EMMartialArtsEditorModule.h"
#include "UnrealEdGlobals.h"
#include "Camera/KMSpringArmComponent.h"
#include "Modules/ModuleManager.h"
#include "ComponentVisualizer/KMSpringArmComponentVisualizer.h"
#include "Editor/UnrealEdEngine.h"
#include "MartialArts/KMMartialArtsEditor.h"
#include "PropertyCustomization/EMAnimationSetPropertyCustomization.h"

void FKMEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		if (GUnrealEd)
		{
			TSharedPtr<FComponentVisualizer> visualizer = MakeShared<FKMSpringArmComponentVisualizer>();
			GUnrealEd->RegisterComponentVisualizer(UKMSpringArmComponent::StaticClass()->GetFName(), visualizer);
			visualizer->OnRegister();
		}

		FEMMartialArtsEditorModule* martialArtsEditorModule = FModuleManager::GetModulePtr<FEMMartialArtsEditorModule>("EMMartialArtsEditor");
		if (martialArtsEditorModule)
		{
			martialArtsEditorModule->GetCreateMartialArtsEditorDelegate().BindLambda([]()
			{
				return MakeShared<FKMMartialArtsEditor>();
			});
		}
		
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout("EMAnimationSetTag",
	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FKMChooserPropertyCustomization::MakeInstance));

PropertyModule.NotifyCustomizationModuleChanged();
	});

}

void FKMEditorModule::ShutdownModule()
{
	if(GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UKMSpringArmComponent::StaticClass()->GetFName());
	}
}

IMPLEMENT_PRIMARY_GAME_MODULE(FKMEditorModule, KMEditor, "KMEditor" );

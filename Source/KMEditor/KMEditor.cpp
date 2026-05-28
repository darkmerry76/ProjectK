#include "KMEditor.h"
#include "EMMartialArtsEditorModule.h"
#include "UnrealEdGlobals.h"
#include "Camera/KMSpringArmComponent.h"
#include "Modules/ModuleManager.h"
#include "ComponentVisualizer/KMSpringArmComponentVisualizer.h"
#include "Editor/UnrealEdEngine.h"
#include "MartialArts/KMMartialArtsEditor.h"

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

		FEMMartialArtsEditorModule& martialArtsEditorModule = FModuleManager::GetModuleChecked<FEMMartialArtsEditorModule>("EMMartialArtsEditor");
		martialArtsEditorModule.GetCreateMartialArtsEditorDelegate().BindLambda([]()
		{
			return MakeShared<FKMMartialArtsEditor>();
		});
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

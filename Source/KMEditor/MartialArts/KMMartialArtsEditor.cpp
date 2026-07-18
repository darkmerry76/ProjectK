#include "KMMartialArtsEditor.h"
#include "EMAnimationEditorPreviewActor.h"
#include "EMAnimationEditorViewportClient.h"
#include "EMMartialArts.h"
#include "EMMartialArtsComponent.h"
#include "EMMartialArtsEditorModule.h"
#include "IPersonaPreviewScene.h"
#include "ISkeletonTree.h"
#include "KMMartialArtsEditorMode.h"
#include "Character/KMCharacter.h"
#include "CharacterOutliner/KMCharacterOutlinerHierarchy.h"
#include "Notify/KMAnimNotifyState_Animation.h"
#include "Util/KMUtil.h"

#define LOCTEXT_NAMESPACE "KMMartialArtsEditor"

FKMMartialArtsEditor::~FKMMartialArtsEditor()
{
	if (FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.IsBoundToObject(this))
	{
		FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.RemoveAll(this);
	}
	if (FKMCharacterOutlinerHierarchy::BeastSelectedDelegate.IsBoundToObject(this))
	{
		FKMCharacterOutlinerHierarchy::BeastSelectedDelegate.RemoveAll(this);
	}
	if (!FCoreUObjectDelegates::OnObjectPropertyChanged.IsBoundToObject(this))
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
	}
}

void FKMMartialArtsEditor::InitEditor(const EToolkitMode::Type mode, const TSharedPtr<class IToolkitHost>& initToolkitHost, class UAnimationAsset* animationAsset)
{
	FEMMartialArtsEditor::InitEditor(mode, initToolkitHost, animationAsset);

	if (!FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.IsBoundToObject(this))
	{
		FKMCharacterOutlinerHierarchy::CharacterSelectedDelegate.AddSP(this, &FKMMartialArtsEditor::OnOwnerCharacterSelected);
	}
	if (!FKMCharacterOutlinerHierarchy::BeastSelectedDelegate.IsBoundToObject(this))
	{
		FKMCharacterOutlinerHierarchy::BeastSelectedDelegate.AddSP(this, &FKMMartialArtsEditor::OnOwnerBeastSelected);
	}
	if (!FCoreUObjectDelegates::OnObjectPropertyChanged.IsBoundToObject(this))
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.AddSP(this, &FKMMartialArtsEditor::OnObjectPropertyChanged);
	}
}

void FKMMartialArtsEditor::OnObjectPropertyChanged(UObject* object, FPropertyChangedEvent& propertyChangedEvent)
{
	if (IEMAnimNotifyInterfaceMA* animNotifyInteface = Cast<IEMAnimNotifyInterfaceMA>(object))
	{
		animNotifyInteface->PostEditChangeProperty(GetOwnerCharacter(), propertyChangedEvent);
	}
}

TSharedPtr<FEMMartialArtsEditorMode> FKMMartialArtsEditor::CreateEditorMode()
{
	return MakeShared<FKMMartialArtsEditorMode>(SharedThis(this), SkeletonTree.ToSharedRef());
}

void FKMMartialArtsEditor::AddReferencedObjects(FReferenceCollector& collector)
{
	FEMMartialArtsEditor::AddReferencedObjects(collector);

	collector.AddReferencedObject(OwnerCharacterInstance);
	collector.AddReferencedObjects(TargetCharacterInstances);
}

UKMCharacterInstance* FKMMartialArtsEditor::GetOwnerCharacterInstance() const
{
	return OwnerCharacterInstance;
}

AKMCharacter* FKMMartialArtsEditor::GetOwnerCharacter() const
{
	if (!IsValid(OwnerCharacterInstance))
	{
		return nullptr;
	}
	return OwnerCharacterInstance->GetCharacter();
}

UKMCharacterInstance* FKMMartialArtsEditor::SpawnCharacterInstance(const FKMTable_CharacterRow* characterTable, const FTransform& spawnedTransform)
{
	UWorld* world = GetWorld();
	if (!IsValid(world))
	{
		UE_LOG(LogEMMartialArtsEditor, Warning, TEXT("FKMMartialArtsEditor::SpawnCharacter world == nullptr"));
		return nullptr;
	}

	UKMCharacterInstance* spwnCharacterInstance = UKMUtil::SpawnCharacterObjectByTable(world, characterTable, spawnedTransform);
	if (!IsValid(spwnCharacterInstance))
	{
		UE_LOG(LogEMMartialArtsEditor, Warning, TEXT("FKMMartialArtsEditor::SpawnCharacter spwnCharacterInstance == nullptr"));
		return nullptr;
	}

	FActorSpawnParameters spawnParameters;
	APlayerController* newPlayerController = world->SpawnActor<APlayerController>(APlayerController::StaticClass(), spawnedTransform, spawnParameters);
	if (IsValid(newPlayerController))
	{
		newPlayerController->Possess(spwnCharacterInstance->GetCharacter());
	}
	
	return spwnCharacterInstance;
}

bool FKMMartialArtsEditor::DestroyCharacterInstance(UKMCharacterInstance* characterInstance)
{
	UWorld* world = GetWorld();
	if (!IsValid(world))
	{
		UE_LOG(LogEMMartialArtsEditor, Warning, TEXT("FKMMartialArtsEditor::DestroyCharacter world == nullptr"));
		return false;
	}

	if (OwnerCharacterInstance == characterInstance)
	{
		if (ACharacter* character = OwnerCharacterInstance->GetCharacter())
		{
			if (AController* controller = character->GetController())
			{
				controller->Destroy();
			}
		}
		OwnerCharacterInstance->EndPlay();
		OwnerCharacterInstance = nullptr;
	}
	
	int32 targetCharacterIndex = TargetCharacterInstances.Find(characterInstance);
	if (targetCharacterIndex != INDEX_NONE)
	{
		if (!IsValid(TargetCharacterInstances[targetCharacterIndex]))
		{
			TargetCharacterInstances[targetCharacterIndex]->EndPlay();
		}
		TargetCharacterInstances.RemoveAt(targetCharacterIndex);
	}

	return true;
}

void FKMMartialArtsEditor::OnOwnerCharacterSelected(const FKMTable_CharacterRow* newCharacterTable)
{
	SpawnOwnerCharacterInstance(newCharacterTable);	
}

void FKMMartialArtsEditor::OnOwnerBeastSelected(const FKMTable_BeastRow* newBeastTable)
{
}

FSphere FKMMartialArtsEditor::GetCameraTargetSphere() const
{
	if (!IsValid(OwnerCharacterInstance))
	{
		return FEMMartialArtsEditor::GetCameraTargetSphere();
	}
	AKMCharacter* character = Cast<AKMCharacter>(OwnerCharacterInstance->GetCharacter());
	if (!IsValid(character))
	{
		return FEMMartialArtsEditor::GetCameraTargetSphere();
	}

	return character->GetMesh()->CalcBounds(FTransform::Identity).GetSphere();
}

void FKMMartialArtsEditor::SpawnOwnerCharacterInstance(const FKMTable_CharacterRow* characterTable)
{
	AEMAnimationEditorPreviewActor* previewActor = Cast<AEMAnimationEditorPreviewActor>(GetPreviewScene()->GetActor());
	if (IsValid(OwnerCharacterInstance))
	{
		if (DestroyCharacterInstance(OwnerCharacterInstance))
		{
			if (IsValid(previewActor))
			{
				previewActor->SetAdjustCharacter(nullptr);
			}
			OwnerCharacterInstance = nullptr;
		}
	}
	
	UKMCharacterInstance* spawnCharacterInstance = SpawnCharacterInstance(characterTable);
	if (!IsValid(spawnCharacterInstance))
	{
		return;
	}
	OwnerCharacterInstance = spawnCharacterInstance;

	if (IsValid(previewActor))
	{
		previewActor->SetAdjustCharacter(OwnerCharacterInstance->GetCharacter());
	}

	if (AKMCharacter* character = Cast<AKMCharacter>(spawnCharacterInstance->GetCharacter()))
	{
		UEMMartialArtsComponent* martialArtsComponent = character->GetMartialArtsComponent();
		check(IsValid(martialArtsComponent));

		if (UEMMartialArts* martialArts = Cast<UEMMartialArts>(AnimationAsset))
		{
			martialArtsComponent->PlayEx(martialArts, nullptr, 1.f, true);
		}
		martialArtsComponent->SetPause(true);
		SkeletonTree->SetSkeletalMesh(character->GetMesh()->GetSkeletalMeshAsset());
		static_cast<FEMAnimationViewportClient*>(ViewportClient.Get())->SetAdjustSkeletalMeshComponent(character->GetMesh());
	}
	FocusCamera();
}

void FKMMartialArtsEditor::AddTargetCharacterInstance(const struct FKMTable_CharacterRow* newCharacterTable)
{
	
}

#undef LOCTEXT_NAMESPACE

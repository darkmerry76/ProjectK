#pragma once

#include "CoreMinimal.h"
#include "EMMartialArtsEditor.h"

class FKMMartialArtsEditor : public FEMMartialArtsEditor
{
public:
	virtual ~FKMMartialArtsEditor() override;

	class UKMCharacterInstance* GetOwnerCharacterInstance() const;
	class AKMCharacter* GetOwnerCharacter() const;

protected:
	virtual void InitEditor(const EToolkitMode::Type mode, const TSharedPtr<class IToolkitHost>& initToolkitHost, class UAnimationAsset* animationAsset) override;
	virtual TSharedPtr<class FEMMartialArtsEditorMode> CreateEditorMode() override;

	virtual void AddReferencedObjects(FReferenceCollector& collector) override;
	virtual FSphere GetCameraTargetSphere() const override;

	void OnOwnerCharacterSelected(const struct FKMTable_CharacterRow* newCharacterTable);
	void OnOwnerBeastSelected(const struct FKMTable_BeastRow* newBeastTable);
	
	void AddTargetCharacterInstance(const struct FKMTable_CharacterRow* newCharacterTable);

	void SpawnOwnerCharacterInstance(const struct FEM_TableBaseRow* characterTableBase);

	virtual FName GetEditorName() const override
	{
		return TEXT("KMMartialArtsEditor");
	}

protected:
	class UKMCharacterInstance* SpawnCharacterInstance(const struct FKMTable_CharacterRow* characterTable, const FTransform& spawnedTransform = FTransform::Identity);
	
	class UKMCharacterInstance* SpawnCharacterInstance(const struct FKMTable_BeastRow* beastTable, const FTransform& spawnedTransform = FTransform::Identity);
	
	bool DestroyCharacterInstance(class UKMCharacterInstance* characterInstance);

	void OnObjectPropertyChanged(UObject* object, FPropertyChangedEvent& propertyChangedEvent);

protected:
	TObjectPtr<class UKMCharacterInstance> OwnerCharacterInstance;
	TArray<TObjectPtr<class UKMCharacterInstance>> TargetCharacterInstances;
};

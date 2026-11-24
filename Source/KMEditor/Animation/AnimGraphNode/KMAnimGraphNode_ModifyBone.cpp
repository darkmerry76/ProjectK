#include "KMAnimGraphNode_ModifyBone.h"
#include "UnrealWidgetFwd.h"
#include "AnimNodeEditModes.h"
#include "Kismet2/CompilerResultsLog.h"

/////////////////////////////////////////////////////
// UAnimGraphNode_ModifyBone

#include UE_INLINE_GENERATED_CPP_BY_NAME(KMAnimGraphNode_ModifyBone)

#define LOCTEXT_NAMESPACE "A3Nodes"

UKMAnimGraphNode_ModifyBone::UKMAnimGraphNode_ModifyBone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKMAnimGraphNode_ModifyBone::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	if (ForSkeleton && !ForSkeleton->HasAnyFlags(RF_NeedPostLoad))
	{
		if (Node.DynamicBoneReference.IsValidToEvaluate())
		{
			if (ForSkeleton->GetReferenceSkeleton().FindBoneIndex(Node.DynamicBoneReference.BoneName) == INDEX_NONE)
			{
				if (Node.DynamicBoneReference.BoneName == NAME_None)
				{
					MessageLog.Warning(*LOCTEXT("NoBoneSelectedToModify", "@@ - You must pick a bone to modify").ToString(), this);
				}
				else
				{
					FFormatNamedArguments Args;
					Args.Add(TEXT("BoneName"), FText::FromName(Node.DynamicBoneReference.BoneName));

					FText Msg = FText::Format(LOCTEXT("NoBoneFoundToModify", "@@ - Bone {BoneName} not found in Skeleton"), Args);

					MessageLog.Warning(*Msg.ToString(), this);
				}
			}
		}
	}

	if ((Node.TranslationMode == BMM_Ignore) && (Node.RotationMode == BMM_Ignore) && (Node.ScaleMode == BMM_Ignore))
	{
		MessageLog.Warning(*LOCTEXT("NothingToModify", "@@ - No components to modify selected.  Either Rotation, Translation, or Scale should be set to something other than Ignore").ToString(), this);
	}

	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

FText UKMAnimGraphNode_ModifyBone::GetControllerDescription() const
{
	return LOCTEXT("KMTransformModifyBone", "KMTransform (Modify) Bone");
}

FText UKMAnimGraphNode_ModifyBone::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_ModifyBone_Tooltip", "The Transform Bone node alters the transform - i.e. Translation, Rotation, or Scale - of the bone");
}

FText UKMAnimGraphNode_ModifyBone::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if ((TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle) && (Node.DynamicBoneReference.BoneName == NAME_None))
	{
		return GetControllerDescription();
	}
	else
	{
/*		FFormatNamedArguments Args;
		Args.Add(TEXT("ControllerDescription"), GetControllerDescription());
		Args.Add(TEXT("BoneName"), FText::FromName(Node.DynamicBoneReference.BoneName));

		if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ModifyBone_ListTitle", "{ControllerDescription} - Bone: {BoneName}"), Args), this);
		}
		else
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_ModifyBone_Title", "{ControllerDescription}\nBone: {BoneName}"), Args), this);
		}*/
	}
	return CachedNodeTitles[TitleType];
}

void UKMAnimGraphNode_ModifyBone::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{
	FKMAnimNode_ModifyBone* ModifyBone = static_cast<FKMAnimNode_ModifyBone*>(InPreviewNode);

	ModifyBone->Translation = Node.Translation;
	ModifyBone->Rotation = Node.Rotation;
	ModifyBone->Scale = Node.Scale;

	ModifyBone->TranslationMode = Node.TranslationMode;
	ModifyBone->RotationMode = Node.RotationMode;
	ModifyBone->ScaleMode = Node.ScaleMode;

	ModifyBone->TranslationSpace = Node.TranslationSpace;
	ModifyBone->RotationSpace = Node.RotationSpace;
	ModifyBone->ScaleSpace = Node.ScaleSpace;
}

/*FEditorModeID UKMAnimGraphNode_ModifyBone::GetEditorMode() const
{
	return AnimNodeEditModes::ModifyBone;
}*/

void UKMAnimGraphNode_ModifyBone::CopyPinDefaultsToNodeData(UEdGraphPin* InPin)
{
	if (InPin->GetName() == GET_MEMBER_NAME_STRING_CHECKED(FKMAnimNode_ModifyBone, Translation))
	{
		GetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FKMAnimNode_ModifyBone, Translation), Node.Translation);
	}
	else if (InPin->GetName() == GET_MEMBER_NAME_STRING_CHECKED(FKMAnimNode_ModifyBone, Rotation))
	{
		GetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FKMAnimNode_ModifyBone, Rotation), Node.Rotation);
	}
	else if (InPin->GetName() == GET_MEMBER_NAME_STRING_CHECKED(FKMAnimNode_ModifyBone, Scale))
	{
		GetDefaultValue(GET_MEMBER_NAME_STRING_CHECKED(FKMAnimNode_ModifyBone, Scale), Node.Scale);
	}
}

#undef LOCTEXT_NAMESPACE

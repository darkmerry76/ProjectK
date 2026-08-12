#include "KMAnimNode_MultiSlot.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimStats.h"
#include "Animation/AnimTrace.h"
#include "Animation/AnimNode_Inertialization.h"
#include "Animation/KMAnimInstance.h"

FKMAnimNode_MultiSlot::FKMAnimNode_MultiSlot() : bAlwaysUpdateSourcePose(false)
{
	SlotName.Emplace(FAnimSlotGroup::DefaultSlotName);
}

void FKMAnimNode_MultiSlot::Initialize_AnyThread(const FAnimationInitializeContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(context);

	Source.Initialize(context);
	DefaultWeightData.Reset();
	TargetWeightData.Reset();

	if (!SlotNodeInitializationCounter.IsSynchronized_Counter(context.AnimInstanceProxy->GetSlotNodeInitializationCounter()))
	{
		SlotNodeInitializationCounter.SynchronizeWith(context.AnimInstanceProxy->GetSlotNodeInitializationCounter());

		for (auto slotName : SlotName)
		{
			context.AnimInstanceProxy->RegisterSlotNodeWithAnimInstance(slotName);
		}
	}
}

void FKMAnimNode_MultiSlot::CacheBones_AnyThread(const FAnimationCacheBonesContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
	Source.CacheBones(context);
}

void FKMAnimNode_MultiSlot::UpdateSlot(const FAnimationUpdateContext& context, const FName& slotName, FSlotNodeWeightInfo& weightInfo)
{
	if (slotName == NAME_None)
	{
		return;
	}
	context.AnimInstanceProxy->GetSlotWeight(slotName, weightInfo.SlotNodeWeight, weightInfo.SourceWeight, weightInfo.TotalNodeWeight);
	context.AnimInstanceProxy->UpdateSlotNodeWeight(slotName, weightInfo.SlotNodeWeight, context.GetFinalBlendWeight());

	FInertializationRequest inertializationRequest;
	if (context.AnimInstanceProxy->GetSlotInertializationRequestData(slotName, inertializationRequest))
	{
		UE::Anim::IInertializationRequester* inertializationRequester = context.GetMessage<UE::Anim::IInertializationRequester>();
		if (inertializationRequester)
		{
#if ANIM_TRACE_ENABLED
			inertializationRequest.NodeId = context.GetCurrentNodeId();
			inertializationRequest.AnimInstance = context.AnimInstanceProxy->GetAnimInstanceObject();
#endif

			inertializationRequester->RequestInertialization(inertializationRequest);
		}
		else
		{
			FAnimNode_Inertialization::LogRequestError(context, Source);
		}
	}

#if ANIM_TRACE_ENABLED
	TRACE_ANIM_NODE_VALUE(context, TEXT("Name"), slotName);
	TRACE_ANIM_NODE_VALUE(context, TEXT("Slot Weight"), weightInfo.SlotNodeWeight);
	TRACE_ANIM_NODE_VALUE(context, TEXT("Pose Source"), (weightInfo.SourceWeight <= ZERO_ANIMWEIGHT_THRESH));

	context.AnimInstanceProxy->TraceMontageEvaluationData(context, slotName);
#endif
}

void FKMAnimNode_MultiSlot::Update_AnyThread(const FAnimationUpdateContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)

	const FKMAnimInstanceProxy& animInstanceProxy = static_cast<const FKMAnimInstanceProxy&>(*context.AnimInstanceProxy);
	const FKMMultiSlotBlendInfo& slotBlendInfo = animInstanceProxy.GetSlotBlendInfo();

	if (slotBlendInfo.BlendWeight > ZERO_ANIMWEIGHT_THRESH && slotBlendInfo.BlendWeight < (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		UpdateSlot(context, SlotName[0],  DefaultWeightData);
		UpdateSlot(context, slotBlendInfo.TargetSlot, TargetWeightData);
	}
	else if (slotBlendInfo.BlendWeight >= (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		UpdateSlot(context, slotBlendInfo.TargetSlot, TargetWeightData);
	}
	else if (slotBlendInfo.BlendWeight <= ZERO_ANIMWEIGHT_THRESH)
	{
		UpdateSlot(context, SlotName[0], DefaultWeightData);
	}

	float finalSourceWeight = FMath::Max(1.f - slotBlendInfo.BlendWeight,slotBlendInfo.BlendWeight);
	if (bAlwaysUpdateSourcePose || FAnimWeight::IsRelevant(finalSourceWeight))
	{
		Source.Update(context);
	}
}

void FKMAnimNode_MultiSlot::Evaluate_AnyThread(FPoseContext& output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(Slot, !IsInGameThread());

	if (DefaultWeightData.SlotNodeWeight + TargetWeightData.SlotNodeWeight <= ZERO_ANIMWEIGHT_THRESH)
	{
		Source.Evaluate(output);
		PostEvaluateSourcePose(output);
	}
	else
	{
		const FKMAnimInstanceProxy& animInstanceProxy = static_cast<const FKMAnimInstanceProxy&>(*output.AnimInstanceProxy);
		const FKMMultiSlotBlendInfo& slotBlendInfo = animInstanceProxy.GetSlotBlendInfo();

		bool bEvaluate = FMath::Max(1.f - slotBlendInfo.BlendWeight,slotBlendInfo.BlendWeight) > ZERO_ANIMWEIGHT_THRESH;
		if (slotBlendInfo.BlendWeight > ZERO_ANIMWEIGHT_THRESH && slotBlendInfo.BlendWeight < (1.f - ZERO_ANIMWEIGHT_THRESH))
		{
			FPoseContext sourceContext= output;
			if (bEvaluate)
			{
				Source.Evaluate(sourceContext);
				PostEvaluateSourcePose(sourceContext);
			}
			FPoseContext defaultContext( sourceContext);
			FAnimationPoseData outputDefaultPoseData(defaultContext);

			const FAnimationPoseData defaultPoseData(sourceContext);
			output.AnimInstanceProxy->SlotEvaluatePose(SlotName[0], defaultPoseData,
			DefaultWeightData.SourceWeight, outputDefaultPoseData, DefaultWeightData.SlotNodeWeight, DefaultWeightData.TotalNodeWeight);

			FPoseContext targetContext = sourceContext;
			FAnimationPoseData outputTargetPoseData(targetContext);

			const FAnimationPoseData targetPoseData(sourceContext);
			targetContext.AnimInstanceProxy->SlotEvaluatePose(slotBlendInfo.TargetSlot, targetPoseData,
			TargetWeightData.SourceWeight, outputTargetPoseData, TargetWeightData.SlotNodeWeight, TargetWeightData.TotalNodeWeight);

			TArray<const FCompactPose*> poses;
			TArray<const FBlendedCurve*> curves;
			TArray<const UE::Anim::FStackAttributeContainer*> attributes;
			TArray<float> weights;

			poses.Add(&outputDefaultPoseData.GetPose());
			poses.Add(&outputTargetPoseData.GetPose());

			curves.Add(&outputDefaultPoseData.GetCurve());
			curves.Add(&outputTargetPoseData.GetCurve());

			attributes.Add(&outputDefaultPoseData.GetAttributes());
			attributes.Add(&outputTargetPoseData.GetAttributes());

			weights.Add(1.f - slotBlendInfo.BlendWeight);
			weights.Add(slotBlendInfo.BlendWeight);

			FAnimationPoseData outputPoseData(output);
			FAnimationRuntime::BlendPosesTogetherIndirect(poses,curves,attributes,weights, outputPoseData);
		}
		else if (slotBlendInfo.BlendWeight >= (1.f - ZERO_ANIMWEIGHT_THRESH) && TargetWeightData.SlotNodeWeight > ZERO_ANIMWEIGHT_THRESH)
		{
			FPoseContext targetContext(output);
			if (bEvaluate)
			{
				Source.Evaluate(targetContext);
				PostEvaluateSourcePose(targetContext);
			}
			const FAnimationPoseData targetPoseData(targetContext);
			FAnimationPoseData outputTargettPoseData(output);
			output.AnimInstanceProxy->SlotEvaluatePose(slotBlendInfo.TargetSlot, targetPoseData,
				TargetWeightData.SourceWeight, outputTargettPoseData, TargetWeightData.SlotNodeWeight, TargetWeightData.TotalNodeWeight);
		}
		else if (slotBlendInfo.BlendWeight <= ZERO_ANIMWEIGHT_THRESH && DefaultWeightData.SlotNodeWeight > ZERO_ANIMWEIGHT_THRESH)
		{
			FPoseContext defaultContext(output);
			if (bEvaluate)
			{
				Source.Evaluate(defaultContext);
				PostEvaluateSourcePose(defaultContext);
			}
			const FAnimationPoseData defulatPoseData(defaultContext);
			
			FAnimationPoseData outputDefaultPoseData(output);
			output.AnimInstanceProxy->SlotEvaluatePose(SlotName[0], defulatPoseData,
				DefaultWeightData.SourceWeight, outputDefaultPoseData, DefaultWeightData.SlotNodeWeight, DefaultWeightData.TotalNodeWeight);
		}
		else
		{
			Source.Evaluate(output);
			PostEvaluateSourcePose(output);
		}

		checkSlow(!output.ContainsNaN());
		checkSlow(output.IsNormalized());
	}
}

void FKMAnimNode_MultiSlot::GatherDebugData(FNodeDebugData& debugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	
	FString debugLine = debugData.GetNodeName(this);
	
	for (auto slotName : SlotName)
	{
		debugLine += FString::Printf(TEXT("(Slot Name: '%s' Weight:%.1f%%)"), *slotName.ToString(), DefaultWeightData.SlotNodeWeight*100.f);
		
		bool const bIsPoseSource = (DefaultWeightData.SourceWeight <= ZERO_ANIMWEIGHT_THRESH);
		debugData.AddDebugItem(debugLine, bIsPoseSource);
		Source.GatherDebugData(debugData.BranchFlow(DefaultWeightData.SourceWeight));

		for (FAnimMontageInstance* montageInstance : debugData.AnimInstance->MontageInstances)
		{
			if (montageInstance->IsValid() && montageInstance->Montage->IsValidSlot(slotName))
			{
				if (const FAnimTrack* const track = montageInstance->Montage->GetAnimationData(slotName))
				{
					if (const FAnimSegment* const segment = track->GetSegmentAtTime(montageInstance->GetPosition()))
					{
						float currentAnimPos;
						if (UAnimSequenceBase* anim = segment->GetAnimationData(montageInstance->GetPosition(), currentAnimPos))
						{
							FString montageLine = FString::Printf(TEXT("Montage('%s') Anim('%s') P(%.2f) W(%.0f%%)"), *montageInstance->Montage->GetName(), *anim->GetName(), currentAnimPos, DefaultWeightData.SlotNodeWeight*100.f);
							debugData.BranchFlow(1.0f).AddDebugItem(montageLine, true);
							break;
						}
					}
				}
			}
		}
	}
}
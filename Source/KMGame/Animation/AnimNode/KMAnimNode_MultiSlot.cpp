#include "KMAnimNode_MultiSlot.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimStats.h"
#include "Animation/AnimTrace.h"
#include "Animation/AnimNode_Inertialization.h"

FKMAnimNode_MultiSlot::FKMAnimNode_MultiSlot() : bAlwaysUpdateSourcePose(false)
{
	SlotName.Emplace(FAnimSlotGroup::DefaultSlotName);
}

bool FKMAnimNode_MultiSlot::GetBestSlotName(const FAnimInstanceProxy* animInstanceProxy, FName& outSlotName, FSlotNodeWeightInfo& outWeightData) const
{
	check(!SlotName.IsEmpty());

	outSlotName = SlotName[0];
	
	if (!animInstanceProxy)
	{
		return false;
	}
	for (int32 slotIndex = SlotName.Num() - 1; slotIndex >= 0; --slotIndex)
	{
		float slotNodeWeight;
		float sourceWeight;
		float totalNodeWeight;

		animInstanceProxy->GetSlotWeight(SlotName[slotIndex],slotNodeWeight,sourceWeight,totalNodeWeight);
		if (slotNodeWeight > ZERO_ANIMWEIGHT_THRESH)
		{
			outSlotName = SlotName[slotIndex];

			outWeightData.SlotNodeWeight = slotNodeWeight;
			outWeightData.SourceWeight = sourceWeight;
			outWeightData.TotalNodeWeight = totalNodeWeight;

			return true;
		}
	}

	outSlotName = NAME_None;
	outWeightData.Reset();
	return false;
}

void FKMAnimNode_MultiSlot::Initialize_AnyThread(const FAnimationInitializeContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(context);

	Source.Initialize(context);
	WeightData.Reset();

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

void FKMAnimNode_MultiSlot::Update_AnyThread(const FAnimationUpdateContext& context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)

	FSlotNodeWeightInfo slotNodeWeightInfo;
	FName bestSlotName = NAME_None;
	GetBestSlotName(context.AnimInstanceProxy, bestSlotName, slotNodeWeightInfo);

	context.AnimInstanceProxy->GetSlotWeight(bestSlotName, WeightData.SlotNodeWeight, WeightData.SourceWeight, WeightData.TotalNodeWeight);
	context.AnimInstanceProxy->UpdateSlotNodeWeight(bestSlotName, WeightData.SlotNodeWeight, context.GetFinalBlendWeight());

	FInertializationRequest inertializationRequest;
	if (context.AnimInstanceProxy->GetSlotInertializationRequestData(bestSlotName, inertializationRequest))
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

	const bool bUpdateSource = bAlwaysUpdateSourcePose || FAnimWeight::IsRelevant(WeightData.SourceWeight);
	if (bUpdateSource)
	{
		const float sourceWeight = FMath::Max(FAnimWeight::GetSmallestRelevantWeight(), WeightData.SourceWeight);
		Source.Update(context.FractionalWeight(sourceWeight));
	}

#if ANIM_TRACE_ENABLED
	TRACE_ANIM_NODE_VALUE(context, TEXT("Name"), bestSlotName);
	TRACE_ANIM_NODE_VALUE(context, TEXT("Slot Weight"), WeightData.SlotNodeWeight);
	TRACE_ANIM_NODE_VALUE(context, TEXT("Pose Source"), (WeightData.SourceWeight <= ZERO_ANIMWEIGHT_THRESH));

	context.AnimInstanceProxy->TraceMontageEvaluationData(context, bestSlotName);
#endif
}

void FKMAnimNode_MultiSlot::Evaluate_AnyThread(FPoseContext& output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(Slot, !IsInGameThread());

	if (WeightData.SlotNodeWeight <= ZERO_ANIMWEIGHT_THRESH)
	{
		Source.Evaluate(output);
		PostEvaluateSourcePose(output);
	}
	else
	{
		FPoseContext sourceContext(output);
		if (WeightData.SourceWeight > ZERO_ANIMWEIGHT_THRESH)
		{
			Source.Evaluate(sourceContext);
		}

		FSlotNodeWeightInfo slotNodeWeightInfo;
		FName bestSlotName = NAME_None;
		GetBestSlotName(output.AnimInstanceProxy, bestSlotName, slotNodeWeightInfo);

		if (bestSlotName != SlotName[0])
		{
			bestSlotName = bestSlotName;
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT("Best Slot Name: %s"), *bestSlotName.ToString()));
		}
		
		PostEvaluateSourcePose(sourceContext);

		const FAnimationPoseData sourcePoseData(sourceContext);
		FAnimationPoseData outputPoseData(output);
		output.AnimInstanceProxy->SlotEvaluatePose(bestSlotName, sourcePoseData,
			WeightData.SourceWeight, outputPoseData, WeightData.SlotNodeWeight, WeightData.TotalNodeWeight);

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
		debugLine += FString::Printf(TEXT("(Slot Name: '%s' Weight:%.1f%%)"), *slotName.ToString(), WeightData.SlotNodeWeight*100.f);
		
		bool const bIsPoseSource = (WeightData.SourceWeight <= ZERO_ANIMWEIGHT_THRESH);
		debugData.AddDebugItem(debugLine, bIsPoseSource);
		Source.GatherDebugData(debugData.BranchFlow(WeightData.SourceWeight));

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
							FString montageLine = FString::Printf(TEXT("Montage('%s') Anim('%s') P(%.2f) W(%.0f%%)"), *montageInstance->Montage->GetName(), *anim->GetName(), currentAnimPos, WeightData.SlotNodeWeight*100.f);
							debugData.BranchFlow(1.0f).AddDebugItem(montageLine, true);
							break;
						}
					}
				}
			}
		}
	}
}
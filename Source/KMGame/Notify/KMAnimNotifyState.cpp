#include "KMAnimNotifyState.h"
#include "GameObject/KMCharacterInstance.h"

UKMAnimNotifyState::UKMAnimNotifyState(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{

}

bool UKMAnimNotifyState::IsCompareTag(const UKMCharacterInstance* ownerCharacterInstance) const
{
	if (CompareTags.IsEmpty())
	{
		return true;
	}

	bool bHasOr = false;
	bool bAndResult = true;

	for (auto& compareTag : CompareTags)
	{
		bool bMatched = false;

		if (compareTag.Compare == EKMTageCompareType::Equal)
		{
			bMatched = ownerCharacterInstance->HasGameplayTag(compareTag.Tag);
		}
		else
		{
			bMatched = !ownerCharacterInstance->HasGameplayTag(compareTag.Tag);
		}

		if (compareTag.Operator == EKMTageOperatorType::Or)
		{
			bHasOr = true;
			if (bMatched == true)
			{
				return true;
			}
		}
		else
		{
			bAndResult &= bMatched;
		}
	}

	if (bHasOr == true)
	{
		return false;
	}

	return bAndResult;
}
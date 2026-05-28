
#include "EMMetaSheet.h"


void EMMetaSheet::AddMeta(FString VariableName, FString MetaKey, FString MetaValue)
{
	if (VariableName.IsEmpty() | MetaKey.IsEmpty() | MetaValue.IsEmpty())
	{
		return;
	}
	TArray<MetaProperty>& Property = MetaPropertyMap.FindOrAdd(VariableName);
	Property.Add(TPair<FString, FString>(MetaKey, MetaValue));
}

FString EMMetaSheet::GetMetaString(FString VariableName, FString DefaultMeta)
{
	TArray<MetaProperty>* Propertise = MetaPropertyMap.Find(VariableName);
	if (Propertise == nullptr)
	{
		return DefaultMeta.IsEmpty() ? TEXT("") : DefaultMeta;
	}
	else
	{
		FString MetaStr = TEXT("meta = (");
		
		//변수에 등록된 메타 설정이 있을 경우
		if (false == DefaultMeta.IsEmpty())
		{
			MetaStr = DefaultMeta.TrimEnd();
			MetaStr = MetaStr.Replace(TEXT(")"), TEXT(", "));
		}

		FString Body = TEXT("");
		for (MetaProperty Property : *Propertise)
		{
			Body += FString::Printf(TEXT("%s=\"%s\", "), *Property.Key, *Property.Value);
		}

		if(Body.IsEmpty() == false)
		{
			MetaStr.Append(Body);
		}
		MetaStr.RemoveFromEnd(", ");
		MetaStr.Append(")");

		return MetaStr;
	}
}
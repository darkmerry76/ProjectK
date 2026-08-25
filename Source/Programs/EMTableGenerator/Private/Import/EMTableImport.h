#pragma once

#include "EMTableGenerator.h"
#include "Config/EMTableConfig.h"
#include "SheetBuilder/EMSheet.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImport
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableImport
{
public:
	virtual ~EMTableImport() {};
	virtual void Clear() = 0;

	virtual bool DoImport(const FString& Path) { return false; };

	TMap<FString, TArray<TSharedPtr<class EMSheet>>>& GetMutableSheetAnalizerMaps() { return SheetAnalizerMaps; }

protected:
	void AddSheetAnalizer(const FString& ExcelName, const FString& SheetName, TSharedPtr<class EMSheet> Analizer)
	{
		check(Analizer.IsValid());

		FString finalSheetName = EMTableConfig::Get().GetExcelFileSection().GetFinalSheetName(SheetName);;

		int32 hashIndex;

		if (finalSheetName.FindChar('#', hashIndex) == true)
		{
			int32 closeIndex;
			if (finalSheetName.FindChar('>', closeIndex) && closeIndex > hashIndex)
			{
				finalSheetName.RemoveAt(hashIndex, closeIndex - hashIndex);
			}
			else
			{
				finalSheetName.RemoveAt(hashIndex, finalSheetName.Len() - hashIndex);
			}
		}

		Analizer->SetExcelName(ExcelName);
		Analizer->SetSheetName(finalSheetName);

		TArray<TSharedPtr<class EMSheet>>* AnalizersResult = SheetAnalizerMaps.Find(finalSheetName);
		if(nullptr == AnalizersResult)
		{
			TArray<TSharedPtr<class EMSheet>> NewAnalizers;
			NewAnalizers.Emplace(Analizer);
			SheetAnalizerMaps.Emplace(finalSheetName, NewAnalizers);
		}
		else
		{
			AnalizersResult->Emplace(Analizer);
		}
	}

protected:
	TMap<FString, TArray<TSharedPtr<class EMSheet>>>	SheetAnalizerMaps;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportFactoryBase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableImportFactoryBase
{
public:
	virtual TSharedPtr<EMTableImport> DoImport(const FString& Path) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportFactory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename _TL>
class EMTableImportFactory : public EMTableImportFactoryBase
{
public:
	virtual TSharedPtr<EMTableImport> DoImport(const FString& Path) override
	{
		TSharedPtr<_TL> NewImport = MakeShareable(new _TL());
		if (false == NewImport->DoImport(Path))
		{
			return nullptr;
		}
		return NewImport;
	};
};
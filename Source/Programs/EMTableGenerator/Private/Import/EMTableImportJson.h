#pragma once

#include "CoreMinimal.h"
#include "EMTableGenerator.h"
#include "EMTableImport.h"
#include "Schema/EMSchemaMgr.h"
#include "SheetBuilder/EMStructure.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EMTableImportJSON
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EMTableImportJSON : public EMTableImport
{
public:
	EMTableImportJSON();
	virtual ~EMTableImportJSON()
	{
		Clear();
	}

	virtual void Clear() override;
	virtual bool DoImport(const FString& Path) override;

protected:
	TSharedPtr<FEMStructure> SetStructuresMembers(const TSharedPtr<EMSchemaItem>& PropertyItem, const EMSchemaItem* ParentItem = nullptr, bool IsArray = false);
	bool SetStructuresMember(TSharedPtr<FEMStructure> ParentJsonStruct, const TSharedPtr<EMSchemaItem>& PropertyItem, const EMSchemaItem* ParentItem = nullptr, bool IsArray = false);
	bool SetStructureProperties(TSharedPtr<FEMStructure> ParentJsonStruct, const FName& MemberName, const TSharedPtr<EMSchemaItem>& PropertyItem, const EMSchemaItem* ParentItem, bool IsArray);
	const TArray<TSharedPtr<EMSchemaItem>> GetMemberItems(const TSharedPtr<EMSchemaItem>& PropertyItem) const;
	void CollectStructureContentConfig(TSharedPtr<FEMStructure> ParentJsonStruct, const TSharedPtr<EMSchemaItem>& PropertyItem);

	bool IsReservedJsonMember(const TSharedPtr<EMSchemaItem>& PropertyItem) const;

protected:
	TSet<FName> ReservedJsonMemberSet;
};
#include "EMDataTable.h"

#include "LinkerLoad.h"
#include "UserDefinedStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EMDataTable)

LLM_DEFINE_TAG(DataTable);

UDataTable::FScopedDataTableChange::FScopedDataTableChange(UDataTable* InTable)
	: Table(InTable)
{
	FScopeLock Lock(&CriticalSection);
	int32& Count = ScopeCount.FindOrAdd(Table);
	++Count;
}

UDataTable::FScopedDataTableChange::~FScopedDataTableChange()
{
	FScopeLock Lock(&CriticalSection);
	int32& Count = ScopeCount.FindChecked(Table);
	--Count;
	if (Count == 0)
	{
		Table->HandleDataTableChanged();
		ScopeCount.Remove(Table);
	}
}

TMap<UDataTable*, int32> UDataTable::FScopedDataTableChange::ScopeCount;
FCriticalSection UDataTable::FScopedDataTableChange::CriticalSection;

#define DATATABLE_CHANGE_SCOPE()	UDataTable::FScopedDataTableChange ActiveScope(this);

UDataTable::UDataTable(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	bIgnoreExtraFields = false;
	bIgnoreMissingFields = false;
	bStripFromClientBuilds = false;
	bPreserveExistingValues = false;
}

#if WITH_EDITOR
void UDataTable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

#if WITH_EDITORONLY_DATA
	HandleDataTableChanged();
#endif
}
#endif

void UDataTable::LoadStructData(FStructuredArchiveSlot Slot)
{
	UScriptStruct* LoadUsingStruct = RowStruct;
	if (!LoadUsingStruct)
	{
		LoadUsingStruct = FTableRowBase::StaticStruct();
	}

	int32 NumRows;
	FStructuredArchiveArray Array = Slot.EnterArray(NumRows);

	DATATABLE_CHANGE_SCOPE();

	RowMap.Reserve(NumRows);
	for (int32 RowIdx = 0; RowIdx < NumRows; RowIdx++)
	{
		FStructuredArchiveRecord RowRecord = Array.EnterElement().EnterRecord();

		// Load row name
		FName RowName;
		RowRecord << SA_VALUE(TEXT("Name"), RowName);

		// Load row data
		uint8* RowData = (uint8*)FMemory::Malloc(LoadUsingStruct->GetStructureSize());

		// And be sure to call DestroyScriptStruct later
		LoadUsingStruct->InitializeStruct(RowData);

		LoadUsingStruct->SerializeItem(RowRecord.EnterField(TEXT("Value")), RowData, nullptr);

		// Add to map
		RowMap.Add(RowName, RowData);
	}
}

void UDataTable::SaveStructData(FStructuredArchiveSlot Slot)
{
	UScriptStruct* SaveUsingStruct = RowStruct;
	if (!SaveUsingStruct)
	{
		SaveUsingStruct = FTableRowBase::StaticStruct();
	}

	int32 NumRows = RowMap.Num();
	FStructuredArchiveArray Array = Slot.EnterArray(NumRows);

	// Now iterate over rows in the map
	for (auto RowIt = RowMap.CreateIterator(); RowIt; ++RowIt)
	{
		// Save out name
		FName RowName = RowIt.Key();
		FStructuredArchiveRecord Row = Array.EnterElement().EnterRecord();
		Row << SA_VALUE(TEXT("Name"), RowName);

		// Save out data
		uint8* RowData = RowIt.Value();

		SaveUsingStruct->SerializeItem(Row.EnterField(TEXT("Value")), RowData, nullptr);
	}
}

void UDataTable::GetPreloadDependencies(TArray<UObject*>& OutDeps)
{
	Super::GetPreloadDependencies(OutDeps);
	OutDeps.Add(RowStruct);
}

void UDataTable::OnPostDataImported(TArray<FString>& OutCollectedImportProblems)
{
	if (RowStruct)
	{
		const bool bIsNativeRowStruct = RowStruct->IsChildOf(FTableRowBase::StaticStruct());

		FString DataTableTextNamespace = GetName();
		for (const TPair<FName, uint8*>& TableRowPair : RowMap)
		{
			if (bIsNativeRowStruct)
			{
				FTableRowBase* CurRow = reinterpret_cast<FTableRowBase*>(TableRowPair.Value);
				CurRow->OnPostDataImport(this, TableRowPair.Key, OutCollectedImportProblems);
			}

#if WITH_EDITOR
			// Perform automatic fix-up on any text properties that have been imported from a raw string to assign them deterministic keys
			// We do this after OnPostDataImport has been run on the row, as that function may perform custom fix-up logic that will fix the keys differently than the default logic
			StabilizeLocalizationKeys::StabilizeLocalizationKeysForStruct(RowStruct, TableRowPair.Value, DataTableTextNamespace, TableRowPair.Key.ToString());
#endif
		}
	}
	
	// Don't need to call HandleDataTableChanged because it gets called by the scope and post edit callbacks
	// If you need to handle an import-specific problem, register with FDataTableEditorUtils
}

void UDataTable::HandleDataTableChanged(FName ChangedRowName)
{
	if (!IsValidChecked(this) || IsUnreachable() || HasAnyFlags(RF_BeginDestroyed))
	{
		// This gets called during destruction, don't broadcast callbacks
		return;
	}

	// Do the row fixup before global callback
	if (RowStruct)
	{
		const bool bIsNativeRowStruct = RowStruct->IsChildOf(FTableRowBase::StaticStruct());

		if (bIsNativeRowStruct)
		{
			for (const TPair<FName, uint8*>& TableRowPair : RowMap)
			{
				if (ChangedRowName != NAME_None && ChangedRowName != TableRowPair.Key)
				{
					continue;
				}

				FTableRowBase* CurRow = reinterpret_cast<FTableRowBase*>(TableRowPair.Value);
				CurRow->OnDataTableChanged(this, TableRowPair.Key);
			}
		}
	}

	OnDataTableChanged().Broadcast();
}

void UDataTable::Serialize(FStructuredArchiveRecord Record)
{
	FArchive& BaseArchive = Record.GetUnderlyingArchive();
	LLM_SCOPE_BYTAG(DataTable);

	Super::Serialize(Record); // When loading, this should load our RowStruct!	

	if (RowStruct && RowStruct->HasAnyFlags(RF_NeedLoad))
	{
		auto RowStructLinker = RowStruct->GetLinker();
		if (RowStructLinker)
		{
			RowStructLinker->Preload(RowStruct);
		}
	}

	if(BaseArchive.IsLoading())
	{
		DATATABLE_CHANGE_SCOPE();
		EmptyTable();
		LoadStructData(Record.EnterField(TEXT("Data")));
	}
	else if(BaseArchive.IsSaving())
	{
		SaveStructData(Record.EnterField(TEXT("Data")));
	}
}

void UDataTable::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{	
	UDataTable* This = CastChecked<UDataTable>(InThis);

	// Need to emit references for referenced rows (unless there's no properties that reference UObjects)
	if(This->RowStruct != nullptr && This->RowStruct->RefLink != nullptr)
	{
		// Now iterate over rows in the map
		for (const TPair<FName, uint8*>& Pair : This->RowMap)
		{
			if (uint8* RowData = Pair.Value)
			{
				Collector.AddPropertyReferencesWithStructARO(This->RowStruct, RowData, This);
			}
		}
	}

	Super::AddReferencedObjects( This, Collector );
}

void UDataTable::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize)
{
	Super::GetResourceSizeEx(CumulativeResourceSize);

	CumulativeResourceSize.AddDedicatedSystemMemoryBytes(RowMap.GetAllocatedSize());
	if (RowStruct)
	{
		CumulativeResourceSize.AddDedicatedSystemMemoryBytes(RowMap.Num() * RowStruct->GetStructureSize());
	}
}

void UDataTable::FinishDestroy()
{
	Super::FinishDestroy();
	if(!IsTemplate())
	{
		EmptyTable(); // Free memory when UObject goes away
	}
}


UScriptStruct& UDataTable::GetEmptyUsingStruct() const
{
	UScriptStruct* EmptyUsingStruct = RowStruct;
	if (!EmptyUsingStruct)
	{
		EmptyUsingStruct = FTableRowBase::StaticStruct();
	}

	return *EmptyUsingStruct;
}

void UDataTable::EmptyTable()
{
	DATATABLE_CHANGE_SCOPE();

	UScriptStruct& EmptyUsingStruct = GetEmptyUsingStruct();

	// Iterate over all rows in table and free mem
	for (auto RowIt = RowMap.CreateIterator(); RowIt; ++RowIt)
	{
		uint8* RowData = RowIt.Value();
		EmptyUsingStruct.DestroyStruct(RowData);
		FMemory::Free(RowData);
	}

	// Finally empty the map
	RowMap.Empty();
}

void UDataTable::RemoveRow(FName RowName)
{
	DATATABLE_CHANGE_SCOPE();

	RemoveRowInternal(RowName);
}

void UDataTable::RemoveRowInternal(FName RowName)
{
	UScriptStruct& EmptyUsingStruct = GetEmptyUsingStruct();

	uint8* RowData = nullptr;
	RowMap.RemoveAndCopyValue(RowName, RowData);
		
	if (RowData)
	{
		EmptyUsingStruct.DestroyStruct(RowData);
		FMemory::Free(RowData);
	}
}

void UDataTable::AddRow(FName RowName, const FTableRowBase& RowData)
{
	DATATABLE_CHANGE_SCOPE();

	UScriptStruct& EmptyUsingStruct = GetEmptyUsingStruct();

	// We want to delete the row memory even for child classes that override remove
	RemoveRowInternal(RowName);
		
	uint8* NewRawRowData = (uint8*)FMemory::Malloc(EmptyUsingStruct.GetStructureSize());
	
	EmptyUsingStruct.InitializeStruct(NewRawRowData);
	EmptyUsingStruct.CopyScriptStruct(NewRawRowData, &RowData);

	// Add to map
	AddRowInternal(RowName, NewRawRowData);
}

void UDataTable::AddRow(FName RowName, const uint8* RowData, const UScriptStruct* RowType)
{
	DATATABLE_CHANGE_SCOPE();

	UScriptStruct& EmptyUsingStruct = GetEmptyUsingStruct();

	checkf(RowType == &EmptyUsingStruct, TEXT("AddRow called with an incompatible row type! Got '%s', but expected '%s'"), *RowType->GetPathName(), *EmptyUsingStruct.GetPathName());

	// We want to delete the row memory even for child classes that override remove
	RemoveRowInternal(RowName);

	uint8* NewRawRowData = (uint8*)FMemory::Malloc(EmptyUsingStruct.GetStructureSize());

	EmptyUsingStruct.InitializeStruct(NewRawRowData);
	EmptyUsingStruct.CopyScriptStruct(NewRawRowData, RowData);

	// Add to map
	AddRowInternal(RowName, NewRawRowData);
}

void UDataTable::AddRowInternal(FName RowName, uint8* RowData)
{
	RowMap.Add(RowName, RowData);
}

/** Returns the column property where PropertyName matches the name of the column property. Returns NULL if no match is found or the match is not a supported table property */
FProperty* UDataTable::FindTableProperty(const FName& PropertyName) const
{
	FProperty* Property = nullptr;

	if (RowStruct)
	{
		Property = RowStruct->FindPropertyByName(PropertyName);
		if (Property == nullptr && RowStruct->IsA<UUserDefinedStruct>())
		{
			const FString PropertyNameStr = PropertyName.ToString();

			for (TFieldIterator<FProperty> It(RowStruct); It; ++It)
			{
				if (PropertyNameStr == RowStruct->GetAuthoredNameForField(*It))
				{
					Property = *It;
					break;
				}
			}
		}
	}

	return Property;
}

TArray<FName> UDataTable::GetRowNames() const
{
	TArray<FName> Keys;
	RowMap.GetKeys(Keys);
	return Keys;
}

bool FDataTableRowHandle::operator==(FDataTableRowHandle const& Other) const
{
	return DataTable == Other.DataTable && RowName == Other.RowName;
}

bool FDataTableRowHandle::operator != (FDataTableRowHandle const& Other) const
{
	return DataTable != Other.DataTable || RowName != Other.RowName;
}

void FDataTableRowHandle::PostSerialize(const FArchive& Ar)
{
	if (Ar.IsSaving() && !IsNull() && DataTable)
	{
		// Note which row we are pointing to for later searching
		Ar.MarkSearchableName(DataTable, RowName);
	}
}

bool FDataTableCategoryHandle::operator==(FDataTableCategoryHandle const& Other) const
{
	return DataTable == Other.DataTable && ColumnName == Other.ColumnName && RowContents == Other.RowContents;
}

bool FDataTableCategoryHandle::operator != (FDataTableCategoryHandle const& Other) const
{
	return DataTable != Other.DataTable || ColumnName != Other.ColumnName || RowContents != Other.RowContents;
}
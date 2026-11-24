#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetData.h"

class USkeleton;

// ---------------------------------------
// Batch Conversion Result
// ---------------------------------------

struct FCharacterConverterResult
{
    int32 TotalCount = 0;

    int32 SuccessCount = 0;

    int32 FailedCount = 0;

    int32 SkippedCount = 0;

    double ElapsedTime = 0.0;

    TArray<FString> FailedAssets;

    TArray<FString> SkippedAssets;
};

// ---------------------------------------
// Character Converter
// ---------------------------------------

class FCharacterConverter
{
public:

    static bool FindAnimations(
        const USkeleton* SourceSkeleton,
        TArray<FAssetData>& OutAssets
    );

    static bool ConvertAllAnimations(
        USkeleton* SourceSkeleton,
        USkeleton* TargetSkeleton,
        const FString& OutputFolder,
        FCharacterConverterResult& OutResult
    );

    static bool ConvertCharacter(
        USkeleton* SourceSkeleton,
        USkeleton* ReferenceSkeleton,
        const FString& OutputFolder
    );

    static void DumpResult(
        const FCharacterConverterResult& Result
    );

    static void CommandConvert(
        const TArray<FString>& Args
    );

    static void CommandConvertAnimations(
        const TArray<FString>& Args
    );

    static void CommandListAnimations(
        const TArray<FString>& Args
    );
    
    static bool ConvertAll(
    USkeleton* SourceSkeleton,
    USkeleton* TargetSkeleton,
    const FString& OutputFolder
);

    static void CommandConvertAll(
        const TArray<FString>& Args
    );
};
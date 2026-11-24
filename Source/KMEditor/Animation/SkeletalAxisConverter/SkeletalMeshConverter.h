#pragma once

#include "CoreMinimal.h"

class USkeleton;
class USkeletalMesh;

class FSkeletalMeshConverter
{
public:

	static USkeletalMesh* CreatePreviewMesh(
		USkeleton* Skeleton,
		const FString& PackagePath,
		const FString& AssetName
	);

	static bool ValidateReferenceSkeleton(
		const USkeleton* Skeleton,
		const USkeletalMesh* SkeletalMesh
	);

	static void Test(
		const TArray<FString>& Args
	);
};
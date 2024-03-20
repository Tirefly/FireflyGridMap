// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FireflyGridMapLibrary.generated.h"

class UFireflyGridBase;

/**
 * 
 */
UCLASS()
class FIREFLYGRIDMAP_API UFireflyGridMapLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 获取距离当前Actor所在的棋格最近且空闲的棋格
	UFUNCTION(BlueprintCallable, Category = TraceTarget, Meta = (WorldContext = "WorldContext"))
	static UFireflyGridBase* GetNearestVacantGridOfMap(const UObject* WorldContext, AActor* Actor);
	
};

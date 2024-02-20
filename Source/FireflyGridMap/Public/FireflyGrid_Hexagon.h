// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FireflyGridBase.h"
#include "FireflyGrid_Hexagon.generated.h"


UCLASS()
class FIREFLYGRIDMAP_API UFireflyGrid_Hexagon : public UFireflyGridBase
{
	GENERATED_BODY()

#pragma region Grid

public:
	UFireflyGrid_Hexagon(const FObjectInitializer& ObjectInitializer);

	//基础-获取相邻棋格-覆盖
	virtual TArray<UFireflyGridBase*> GetNeighbors() override;

	//寻路-获取两两棋格的实际距离-覆盖
	virtual float GetRealRadiusSize() const override { return Size * FMath::Sqrt(3.f) * 0.5; };

	//模型-绘制模型-覆盖
	virtual void DrawGrid(TArray<FVector>& InVertices,
		TArray<int32>& InIndecies,
		TArray<FVector>& InNormals,
		TArray<FVector2D>& InUV,
		TArray<FColor>& InVertexColors,
		TArray<FVector>& InTangents,
		FVector InOffset) override;

public:
	//基础-右上棋格
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid|Hexagon")
	UFireflyGrid_Hexagon* RightUpGrid;

	//基础-右棋格
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid|Hexagon")
	UFireflyGrid_Hexagon* RightGrid;

	//基础-右下棋格
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid|Hexagon")
	UFireflyGrid_Hexagon* RightDownGrid;

	//基础-左下棋格
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid|Hexagon")
	UFireflyGrid_Hexagon* LeftDownGrid;

	//基础-左棋格
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid|Hexagon")
	UFireflyGrid_Hexagon* LeftGrid;

	//基础-左上棋格
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid|Hexagon")
	UFireflyGrid_Hexagon* LeftUpGrid;

#pragma endregion
};

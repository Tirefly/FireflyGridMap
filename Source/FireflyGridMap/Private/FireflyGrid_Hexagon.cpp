// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyGrid_Hexagon.h"


UFireflyGrid_Hexagon::UFireflyGrid_Hexagon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GridShape = EGridShape::Hexagon;
}

TArray<UFireflyGridBase*> UFireflyGrid_Hexagon::GetNeighbors()
{
	TArray<UFireflyGridBase*> OutGrids;

	if (IsValid(RightUpGrid))
		OutGrids.Add(RightUpGrid);

	if (IsValid(RightGrid))
		OutGrids.Add(RightGrid);

	if (IsValid(RightDownGrid))
		OutGrids.Add(RightDownGrid);

	if (IsValid(LeftDownGrid))
		OutGrids.Add(LeftDownGrid);

	if (IsValid(LeftGrid))
		OutGrids.Add(LeftGrid);

	if (IsValid(LeftUpGrid))
		OutGrids.Add(LeftUpGrid);

	return OutGrids;
}

void UFireflyGrid_Hexagon::DrawGrid(TArray<FVector>& InVertices, TArray<int32>& InIndecies, TArray<FVector>& InNormals,
	TArray<FVector2D>& InUV, TArray<FColor>& InVertexColors, TArray<FVector>& InTangents, FVector InOffset)
{
	//顶点
	TArray<FVector> HexVerts;
	HexVerts.AddUninitialized(7);
	HexVerts[0] = WorldTransform.GetLocation() - InOffset;
	HexVerts[1] = FVector(Size, 0, 0) + WorldTransform.GetLocation() - InOffset;
	HexVerts[2] = FVector(0.5 * Size, 0.5 * FMath::Sqrt(3.f) * Size, 0) + WorldTransform.GetLocation() - InOffset;
	HexVerts[3] = FVector(-0.5 * Size, 0.5 * FMath::Sqrt(3.f) * Size, 0) + WorldTransform.GetLocation() - InOffset;
	HexVerts[4] = FVector(-Size, 0, 0) + WorldTransform.GetLocation() - InOffset;
	HexVerts[5] = FVector(-0.5 * Size, -0.5 * FMath::Sqrt(3.f) * Size, 0) + WorldTransform.GetLocation() - InOffset;
	HexVerts[6] = FVector(0.5 * Size, -0.5 * FMath::Sqrt(3.f) * Size, 0) + WorldTransform.GetLocation() - InOffset;

	//顶点索引
	TArray<int32> Indices;
	Indices.AddUninitialized(18);
	Indices[0] = 0; Indices[1] = 2; Indices[2] = 1;
	Indices[3] = 0; Indices[4] = 3; Indices[5] = 2;
	Indices[6] = 0; Indices[7] = 4; Indices[8] = 3;
	Indices[9] = 0; Indices[10] = 5; Indices[11] = 4;
	Indices[12] = 0; Indices[13] = 6; Indices[14] = 5;
	Indices[15] = 0; Indices[16] = 1; Indices[17] = 6;

	//法线
	TArray<FVector> Normals;
	Normals.Init(FVector(0, 0, 1), 7);

	//UV
	TArray<FVector2D> UV;
	UV.Init(FVector2D(0.5f, 0.5f), 7);
	UV[1] += FVector2D(0.0f, 0.5f);
	UV[2] += FVector2D(0.25f * FMath::Sqrt(3.f), 0.25f);
	UV[3] += FVector2D(0.25f * FMath::Sqrt(3.f), -0.25f);
	UV[4] += FVector2D(0.0f, -0.5f);
	UV[5] += FVector2D(-0.25f * FMath::Sqrt(3.f), -0.25f);
	UV[6] += FVector2D(-0.25f * FMath::Sqrt(3.f), 0.25f);

	//顶点颜色
	TArray<FColor> VertexColors;
	VertexColors.Init(FColor::White, 7);

	//切线
	TArray<FVector> Tangents;
	Tangents.Init(FVector(1, 0, 0), 7);

	//回传
	InVertices = HexVerts;
	InIndecies = Indices;
	InNormals = Normals;
	InUV = UV;
	InVertexColors = VertexColors;
	InTangents = Tangents;
}

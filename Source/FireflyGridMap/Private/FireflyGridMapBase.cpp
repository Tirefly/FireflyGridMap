// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyGridMapBase.h"

#include "FireflyGrid_Hexagon.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AFireflyGridMapBase::AFireflyGridMapBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshOfMap = CreateDefaultSubobject<UProceduralMeshComponent>("MeshOfMap");
	if (MeshOfMap)
	{
		SetRootComponent(MeshOfMap);
		MeshOfMap->bUseAsyncCooking = true;
	}
}

// Called when the game starts or when spawned
void AFireflyGridMapBase::BeginPlay()
{
	Super::BeginPlay();

	//重新生成棋盘
	GenerateGridMap();
}

void AFireflyGridMapBase::PostInitProperties()
{
	Super::PostInitProperties();

	GenerateGridMap();
}

#if WITH_EDITOR
void AFireflyGridMapBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//当行、列、大小改变时重新生成棋盘
	if (PropertyChangedEvent.Property->GetName() == "MapRoll"
		|| PropertyChangedEvent.Property->GetName() == "MapColumn"
		|| PropertyChangedEvent.Property->GetName() == "GridRadius")
	{
		//重新生成棋盘
		GenerateGridMap();
	}
}
#endif

void AFireflyGridMapBase::GenerateGridMap()
{
	for (const auto Grid : GridsOfMap)
	{
		//标记垃圾回收
		if (Grid.Value)
			Grid.Value->MarkAsGarbage();
	}
	//重置Map
	GridsOfMap.Reset();
	//重新生成棋格
	GenerateGrids(GridRadius, MapRoll, MapColumn);
	//重新生成模型
	GenerateGridMeshes();
}

UFireflyGridBase* AFireflyGridMapBase::GetGridOfMap(FFireflyGridCoordinate InCoordinate) const
{
	if (GridsOfMap.Contains(InCoordinate))
	{
		return GridsOfMap[InCoordinate];
	}

	return nullptr;
}

void AFireflyGridMapBase::GenerateHexagonGrids(float InHexSize, int InRoll, int InColumn)
{
	FVector tHexLocation;
	for (int i = 0; i < InRoll; ++i)
	{
		for (int j = 0; j < InColumn; ++j)
		{
			if (i % 2 != 0 && j == InColumn - 1)
			{
				continue;
			}

			//UE中横向坐标轴为Y，纵向坐标轴为X，需要调换在二维坐标系XY的值
			FFireflyGridCoordinate tHexVector = FFireflyGridCoordinate(j, i);
			tHexLocation.X = 1.5 * InHexSize * i;
			tHexLocation.Y = i % 2 == 0 ? (FMath::Sqrt(3.f) * InHexSize * j)
				: (FMath::Sqrt(3.f) * InHexSize * j + FMath::Sqrt(3.f) * 0.5 * InHexSize);
			tHexLocation.Z = 0;
			tHexLocation += GetActorLocation();
			UFireflyGridBase* tGrid = NewObject<UFireflyGrid_Hexagon>(this);

			//棋格初始化
			tGrid->InitGrid(this, FTransform(GetActorRotation(), tHexLocation), tHexVector, InHexSize);
			GridsOfMap.Add(tHexVector, tGrid);
		}
	}
}

void AFireflyGridMapBase::GenerateGrids(float InSize, int InRoll, int InColumn)
{
	switch (MapShape)
	{
		case EGridShape::None:
			break;
		//六边形棋格
		case EGridShape::Hexagon:
			GenerateHexagonGrids(InSize, InRoll, InColumn);
			break;
		default:
			break;
	}

	//初始化棋格
	InitGridMap();
}

void AFireflyGridMapBase::InitHexagonGridMap()
{
	//设置六边形棋格相邻棋格，判断对应坐标棋格是否存在，并赋值
	for (auto a : GridsOfMap)
	{
		UFireflyGrid_Hexagon* tHexGrid = Cast<UFireflyGrid_Hexagon>(a.Value);
		if (!tHexGrid)
			continue;
		FFireflyGridCoordinate tRightUp = a.Key.Y % 2 == 0 ? a.Key + FFireflyGridCoordinate(0, 1) : a.Key + FFireflyGridCoordinate(1, 1);
		if (GridsOfMap.Contains(tRightUp) && GridsOfMap[tRightUp]->IsA(UFireflyGrid_Hexagon::StaticClass()))
			tHexGrid->RightUpGrid = Cast<UFireflyGrid_Hexagon>(GridsOfMap[tRightUp]);

		FFireflyGridCoordinate tRight = a.Key + FFireflyGridCoordinate(1, 0);
		if (GridsOfMap.Contains(tRight) && GridsOfMap[tRight]->IsA(UFireflyGrid_Hexagon::StaticClass()))
			tHexGrid->RightGrid = Cast<UFireflyGrid_Hexagon>(GridsOfMap[tRight]);

		FFireflyGridCoordinate tRightDown = a.Key.Y % 2 == 0 ? a.Key + FFireflyGridCoordinate(0, -1) : a.Key + FFireflyGridCoordinate(1, -1);
		if (GridsOfMap.Contains(tRightDown) && GridsOfMap[tRightDown]->IsA(UFireflyGrid_Hexagon::StaticClass()))
			tHexGrid->RightDownGrid = Cast<UFireflyGrid_Hexagon>(GridsOfMap[tRightDown]);

		FFireflyGridCoordinate tLeftDown = a.Key.Y % 2 == 0 ? a.Key + FFireflyGridCoordinate(-1, -1) : a.Key + FFireflyGridCoordinate(0, -1);
		if (GridsOfMap.Contains(tLeftDown) && GridsOfMap[tLeftDown]->IsA(UFireflyGrid_Hexagon::StaticClass()))
			tHexGrid->LeftDownGrid = Cast<UFireflyGrid_Hexagon>(GridsOfMap[tLeftDown]);

		FFireflyGridCoordinate tLeft = a.Key + FFireflyGridCoordinate(-1, 0);
		if (GridsOfMap.Contains(tLeft) && GridsOfMap[tLeft]->IsA(UFireflyGrid_Hexagon::StaticClass()))
			tHexGrid->LeftGrid = Cast<UFireflyGrid_Hexagon>(GridsOfMap[tLeft]);

		FFireflyGridCoordinate tLeftUp = a.Key.Y % 2 == 0 ? a.Key + FFireflyGridCoordinate(-1, 1) : a.Key + FFireflyGridCoordinate(0, 1);
		if (GridsOfMap.Contains(tLeftUp) && GridsOfMap[tLeftUp]->IsA(UFireflyGrid_Hexagon::StaticClass()))
			tHexGrid->LeftUpGrid = Cast<UFireflyGrid_Hexagon>(GridsOfMap[tLeftUp]);

	}
}

void AFireflyGridMapBase::InitGridMap()
{
	switch (MapShape)
	{
		case EGridShape::None:
			break;
		//六边形棋格
		case EGridShape::Hexagon:
			InitHexagonGridMap();
			break;
		default:
			break;
	}
}

UFireflyGridBase* AFireflyGridMapBase::CheckHitHexagonGridOfMap(const FVector& InPosition)
{
	UFireflyGridBase* tHitGrid = nullptr;
	for (const auto Grid : GridsOfMap)
	{
		if (!Grid.Value)
			continue;
		const float GridX = FMath::Abs(InPosition.X - Grid.Value->WorldTransform.GetLocation().X);
		const float GridY = FMath::Abs(InPosition.Y - Grid.Value->WorldTransform.GetLocation().Y);
		const float Size = Grid.Value->Size;
		if (GridX > Size || GridY > FMath::Sqrt(3.f) * 0.5 * Size)
			continue;
		if (Size - GridX > GridY / FMath::Sqrt(3.f))
		{
			tHitGrid = Grid.Value;
			break;
		}
	}

	if (tHitGrid)
	{
		UKismetSystemLibrary::PrintString(this, tHitGrid->Coordinate.GetDebugString(), true, false);
	}

	return tHitGrid;
}

UFireflyGridBase* AFireflyGridMapBase::CheckHitGridOfMap(const FVector& InPosition)
{
	UFireflyGridBase* tHitGrid = nullptr;

	//判断Z轴，差距过大直接返回
	if (FMath::Abs(InPosition.Z - GetActorLocation().Z) > 1)
		return tHitGrid;
	switch (MapShape)
	{
		case EGridShape::None:
			break;
		case EGridShape::Hexagon:
			tHitGrid = CheckHitHexagonGridOfMap(InPosition);
			break;
		default:
			break;
	}
	return tHitGrid;
}

UFireflyGridBase* AFireflyGridMapBase::CheckMouseCursorGridOfMap()
{
	const APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		return nullptr;
	}
	
	FVector WorldLocation, WorldDirection;
	//屏幕坐标转世界坐标，回传坐标和方向
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		//获取Z轴差值
		float tScale = GetActorLocation().Z - WorldLocation.Z;
		tScale /= WorldDirection.Z;
		WorldLocation += tScale * WorldDirection;
		
		return CheckHitGridOfMap(WorldLocation);
	}

	return nullptr;
}

bool AFireflyGridMapBase::FindPath(TArray<UFireflyGridBase*>& Path, AActor* InActor, UFireflyGridBase* FromGrid,
	UFireflyGridBase* ToGrid, int StopSteps) const
{
	Path.Empty();

	//安全判断
	if (!FromGrid || !ToGrid)
		return false;
	if (!GridsOfMap.FindKey(FromGrid) || !GridsOfMap.FindKey(ToGrid))
		return false;

	//获取实际所有终点
	TArray<UFireflyGridBase*> ToGrids = GetGridNeighbors(ToGrid, StopSteps);
	for (int i = ToGrids.Num() - 1; i >= 0; i--)
	{
		if (!ToGrids[i]->CanPassGrid(InActor))
			ToGrids.RemoveAt(i);
	}

	//判断起点终点是否存在
	if (!FromGrid->CanPassGrid(InActor))
		return false;
	if (ToGrids.Num() == 0)
		return false;

	//判断是否已经到达终点
	if (ToGrids.Contains(FromGrid))
		return true;

	//将要遍历的路点
	TArray<UFireflyGridBase*> ListsToFind;
	//已经完成遍历的路点
	TArray<UFireflyGridBase*> ListsFound;

	//当前所在路点
	UFireflyGridBase* CurrentGrid = FromGrid;
	ListsToFind.Add(CurrentGrid);
	bool bFound = false;
	//A*寻路
	while (!bFound)
	{
		//从ListsToFind移除，加入ListsFound
		ListsToFind.Remove(CurrentGrid);
		ListsFound.Add(CurrentGrid);

		//获取相邻路点
		TArray<UFireflyGridBase*> Neighbors = CurrentGrid->GetNeighbors();
		for (auto Neighbor : Neighbors)
		{
			if (!Neighbor)
				continue;

			//判断相邻路点是否为终点
			if (ToGrids.Contains(Neighbor))
			{
				bFound = true;
				ToGrid = Neighbor;
				Neighbor->PreGrid = CurrentGrid;
			}

			//如果在ListsFound或不能通行则跳过
			if (ListsFound.Contains(Neighbor) || !Neighbor->CanPassGrid(InActor))
				continue;

			//如果不在ListToFind，则加入ListToFind的队尾，以备后用
			if (!ListsToFind.Contains(Neighbor))
			{
				ListsToFind.Add(Neighbor);
				Neighbor->PreGrid = CurrentGrid;
			}
		}

		//取出队首的路点，设置为下次循环遍历的路点
		if (ListsToFind.Num() <= 0)
		{
			break;
		}
		
		CurrentGrid = ListsToFind[0];
	}

	ListsToFind.Empty();
	ListsFound.Empty();

	//找到了路径
	if (bFound)
	{
		UFireflyGridBase* tGrid = ToGrid;
		while (tGrid != FromGrid)
		{
			Path.Add(tGrid);
			tGrid = Cast<UFireflyGridBase>(tGrid->PreGrid);
		}
		//获取的路径时从终点->起点的路径，需要反转回起点->终点的路径
		Algo::Reverse(Path);

		return true;
	}

	return false;
}

bool AFireflyGridMapBase::IsPathExist(AActor* InActor, UFireflyGridBase* FromGrid, UFireflyGridBase* ToGrid,
	int StopSteps) const
{
	//回传FindPath的值
	TArray<UFireflyGridBase*> Path;

	return FindPath(Path, InActor, FromGrid, ToGrid, StopSteps);
}

TArray<UFireflyGridBase*> AFireflyGridMapBase::GetGridNeighbors(UFireflyGridBase* InGrid, int InStep) const
{
	int32 NeighborSteps = InStep;

	TArray<UFireflyGridBase*> CurrentCheckList;
	TArray<UFireflyGridBase*> NextCheckList;
	TArray<UFireflyGridBase*> FoundList;
	NextCheckList.AddUnique(InGrid);
	FoundList.AddUnique(InGrid);

	//使用While，向外层层判断
	while (NeighborSteps > 0)
	{
		CurrentCheckList = NextCheckList;
		NextCheckList.Empty();
		for (UFireflyGridBase* Grid : CurrentCheckList)
		{
			if (!Grid)
				continue;
			TArray<UFireflyGridBase*> neighbors = Grid->GetNeighbors();
			for (UFireflyGridBase* b : neighbors)
			{
				if (FoundList.Contains(b))
					continue;
				FoundList.AddUnique(b);
				NextCheckList.AddUnique(b);
			}
		}
		NeighborSteps -= 1;
	}

	return FoundList;
}

void AFireflyGridMapBase::GenerateGridMeshes()
{
	//模型索引，表示棋格是Mesh上第几个分区
	int32 Index = 0;
	//重置
	MeshOfMap->ClearAllMeshSections();
	for (auto Grid : GridsOfMap)
	{
		if (!Grid.Value)
			continue;

		TArray<FVector> Vertices;
		TArray<int32> Indecies;
		TArray<FVector> Normals;
		TArray<FVector2D> UV1, EmptyArray;
		TArray<FColor> VertexColors;
		TArray<FVector> Tangents;
		TArray<FProcMeshTangent> MeshTangents;

		//索引赋值
		Grid.Value->MeshIndex = Index;
		//调用DrawNode，以引用方式回传绘制信息
		Grid.Value->DrawGrid(Vertices, Indecies, Normals, UV1, VertexColors, Tangents, GetActorLocation());

		//用切线向量生成切线结构体
		for (const FVector& Tangent : Tangents)
			MeshTangents.Add(FProcMeshTangent(Tangent, false));

		//生成索引区块模型
		MeshOfMap->CreateMeshSection(Index, Vertices, Indecies, Normals, UV1, EmptyArray
			, EmptyArray, EmptyArray, VertexColors, MeshTangents, bCreateCollision);
		//设置材质
		ResetGridMaterial(Grid.Value);

		Index++;
	}
}

void AFireflyGridMapBase::SetGridMaterial(UFireflyGridBase* InGrid, UMaterialInterface* InMaterial)
{
	if (!InGrid || !InMaterial)
	{
		return;
	}

	MeshOfMap->SetMaterial(InGrid->MeshIndex, InMaterial);
}

void AFireflyGridMapBase::ResetGridMaterial(UFireflyGridBase* InGrid)
{
	if (!InGrid)
		return;

	if (InGrid->ActorsInGrid.Num() > 0 && DebugMaterial1)
	{
		SetGridMaterial(InGrid, DebugMaterial1);
		return;
	}

	//根据通行状态设置材质
	if (InGrid->PassFlag == EGridPassFlag::Pass && PassMaterial)
		SetGridMaterial(InGrid, PassMaterial);
	else if (InGrid->PassFlag == EGridPassFlag::Block && BlockMaterial)
		SetGridMaterial(InGrid, BlockMaterial);
}

void AFireflyGridMapBase::ResetGridMaterialAll()
{
	for (const auto Grid : GridsOfMap)
	{
		if (!Grid.Value)
			continue;

		ResetGridMaterial(Grid.Value);
	}
}

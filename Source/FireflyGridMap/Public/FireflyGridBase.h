// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FireflyGridBase.generated.h"


class AFireflyGridMapBase;


#pragma region GridCoordinate

// 棋格在棋盘中的位置
USTRUCT(BlueprintType)
struct  FFireflyGridCoordinate
{
	GENERATED_USTRUCT_BODY()

public:
	// 棋格在棋盘中的X轴坐标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X = 0;

	// 棋格在棋盘中的Y轴坐标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y = 0;

	FFireflyGridCoordinate() {}

	FFireflyGridCoordinate(int InX, int InY) : X(InX), Y(InY) {}

	// 重载运算符==，满足TMap中Key的规范
	FORCEINLINE friend bool operator==(const FFireflyGridCoordinate& Lhs, const FFireflyGridCoordinate& Rhs)
	{
		return (Lhs.X == Rhs.X) && (Lhs.Y == Rhs.Y);
	}

	// 重载运算符+，满足基本加法运算
	FFireflyGridCoordinate operator+(const FFireflyGridCoordinate& F) const
	{
		return FFireflyGridCoordinate(this->X + F.X, this->Y + F.Y);
	}
};


FORCEINLINE uint32 GetTypeHash(const FFireflyGridCoordinate& Key)
{
	return HashCombine(GetTypeHash(Key.X), GetTypeHash(Key.Y));
}

#pragma endregion


#pragma region Gird_Enum

// 棋格形状
UENUM(BlueprintType)
enum class EGridShape : uint8
{
	None,
	Hexagon,
};

// 棋格通行状态
UENUM(BlueprintType)
enum class EGridPassFlag : uint8
{
	Pass,
	Block,
};

#pragma endregion


UCLASS()
class FIREFLYGRIDMAP_API UFireflyGridBase : public UObject
{
	GENERATED_BODY()

#pragma region Grid

public:
	UFireflyGridBase(const FObjectInitializer& ObjectInitializer);

	// 初始化函数
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual void InitGrid(AFireflyGridMapBase* InGridMap, FVector InLocation, FFireflyGridCoordinate InCoordinate, float InSize);

	// 获取相邻棋格
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual TArray<UFireflyGridBase*> GetNeighbors();

public:
	// 棋格大小尺寸
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid")
	float Size;

	// 棋格的世界空间坐标
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid")
	FVector WorldLocation;

	// 棋格在棋盘中的空间坐标
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid")
	FFireflyGridCoordinate Coordinate;

	// 棋格的形状
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid")
	EGridShape GridShape = EGridShape::None;

	// 所属的棋盘
	UPROPERTY(Transient, BlueprintReadWrite, Category = "FireflyGrid|Grid")
	AFireflyGridMapBase* GridMap;

#pragma endregion


#pragma region Navigation

public:
	// 寻路-判断棋格是否能通行
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual bool CanPassGrid(AActor* InActor) const;

	// 寻路-Actor预定要经过该棋格
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual void ScheduleGrid(AActor* InActor);

	// 寻路-Actor取消要经过该棋格的预定
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual void CancelScheduleGrid(AActor* InActor);

	// 寻路-Actor进入该棋格
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual void EnterGrid(AActor* InActor);

	// 寻路-Actor离开该棋格
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual void LeaveGrid(AActor* InActor);

	// 寻路-获取两棋格的实际间距半径
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Grid")
	virtual float GetRealRadiusSize() const { return Size; };

	//模型-绘制模型
	UFUNCTION()
	virtual void DrawGrid(TArray<FVector>& InVertices,
			TArray<int32>& InIndecies,
			TArray<FVector>& InNormals,
			TArray<FVector2D>& InUV,
			TArray<FColor>& InVertexColors,
			TArray<FVector>& InTangents,
			FVector InOffset) {};
		
public:
	// 寻路-该棋格的寻路通信状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Grid")
	EGridPassFlag PassFlag = EGridPassFlag::Pass;

	// 寻路-棋格中的单位
	UPROPERTY(BlueprintReadWrite, Category = "FireflyGrid|Grid")
	TArray<AActor*> ActorsInGrid;

	// 寻路-寻路缓存的导航路径中该棋格的上一个棋格
	UPROPERTY()
	UFireflyGridBase* PreGrid;

	// 模型-棋格模型索引
	UPROPERTY()
	int MeshIndex;

#pragma endregion
};

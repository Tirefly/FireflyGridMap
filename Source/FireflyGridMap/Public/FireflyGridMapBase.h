// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireflyGridBase.h"
#include "FireflyGridMapBase.generated.h"


UCLASS()
class FIREFLYGRIDMAP_API AFireflyGridMapBase : public AActor
{
	GENERATED_BODY()

#pragma region Actor

public:	
	// Sets default values for this actor's properties
	AFireflyGridMapBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

#pragma endregion


#pragma region GridMapBase

public:
	// 基础-生成棋盘
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Map")
	void GenerateGridMap();

	//基础-获取坐标棋格
	UFUNCTION(BlueprintCallable)
	UFireflyGridBase* GetGridOfMap(FFireflyGridCoordinate InCoordinate) const;

public:
	// 基础-棋盘形状
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Map")
	EGridShape MapShape = EGridShape::None;

	// 基础-棋盘单位大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Map")
	float GridSize;

	// 基础-棋盘行数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Map")
	int32 MapRoll;

	// 基础-棋盘列数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Map")
	int32 MapColumn;

	// 基础-棋盘的棋格
	UPROPERTY()
	TMap<FFireflyGridCoordinate, UFireflyGridBase*> GridsOfMap;

#pragma endregion


#pragma region Generagion

protected:
	// 基础-生成六边形棋盘的棋格
	UFUNCTION()
	void GenerateHexagonGrids(float InHexSize, int InRoll, int InColumn);

public:
	// 基础-生成棋盘棋格的执行入口
	UFUNCTION()
	void GenerateGrids(float InSize, int InRoll, int InColumn);

#pragma endregion


#pragma region Initialization

protected:
	//基础-初始化六边形棋盘的棋格
	UFUNCTION()
	void InitHexagonGridMap();

public://基础-初始化棋盘棋格的入口
	UFUNCTION()
	void InitGridMap();

#pragma endregion


#pragma region GirdCheckBlock

protected:
	//基础-判断是否在六边形棋格内
	UFUNCTION()
	UFireflyGridBase* CheckHitHexagonGridOfMap(const FVector& InPosition);

public:
	//基础-判断是否在棋格内
	UFUNCTION(BlueprintCallable)
	UFireflyGridBase* CheckHitGridOfMap(const FVector& InPosition);

#pragma endregion


#pragma region Navigation

public:
	// 寻路-A*寻找路径
	UFUNCTION(BlueprintCallable)
	bool FindPath(TArray<UFireflyGridBase*>& Path, AActor* InActor, UFireflyGridBase* FromGrid, UFireflyGridBase* ToGrid, int StopSteps = 0);

	// 寻路-是否存在路径
	UFUNCTION(BlueprintCallable)
	bool IsPathExist(AActor* InActor, UFireflyGridBase* FromGrid, UFireflyGridBase* ToGrid, int StopSteps = 0);

	// 寻路-获取目标点范围内的棋格
	UFUNCTION()
	TArray<UFireflyGridBase*> GetGridNeighbors(UFireflyGridBase* InGrid, int InStep = 0) const;

#pragma endregion


#pragma region GridDisplay

public:
	//模型-生成棋盘模型
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Map")
	void GenerateGridMeshes();

	//模型-设置棋格材质
	UFUNCTION(BlueprintCallable)
	void SetGridMaterial(UFireflyGridBase* InGrid, UMaterialInterface* InMaterial);

	//模型-重置棋格材质
	UFUNCTION(BlueprintCallable)
	void ResetGridMaterial(UFireflyGridBase* InGrid);

	//模型-重置所有棋格材质
	UFUNCTION(BlueprintCallable)
	void ResetGridMaterialAll();

public:
	//模型-棋盘模型
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UProceduralMeshComponent* MeshOfMap;

	//模型-通行材质
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* PassMaterial;

	//模型-阻挡材质
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* BlockMaterial;

	//模型-阻挡材质1
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* DebugMaterial1;

	//模型-阻挡材质2
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* DebugMaterial2;

#pragma endregion
};

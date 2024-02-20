// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FireflyGridDebugPlayerController.generated.h"


//Debug模式
UENUM(BlueprintType)
enum class EChessDebugMode : uint8
{
	None,
	DebugPath,
	DebugPawn,
	DebugTaskMove,
	DebugTaskRotate,
};


class AFireflyGridMapBase;


UCLASS()
class FIREFLYGRIDMAP_API AFireflyGridDebugPlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region PlayerController

public:
	//基础-构造函数
	AFireflyGridDebugPlayerController();

#pragma endregion


#pragma region Grid

public:
	//基础-获取棋盘
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Controller")
	AFireflyGridMapBase* GetBattleMap();

	//基础-刷新选中棋格
	UFUNCTION()
	void FlushCheckGrids();

	//基础-获取鼠标对应棋盘的位置
	UFUNCTION()
	FVector GetMouseCursorPosition() const;

	//基础-左键事件
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Controller")
	void HandleLeftClick();

	//基础-右键事件
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Controller")
	void HandleRightClick();

	//基础-Debug寻路
	UFUNCTION()
	void HandleDebugPath();

	//基础-Debug角色
	UFUNCTION()
	void HandleDebugPawn();

	//基础-DebugMoveToTask
	UFUNCTION()
	void HandleDebugTaskMove();

	//基础-DebugTurnToTask
	UFUNCTION()
	void HandleDebugTaskRotate();

public:
	//基础-当前棋盘
	UPROPERTY(BlueprintReadWrite, Category = "FireflyGrid|Controller")
	AFireflyGridMapBase* BattleMap;

	//基础-已选中棋格
	UPROPERTY(BlueprintReadWrite, Category = "FireflyGrid|Controller")
	TArray<class UFireflyGridBase*> SelectedGrids;

	//基础-已选中角色
	UPROPERTY(BlueprintReadWrite, Category = "FireflyGrid|Controller")
	APawn* SelectedPawn;

	//基础-Debug模式
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Controller")
	EChessDebugMode DebugMode = EChessDebugMode::None;

	//当前MoveToTask
	UPROPERTY()
	class UFireflyAITask_MoveToGrid* CurrentMoveTask;

	//当前TurnToTask
	UPROPERTY()
	class UFireflyAITask_TurnTo* CurrentRotateTask;

#pragma endregion
};

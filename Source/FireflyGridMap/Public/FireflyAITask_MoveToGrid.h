// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask.h"
#include "FireflyAITask_MoveToGrid.generated.h"


class AAIController;
class UFireflyGridBase;


UCLASS()
class FIREFLYGRIDMAP_API UFireflyAITask_MoveToGrid : public UAITask
{
	GENERATED_BODY()

#pragma region AITask

public:
	// 创建Task对象-静态
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE", DisplayName = "AI Task Move To Grid"))
	static UFireflyAITask_MoveToGrid* AITask_MoveToGrid(AAIController* Controller, UFireflyGridBase* GoalGrid, AActor* GoalActor, int StopOnStep = 1);

protected:
	// 触发任务函数
	virtual void Activate() override;

#pragma endregion


#pragma region MoveToGrid

protected:
	// 执行移动函数
	UFUNCTION()
	void PerformMove();

	// 成功完成任务函数
	UFUNCTION()
	void RequestSuccess();

	// 失败完成任务函数
	UFUNCTION()
	void RequestFailed();

	// 根据目标角色获取其棋格
	UFireflyGridBase* GetGrid(const AActor* InActor, bool bIncludeScheduleGrid = false) const;

public:
	// 强制开启任务
	void ForceActiveTask();

	// 强制结束任务
	void ForceEndTask();

protected:
	// 委托-成功结束
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Task")
	FGenericGameplayTaskDelegate OnRequestSucceeded;

	// 委托-失败结束
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Task")
	FGenericGameplayTaskDelegate OnRequestFailed;

public:
	// 目标角色
	UPROPERTY()
	AActor* GoalActor;

	// 出发棋格
	UPROPERTY()
	UFireflyGridBase* FromGrid;

	// 终点棋格
	UPROPERTY()
	UFireflyGridBase* ToGrid;

	// 停止距离
	UPROPERTY()
	int StopOnStep;
};

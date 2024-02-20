// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask.h"
#include "FireflyAITask_TurnTo.generated.h"


class UFireflyGridBase;


UCLASS()
class FIREFLYGRIDMAP_API UFireflyAITask_TurnTo : public UAITask
{
	GENERATED_BODY()

#pragma region AITask

public:
	// 创建Task对象-静态
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Controller", BlueprintInternalUseOnly = "TRUE", DisplayName = "AI Task Turn To"))
	static UFireflyAITask_TurnTo* AITask_TurnTo(AAIController* Controller, AActor* GoalActor, UFireflyGridBase* GoalNode, FRotator GoalRotation, float StopOnDegree = 10);

protected:
	// 触发任务函数
	virtual void Activate() override;

#pragma endregion


#pragma region TurnTo

protected:
	// 执行旋转函数
	UFUNCTION()
	void PerformRotate();

	// 成功完成任务函数
	UFUNCTION()
	void RequestSuccess();

	// 失败完成任务函数
	UFUNCTION()
	void RequestFailed();

public:
	// 强制执行
	void ForceActiveTask();

	// 强制失败
	void ForceEndTask();

protected:
	// 委托-成功结束
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Task")
	FGenericGameplayTaskDelegate OnRequestSuccess;

	// 委托-失败结束
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Task")
	FGenericGameplayTaskDelegate OnRequestFailed;

public:
	// 旋转目标角色
	UPROPERTY()
	AActor* GoalActor;

	// 旋转目标棋格
	UPROPERTY()
	UFireflyGridBase* GoalNode;

	// 旋转目标值
	UPROPERTY()
	FRotator GoalRotation;

	// 旋转容差值
	UPROPERTY()
	float StopOnDegree;

#pragma endregion
};

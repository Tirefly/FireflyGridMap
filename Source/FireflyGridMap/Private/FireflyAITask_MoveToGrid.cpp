// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyAITask_MoveToGrid.h"

#include "AIController.h"
#include "FireflyGridBase.h"
#include "FireflyGridMapBase.h"
#include "FireflyGridMovementComponent.h"


UFireflyAITask_MoveToGrid* UFireflyAITask_MoveToGrid::AITask_MoveToGrid(AAIController* Controller,
	UFireflyGridBase* GoalGrid, AActor* GoalActor, int StopOnStep)
{
	if (!Controller)
		return nullptr;
	//创建Task对象
	UFireflyAITask_MoveToGrid* MyTask = UAITask::NewAITask<UFireflyAITask_MoveToGrid>(*Controller, EAITaskPriority::High);
	//设置基本属性
	MyTask->ToGrid = GoalGrid;
	MyTask->GoalActor = GoalActor;
	MyTask->StopOnStep = StopOnStep;

	return MyTask;
}

void UFireflyAITask_MoveToGrid::Activate()
{
	Super::Activate();

	PerformMove();
}

void UFireflyAITask_MoveToGrid::PerformMove()
{
	//安全判断
	if (!OwnerController || !OwnerController->GetPawn())
	{
		RequestFailed();
		return;
	}

	//获取当前角色和移动组件
	APawn* OwnerPawn = OwnerController->GetPawn();
	UFireflyGridMovementComponent* GridMovement = OwnerPawn->FindComponentByClass<UFireflyGridMovementComponent>();
	if (!GridMovement)
	{
		RequestFailed();
		return;
	}

	//获取当前起点和终点
	if (GoalActor)
	{
		FromGrid = GetGrid(OwnerPawn);
		ToGrid = GetGrid(GoalActor, true);
	}
	else
	{
		FromGrid = GetGrid(OwnerPawn);
	}

	//判断起点终点状态
	if (!FromGrid || !ToGrid)
	{
		RequestFailed();
		return;
	}
	if (FromGrid == ToGrid)
	{
		RequestSuccess();
		return;
	}

	//进行寻路
	TArray<UFireflyGridBase*> Path;
	const bool tResult = FromGrid->GridMap->FindPath(Path, OwnerPawn, FromGrid, ToGrid, StopOnStep);
	//回传False，直接失败
	if (!tResult)
	{
		RequestFailed();
		return;
	}

	//回传True，并路径点数量为0，则为成功完成移动任务
	if (Path.Num() == 0)
	{
		RequestSuccess();
		return;
	}

	//设置路径
	GridMovement->SetMovementPath(Path);

	//绑定委托，在每次到达中间路点后，重新执行任务
	if (!GridMovement->OnMoveCheckPoint.IsAlreadyBound(this, &UFireflyAITask_MoveToGrid::PerformMove))
		GridMovement->OnMoveCheckPoint.AddDynamic(this, &UFireflyAITask_MoveToGrid::PerformMove);

	//绑定委托，到达终点后，完成任务
	if (!GridMovement->OnMoveComplete.IsAlreadyBound(this, &UFireflyAITask_MoveToGrid::RequestSuccess))
		GridMovement->OnMoveComplete.AddDynamic(this, &UFireflyAITask_MoveToGrid::RequestSuccess);
}

void UFireflyAITask_MoveToGrid::RequestSuccess()
{
	if (!OwnerController || !OwnerController->GetPawn())
		return;

	//移除委托
	UFireflyGridMovementComponent* GridMovement = OwnerController->GetPawn()->FindComponentByClass<UFireflyGridMovementComponent>();
	if (GridMovement && GridMovement->OnMoveCheckPoint.IsAlreadyBound(this, &UFireflyAITask_MoveToGrid::PerformMove))
		GridMovement->OnMoveCheckPoint.RemoveDynamic(this, &UFireflyAITask_MoveToGrid::PerformMove);
	if (GridMovement && GridMovement->OnMoveComplete.IsAlreadyBound(this, &UFireflyAITask_MoveToGrid::RequestSuccess))
		GridMovement->OnMoveComplete.RemoveDynamic(this, &UFireflyAITask_MoveToGrid::RequestSuccess);

	GridMovement->StopMovementImmediately();
	OnRequestSucceeded.Broadcast();
	EndTask();
}

void UFireflyAITask_MoveToGrid::RequestFailed()
{
	if (!OwnerController || !OwnerController->GetPawn())
		return;

	//移除委托
	UFireflyGridMovementComponent* GridMovement = OwnerController->GetPawn()->FindComponentByClass<UFireflyGridMovementComponent>();
	if (GridMovement && GridMovement->OnMoveCheckPoint.IsAlreadyBound(this, &UFireflyAITask_MoveToGrid::PerformMove))
		GridMovement->OnMoveCheckPoint.RemoveDynamic(this, &UFireflyAITask_MoveToGrid::PerformMove);
	if (GridMovement && GridMovement->OnMoveComplete.IsAlreadyBound(this, &UFireflyAITask_MoveToGrid::RequestSuccess))
		GridMovement->OnMoveComplete.RemoveDynamic(this, &UFireflyAITask_MoveToGrid::RequestSuccess);

	GridMovement->StopMovementImmediately();
	OnRequestFailed.Broadcast();
	EndTask();
}

UFireflyGridBase* UFireflyAITask_MoveToGrid::GetGrid(const AActor* InActor, bool bIncludeScheduleGrid) const
{
	if (!InActor)
		return nullptr;

	//获取移动组件的NowNode
	const UFireflyGridMovementComponent* GridMovement = InActor->FindComponentByClass<UFireflyGridMovementComponent>();
	if (!GridMovement)
		return nullptr;

	if (bIncludeScheduleGrid && GridMovement->ScheduledGrid)
		return GridMovement->ScheduledGrid;
	
	return GridMovement->CurrentGrid;
}

void UFireflyAITask_MoveToGrid::ForceActiveTask()
{
	Activate();
}

void UFireflyAITask_MoveToGrid::ForceEndTask()
{
	RequestSuccess();
}

// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyAITask_TurnTo.h"

#include "AIController.h"
#include "FireflyGridMovementComponent.h"


UFireflyAITask_TurnTo* UFireflyAITask_TurnTo::AITask_TurnTo(AAIController* Controller, AActor* GoalActor,
	UFireflyGridBase* GoalNode, FRotator GoalRotation, float StopOnDegree)
{
	if (!Controller)
		return nullptr;

	//创建Task对象
	UFireflyAITask_TurnTo* MyTask = UAITask::NewAITask<UFireflyAITask_TurnTo>(*Controller, EAITaskPriority::High);
	//设置基本属性
	MyTask->GoalActor = GoalActor;
	MyTask->GoalNode = GoalNode;
	MyTask->GoalRotation = GoalRotation;
	MyTask->StopOnDegree = StopOnDegree;

	return MyTask;
}

void UFireflyAITask_TurnTo::Activate()
{
	Super::Activate();

	PerformRotate();
}

void UFireflyAITask_TurnTo::PerformRotate()
{
	//安全判断
	if (!OwnerController || !OwnerController->GetPawn())
	{
		RequestFailed();
		return;
	}

	//获取当前角色和移动组件
	const APawn* OwnerPawn = OwnerController->GetPawn();
	UFireflyGridMovementComponent* ChessMovementComponent = OwnerPawn->FindComponentByClass<UFireflyGridMovementComponent>();
	if (!ChessMovementComponent)
	{
		RequestFailed();
		return;
	}

	//设定旋转目标
	if (GoalActor)
		ChessMovementComponent->RotatingTarget = FFireflyGridRotatingTarget(GoalActor, StopOnDegree);
	else if (GoalNode)
		ChessMovementComponent->RotatingTarget = FFireflyGridRotatingTarget(GoalNode, StopOnDegree);
	else
		ChessMovementComponent->RotatingTarget = FFireflyGridRotatingTarget(GoalRotation, StopOnDegree);

	//绑定委托，旋转完成后，完成任务
	if (!ChessMovementComponent->OnRotateComplete.IsAlreadyBound(this, &UFireflyAITask_TurnTo::RequestSuccess))
		ChessMovementComponent->OnRotateComplete.AddDynamic(this, &UFireflyAITask_TurnTo::RequestSuccess);
}

void UFireflyAITask_TurnTo::RequestSuccess()
{
	if (!OwnerController || !OwnerController->GetPawn())
		return;

	//移除委托
	UFireflyGridMovementComponent* ChessMovementComponent = OwnerController->GetPawn()->FindComponentByClass<UFireflyGridMovementComponent>();
	if (ChessMovementComponent && ChessMovementComponent->OnRotateComplete.IsAlreadyBound(this, &UFireflyAITask_TurnTo::RequestSuccess))
		ChessMovementComponent->OnRotateComplete.RemoveDynamic(this, &UFireflyAITask_TurnTo::RequestSuccess);

	OnRequestSuccess.Broadcast();
	EndTask();
}

void UFireflyAITask_TurnTo::RequestFailed()
{
	if (!OwnerController || !OwnerController->GetPawn())
		return;

	//移除委托
	UFireflyGridMovementComponent* ChessMovementComponent = OwnerController->GetPawn()->FindComponentByClass<UFireflyGridMovementComponent>();
	if (ChessMovementComponent && ChessMovementComponent->OnRotateComplete.IsAlreadyBound(this, &UFireflyAITask_TurnTo::RequestSuccess))
		ChessMovementComponent->OnRotateComplete.RemoveDynamic(this, &UFireflyAITask_TurnTo::RequestSuccess);

	OnRequestFailed.Broadcast();
	EndTask();
}

void UFireflyAITask_TurnTo::ForceActiveTask()
{
	Activate();
}

void UFireflyAITask_TurnTo::ForceEndTask()
{
	RequestSuccess();
}

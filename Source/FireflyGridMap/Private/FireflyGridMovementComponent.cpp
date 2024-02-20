// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyGridMovementComponent.h"


void UFireflyGridMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//安全判断
	if (!UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	AActor* ActorOwner = UpdatedComponent->GetOwner();
	if (!ActorOwner || !IsValid(ActorOwner))
	{
		return;
	}
	if (UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	//执行旋转
	if (RotatingTarget.IsValid())
	{
		FHitResult RotHit(1.f);
		FRotator tFromRotation = UpdatedComponent->GetComponentRotation();
		FRotator tToRotation = RotatingTarget.GetRotation(UpdatedComponent->GetComponentLocation());
		tFromRotation.Pitch = 0; tFromRotation.Roll = 0;
		tToRotation.Pitch = 0; tToRotation.Roll = 0;
		if (FMath::Abs(tFromRotation.Yaw - tToRotation.Yaw) <= FMath::Max(RotatingTarget.StopOnDegree, 0.1f))
		{
			RotatingTarget.Reset();
			OnRotateComplete.Broadcast();
		}
		else
		{
			FRotator tNewRotation = FMath::RInterpConstantTo(tFromRotation, tToRotation, DeltaTime, MaxRotationRate);
			MoveUpdatedComponent(FVector::ZeroVector, tNewRotation, false, &RotHit, ETeleportType::None);
		}
		return;
	}

	//安全判断是否有路径
	if (MovementStack.Num() == 0)
	{
		return;
	}

	//拿出顶部，安全判断
	UFireflyGridBase* tNode = MovementStack.Top();
	if (!tNode)
	{
		MovementStack.Pop();
		return;
	}

	//如果要移动的棋格不是当前棋格，则预定这个棋格
	if (CurrentGrid != tNode && ScheduledGrid != tNode)
	{
		SetScheduledGrid(tNode);
	}

	//设置当前Tick移动参数
	FHitResult MoveHit(1.f);
	float tSpeed = FMath::Max(0.0f, MaxMoveSpeed);
	FRotator tRotation = UpdatedComponent->GetComponentRotation();
	FVector tDirection = tNode->WorldLocation - ActorOwner->GetActorLocation();
	float tDistance = tDirection.Size2D();
	tDirection.Normalize();
	FVector tMoveDelta;

	//设置MoveDelta
	if (tDistance < DeltaTime * tSpeed)
	{
		tMoveDelta = tNode->WorldLocation - ActorOwner->GetActorLocation();
	}
	else
	{
		tMoveDelta = tDirection * tSpeed * DeltaTime;
	}

	//设置RotDelta
	FRotator tRotDelta = FMath::RInterpConstantTo(tRotation, tMoveDelta.Rotation(), DeltaTime, MaxRotationRate);
	//设置当前速度
	Velocity = tMoveDelta / DeltaTime;
	//移动组件
	MoveUpdatedComponent(tMoveDelta, tRotDelta, false, &MoveHit, ETeleportType::None);

	//计算当前所在棋格
	CalculateCurrentGrid();
	//到达中间路点
	if ((ActorOwner->GetActorLocation() - tNode->WorldLocation).IsNearlyZero())
	{
		//弹出顶栈
		MovementStack.Pop();
		if (MovementStack.Num() == 0)
		{
			Velocity = FVector::ZeroVector;
			OnMoveComplete.Broadcast();
			return;
		}

		OnMoveCheckPoint.Broadcast();
	}
}

void UFireflyGridMovementComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (ScheduledGrid)
		ScheduledGrid->CancelScheduleGrid(GetOwner());

	if (CurrentGrid)
		CurrentGrid->LeaveGrid(GetOwner());

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UFireflyGridMovementComponent::StopMovementImmediately()
{
	if (ScheduledGrid)
		ScheduledGrid->CancelScheduleGrid(GetOwner());

	MovementStack.Empty();
	RotatingTarget.Reset();

	Super::StopMovementImmediately();
}

void UFireflyGridMovementComponent::SetCurrentGrid(UFireflyGridBase* InGrid)
{
	//取消当前预定棋格
	if (ScheduledGrid)
		SetScheduledGrid(nullptr);

	//离开当前棋格
	if (CurrentGrid)
		CurrentGrid->LeaveGrid(GetOwner());

	//设置当前棋格
	CurrentGrid = InGrid;
	//进行进入
	if (CurrentGrid)
		CurrentGrid->EnterGrid(GetOwner());
}

void UFireflyGridMovementComponent::SetScheduledGrid(UFireflyGridBase* InGrid)
{
	//取消当前预定棋格
	if (ScheduledGrid)
		ScheduledGrid->CancelScheduleGrid(GetOwner());
	//设置当前预定棋格
	ScheduledGrid = InGrid;
	//进行预定
	if (ScheduledGrid)
		ScheduledGrid->ScheduleGrid(GetOwner());
}

void UFireflyGridMovementComponent::CalculateCurrentGrid()
{
	//安全判断
	const AActor* ActorOwner = UpdatedComponent->GetOwner();
	if (!ActorOwner || (!IsValid(ActorOwner)))
		return;

	if (MovementStack.Num() == 0)
		return;

	UFireflyGridBase* tNode = MovementStack.Top();
	if (!tNode)
		return;

	//获取角色位置到当前和下一个棋格的位置，如果举一例位置小于实际半径的距离，则算进入了棋格
	if (FVector::Dist2D(ActorOwner->GetActorLocation(), tNode->WorldLocation) <= tNode->GetRealRadiusSize())
	{
		SetCurrentGrid(tNode);
	}
}

void UFireflyGridMovementComponent::SetMovementPath(TArray<UFireflyGridBase*> InPath, bool bNeedReverse)
{
	if (bNeedReverse)
	{
		Algo::Reverse(InPath);
	}

	MovementStack = InPath;
	//开始移动，广播开始移动委托
	if (MovementStack.Num() > 0)
	{
		SetScheduledGrid(MovementStack.Top());
		OnMoveBegin.Broadcast();
	}
}

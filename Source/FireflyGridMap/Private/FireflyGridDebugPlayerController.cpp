// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyGridDebugPlayerController.h"

#include "EngineUtils.h"
#include "FireflyGridMapBase.h"
#include "FireflyGridMovementComponent.h"
#include "AIController.h"
#include "FireflyAITask_MoveToGrid.h"
#include "FireflyAITask_TurnTo.h"


AFireflyGridDebugPlayerController::AFireflyGridDebugPlayerController()
{
	bShowMouseCursor = true;
}

AFireflyGridMapBase* AFireflyGridDebugPlayerController::GetBattleMap()
{
	if (BattleMap)
		return BattleMap;

	//使用迭代器遍历
	for (TActorIterator<AFireflyGridMapBase> It(GetWorld()); It; ++It)
	{
		if (!*It)
			continue;

		BattleMap = *It;
		break;
	}

	return BattleMap;
}

void AFireflyGridDebugPlayerController::FlushCheckGrids()
{
	if (!GetBattleMap())
		return;

	switch (DebugMode)
	{
		case EChessDebugMode::DebugPath:
		{
			//超过2个选中棋格就重置材质
			if (SelectedGrids.Num() >= 2)
			{
				SelectedGrids.Reset();
				BattleMap->ResetGridMaterialAll();
			}
			break;
		}
		case EChessDebugMode::DebugPawn:
		case EChessDebugMode::DebugTaskMove:
		case EChessDebugMode::DebugTaskRotate:
		{
			//超过1个选中棋格就重置材质
			if (SelectedGrids.Num() >= 1)
			{
				SelectedGrids.Reset();
				BattleMap->ResetGridMaterialAll();
			}
			break;
		}
		default:
			BattleMap->ResetGridMaterialAll();
			break;
	}
}

FVector AFireflyGridDebugPlayerController::GetMouseCursorPosition() const
{
	if (!BattleMap)
		return FVector::ZeroVector;

	FVector WorldLocation, WorldDirection;
	//屏幕坐标转世界坐标，回传坐标和方向
	if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		//获取Z轴差值
		float tScale = BattleMap->GetActorLocation().Z - WorldLocation.Z;
		tScale /= WorldDirection.Z;
		WorldLocation += tScale * WorldDirection;
		return WorldLocation;
	}

	return FVector::ZeroVector;
}

void AFireflyGridDebugPlayerController::HandleLeftClick()
{
	switch (DebugMode)
	{
		case EChessDebugMode::None:
			break;
		case EChessDebugMode::DebugPath:
			HandleDebugPath();
			break;
		case EChessDebugMode::DebugPawn:
			HandleDebugPawn();
			break;
		case EChessDebugMode::DebugTaskMove:
			HandleDebugTaskMove();
			break;
		case EChessDebugMode::DebugTaskRotate:
			HandleDebugTaskRotate();
			break;
		default:
			break;
	}
}

void AFireflyGridDebugPlayerController::HandleRightClick()
{
	if (!GetBattleMap())
		return;

	FlushCheckGrids();
	//获取选中棋格
	if (UFireflyGridBase* tHitGrid = BattleMap->CheckHitGridOfMap(GetMouseCursorPosition()))
	{
		//通行状态切换
		tHitGrid->PassFlag = tHitGrid->PassFlag == EGridPassFlag::Pass ? EGridPassFlag::Block : EGridPassFlag::Pass;
		BattleMap->ResetGridMaterial(tHitGrid);
	}
}

void AFireflyGridDebugPlayerController::HandleDebugPath()
{
	if (!GetBattleMap())
		return;

	FlushCheckGrids();
	//获取选中棋格
	UFireflyGridBase* tHitGrid = BattleMap->CheckHitGridOfMap(GetMouseCursorPosition());
	//判断棋格通行状态
	if (tHitGrid && tHitGrid->PassFlag != EGridPassFlag::Block)
	{
		SelectedGrids.AddUnique(tHitGrid);
		BattleMap->SetGridMaterial(tHitGrid, BattleMap->DebugMaterial1);
	}

	//如果选中棋格超过2个，则进行寻路
	if (SelectedGrids.Num() >= 2)
	{
		TArray<UFireflyGridBase*> tPath;

		if (!BattleMap->FindPath(tPath, nullptr, SelectedGrids[0], SelectedGrids[1]))
			return;

		for (const auto Grid : tPath)
		{
			//跳过终点材质设置
			if (Grid == SelectedGrids[1])
				continue;

			//设置路径材质
			BattleMap->SetGridMaterial(Grid, BattleMap->DebugMaterial2);
		}
	}
}

void AFireflyGridDebugPlayerController::HandleDebugPawn()
{
	FVector WorldLocation, WorldDirection;
	//获取鼠标点击的角色
	if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		//射线检测
		FVector Start, End;
		Start = WorldLocation;
		End = Start + WorldDirection * 2000;
		FCollisionQueryParams Params;
		FCollisionObjectQueryParams ObjectParams;

		//碰撞类型选择pawn
		ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		FHitResult OutHit;
		if (UWorld* World = GetWorld())
		{
			if (World->LineTraceSingleByObjectType(OutHit, Start, End, ObjectParams, Params))
			{
				//设定当前选中角色
				SelectedPawn = Cast<APawn>(OutHit.GetActor());
				return;
			}
		}
	}

	if (SelectedPawn && GetBattleMap())
	{
		FlushCheckGrids();
		//获取移动组件
		UFireflyGridMovementComponent* ChessMovementComponent = SelectedPawn->FindComponentByClass<UFireflyGridMovementComponent>();
		if (!ChessMovementComponent || !ChessMovementComponent->CurrentGrid)
			return;

		//获取起点，终点
		UFireflyGridBase* FromGrid = ChessMovementComponent->CurrentGrid;
		UFireflyGridBase* ToGrid = BattleMap->CheckHitGridOfMap(GetMouseCursorPosition());
		if (!FromGrid || !ToGrid)
			return;
		if (ToGrid->PassFlag == EGridPassFlag::Block)
			return;

		SelectedGrids.AddUnique(ToGrid);
		//设置材质
		BattleMap->SetGridMaterial(ToGrid, BattleMap->DebugMaterial1);
		TArray<UFireflyGridBase*> tPath;

		//进行寻路
		if (!BattleMap->FindPath(tPath, SelectedPawn, FromGrid, ToGrid))
			return;

		//进行
		ChessMovementComponent->SetMovementPath(tPath);
	}
}

void AFireflyGridDebugPlayerController::HandleDebugTaskMove()
{
	FVector WorldLocation, WorldDirection;
	//获取鼠标点击的角色
	if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		//射线检测
		FVector Start, End;
		Start = WorldLocation;
		End = Start + WorldDirection * 2000;
		FCollisionQueryParams Params;
		FCollisionObjectQueryParams ObjectParams;

		//碰撞类型选择pawn
		ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		FHitResult OutHit;
		if (UWorld* World = GetWorld())
		{
			if (World->LineTraceSingleByObjectType(OutHit, Start, End, ObjectParams, Params))
			{
				SelectedPawn = Cast<APawn>(OutHit.GetActor());
				return;
			}
		}
	}

	if (SelectedPawn && GetBattleMap())
	{
		FlushCheckGrids();

		//获取终点
		UFireflyGridBase* ToGrid = BattleMap->CheckHitGridOfMap(GetMouseCursorPosition());
		if (!ToGrid)
			return;

		//获取AIController
		AAIController* AIController = Cast<AAIController>(SelectedPawn->GetController());
		if (!AIController)
			return;
		if (ToGrid->PassFlag == EGridPassFlag::Block)
			return;

		SelectedGrids.AddUnique(ToGrid);
		BattleMap->SetGridMaterial(ToGrid, BattleMap->DebugMaterial1);
		if (CurrentMoveTask)
		{
			//强制取消上一个Task
			CurrentMoveTask->ForceEndTask();
		}

		//创建Task
		CurrentMoveTask = UFireflyAITask_MoveToGrid::AITask_MoveToGrid(AIController, ToGrid, nullptr, 0);
		//执行Task
		CurrentMoveTask->ForceActiveTask();
	}
}

void AFireflyGridDebugPlayerController::HandleDebugTaskRotate()
{
	//获取鼠标点击的角色
	FVector WorldLocation, WorldDirection;
	if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		//射线检测
		FVector Start, End;
		Start = WorldLocation;
		End = Start + WorldDirection * 2000;
		FCollisionQueryParams Params;
		FCollisionObjectQueryParams ObjectParams;

		//碰撞类型选择pawn
		ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		FHitResult OutHit;
		if (UWorld* World = GetWorld())
		{
			if (World->LineTraceSingleByObjectType(OutHit, Start, End, ObjectParams, Params))
			{
				SelectedPawn = Cast<APawn>(OutHit.GetActor());
				return;
			}
		}
	}

	if (SelectedPawn && GetBattleMap())
	{
		FlushCheckGrids();

		//获取目标点
		UFireflyGridBase* ToGrid = BattleMap->CheckHitGridOfMap(GetMouseCursorPosition());
		if (!ToGrid)
			return;

		//获取AIController
		AAIController* AIController = Cast<AAIController>(SelectedPawn->GetController());
		if (!AIController)
			return;
		if (ToGrid->PassFlag == EGridPassFlag::Block)
			return;

		SelectedGrids.AddUnique(ToGrid);
		BattleMap->SetGridMaterial(ToGrid, BattleMap->DebugMaterial1);
		if (CurrentRotateTask)
		{
			//强制取消上一个Task
			CurrentRotateTask->ForceEndTask();
		}
		//创建Task
		CurrentRotateTask = UFireflyAITask_TurnTo::AITask_TurnTo(AIController, nullptr, ToGrid, FRotator::ZeroRotator, 0);
		//执行Task
		CurrentRotateTask->ForceActiveTask();
	}
}

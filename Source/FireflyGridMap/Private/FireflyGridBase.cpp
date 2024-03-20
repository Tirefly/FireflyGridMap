// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyGridBase.h"

#include "FireflyGridMapBase.h"


UFireflyGridBase::UFireflyGridBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFireflyGridBase::InitGrid(AFireflyGridMapBase* InGridMap, const FTransform& InTransform,
	FFireflyGridCoordinate InCoordinate, float InSize)
{
	GridMap = InGridMap;
	WorldTransform = InTransform;
	Coordinate = InCoordinate;
	Size = InSize;
}

TArray<UFireflyGridBase*> UFireflyGridBase::GetNeighbors()
{
	return TArray<UFireflyGridBase*>();
}

bool UFireflyGridBase::CanPassGrid(AActor* InActor) const
{
	if (PassFlag >= EGridPassFlag::Block)
	{
		return false;
	}
		
	//判断是否有其他棋子站在当前棋格上
	if (ActorsInGrid.Num() > 0 && InActor)
	{
		return ActorsInGrid.Contains(InActor) ? true : false;
	}

	return true;
}

bool UFireflyGridBase::IsGridVacant() const
{
	// if (PassFlag >= EGridPassFlag::Block)
	// {
	// 	return false;
	// }
	return ActorsInGrid.Num() <= 0;
}

void UFireflyGridBase::ScheduleGrid(AActor* InActor)
{
	if (InActor)
	{
		ActorsInGrid.AddUnique(InActor);
		//GridMap->ResetGridMaterial(this);
	}
}

void UFireflyGridBase::CancelScheduleGrid(AActor* InActor)
{
	if (InActor && ActorsInGrid.Contains(InActor))
	{
		ActorsInGrid.Remove(InActor);
		//GridMap->ResetGridMaterial(this);
	}
}

void UFireflyGridBase::EnterGrid(AActor* InActor)
{
	if (InActor)
	{
		ActorsInGrid.AddUnique(InActor);
		//GridMap->ResetGridMaterial(this);
	}
}

void UFireflyGridBase::LeaveGrid(AActor* InActor)
{
	if (InActor && ActorsInGrid.Contains(InActor))
	{
		ActorsInGrid.Remove(InActor);
		//GridMap->ResetGridMaterial(this);
	}
}

UFireflyGridBase* UFireflyGridBase::GetNearestVacantGrid()
{
	if (!IsValid(GridMap))
    {
    	return nullptr;
    }
    
    TArray<UFireflyGridBase*> Queue;
    TSet<UFireflyGridBase*> Visited;
	
    Queue.Add(this);
	Visited.Add(this);
	
    while (!Queue.IsEmpty())
    {
    	UFireflyGridBase* Current = Queue[0];
    	Queue.RemoveAt(0);
    	
    	if (Current->IsGridVacant())
    		return Current;
    	
    	for (UFireflyGridBase* Neighbor : Current->GetNeighbors())
    	{
    		if (!Visited.Contains(Neighbor))
    		{
    			Visited.Add(Neighbor);
    			Queue.Add(Neighbor);
    		}
    	}
    }

	return nullptr;
}

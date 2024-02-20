// Copyright LazyGameStudio, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "FireflyGridBase.h"
#include "FireflyGridMovementComponent.generated.h"


USTRUCT(BlueprintType)
struct  FFireflyGridRotatingTarget
{
public:
	GENERATED_USTRUCT_BODY()

	//是否激活
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActive = false;

	//旋转目标角色
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor = nullptr;

	//旋转目标棋格
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFireflyGridBase* TargetGrid = nullptr;

	//旋转目标值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetRotation;

	//误差角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StopOnDegree = 0;

	FFireflyGridRotatingTarget() {};

	//构建函数-传入角度值
	FFireflyGridRotatingTarget(AActor* InActor, float InStopOnDegree)
	{
		this->bActive = true;
		this->TargetActor = InActor;
		this->TargetGrid = nullptr;
		this->TargetRotation = FRotator::ZeroRotator;
		this->StopOnDegree = InStopOnDegree;
	}

	//构建函数-传入棋格
	FFireflyGridRotatingTarget(UFireflyGridBase* InGrid, float InStopOnDegree)
	{
		this->bActive = true;
		this->TargetActor = nullptr;
		this->TargetGrid = InGrid;
		this->TargetRotation = FRotator::ZeroRotator;
		this->StopOnDegree = InStopOnDegree;
	}

	//构建函数-传入角色
	FFireflyGridRotatingTarget(const FRotator& InRotator, float InStopOnDegree)
	{
		this->bActive = true;
		this->TargetActor = nullptr;
		this->TargetGrid = nullptr;
		this->TargetRotation = InRotator;
		this->StopOnDegree = InStopOnDegree;
	}

	//当前是否激活
	bool IsValid() const
	{
		return bActive;
	}

	//获取目标Rotation
	FRotator GetRotation(const FVector& InLocation) const
	{
		if (TargetActor)
		{
			return FRotationMatrix::MakeFromX(TargetActor->GetActorLocation() - InLocation).Rotator();
		}
		else if (TargetGrid)
		{
			return FRotationMatrix::MakeFromX(TargetGrid->WorldLocation - InLocation).Rotator();
		}
		else
		{
			return TargetRotation;
		}
	}

	//重置
	void Reset()
	{
		bActive = false;
		TargetActor = nullptr;
		TargetGrid = nullptr;
		TargetRotation = FRotator::ZeroRotator;
		StopOnDegree = 0;
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireflyGridMovementDelegate);


UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = Velocity)
class FIREFLYGRIDMAP_API UFireflyGridMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

#pragma region MovementComponent

public:
	// Tick函数
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 处理组件销毁
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	// 立刻停止移动
	virtual void StopMovementImmediately() override;

#pragma endregion


#pragma region GridNavigation

public:
	// 设置当前棋格
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Movement")
	void SetCurrentGrid(UFireflyGridBase* InGrid);

	// 设置预定棋格
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Movement")
	void SetScheduledGrid(UFireflyGridBase* InGrid);

	// 计算当前棋格
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Movement")
	void CalculateCurrentGrid();

	// 设置移动路径
	UFUNCTION(BlueprintCallable, Category = "FireflyGrid|Movement")
	void SetMovementPath(TArray<UFireflyGridBase*> InPath, bool bNeedReverse = true);

public:
	// 当前棋格
	UPROPERTY(BlueprintReadOnly, Category = "FireflyGrid|Movement")
	UFireflyGridBase* CurrentGrid;

	// 预定棋格
	UPROPERTY(BlueprintReadOnly, Category = "FireflyGrid|Movement")
	UFireflyGridBase* ScheduledGrid;

	// 移动棋格栈
	UPROPERTY(BlueprintReadWrite, Category = "FireflyGrid|Movement")
	TArray<UFireflyGridBase*> MovementStack;

	// 旋转目标
	UPROPERTY(BlueprintReadWrite, Category = "FireflyGrid|Movement")
	FFireflyGridRotatingTarget RotatingTarget;

	// 移动速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Movement")
	float MaxMoveSpeed = 300.f;

	// 旋转速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireflyGrid|Movement")
	float MaxRotationRate = 360.f;

#pragma endregion


#pragma region MovementDelegate

public:
	// 委托-开始移动时触发
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Movement")
	FFireflyGridMovementDelegate OnMoveBegin;

	// 委托-到达中间路点时触发
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Movement")
	FFireflyGridMovementDelegate OnMoveCheckPoint;

	// 委托-到达终点时触发
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Movement")
	FFireflyGridMovementDelegate OnMoveComplete;

	// 委托-完成旋转
	UPROPERTY(BlueprintAssignable, Category = "FireflyGrid|Movement")
	FFireflyGridMovementDelegate OnRotateComplete;

#pragma endregion
};

// Copyright LazyGameStudio, Inc. All Rights Reserved.


#include "FireflyGridMapLibrary.h"

#include "FireflyGridMovementComponent.h"

UFireflyGridBase* UFireflyGridMapLibrary::GetNearestVacantGridOfMap(const UObject* WorldContext, AActor* Actor)
{
	if (!IsValid(Actor) || !IsValid(WorldContext))
	{
		return nullptr;
	}

	const UFireflyGridMovementComponent* GridMovement = Actor->FindComponentByClass<UFireflyGridMovementComponent>();
	if (!IsValid(GridMovement))
	{
		return nullptr;
	}

	UFireflyGridBase* CurrentGrid = Cast<UFireflyGridBase>(GridMovement->GetCurrentGrid());
	if (!IsValid(CurrentGrid))
	{
		return nullptr;
	}

	return CurrentGrid->GetNearestVacantGrid();
}

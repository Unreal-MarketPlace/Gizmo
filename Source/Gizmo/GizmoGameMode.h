// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GizmoGameMode.generated.h"

UCLASS(minimalapi)
class AGizmoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGizmoGameMode();

	void StopReplicationWithDelay(AActor* ReplicatedActor, float Delay, ACharacter* InstigatorClient);

private:

	UPROPERTY()
	TArray<AActor*> ReplicatedActors;
	int32 ReplicateActorIndex = 0;

	FTimerHandle StopReplication_Timer;
	FTimerDelegate StopReplication_Delegate;

	UFUNCTION()
	void StopActorReplication(AActor* ReplicatedActor);
};




// Copyright Epic Games, Inc. All Rights Reserved.

#include "GizmoGameMode.h"
#include "Player/GizmoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Gizmo/Player/GizmoCharacter.h"

AGizmoGameMode::AGizmoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GizmoPlayer/BP_GzimoCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGizmoGameMode::StopReplicationWithDelay(AActor* ReplicatedActor, float Delay, ACharacter* InstigatorClient)
{
	if(!ReplicatedActor) return;

	if(Delay <= 0.f) StopActorReplication(ReplicatedActor);

	ReplicatedActor->SetReplicates(true);

	ReplicatedActors.Add(ReplicatedActor);

	StopReplication_Delegate.BindUFunction(InstigatorClient, "StopActorReplication", ReplicatedActors[ReplicateActorIndex]);
	GetWorldTimerManager().SetTimer(StopReplication_Timer, StopReplication_Delegate, Delay, false);

	ReplicateActorIndex++;
}

void AGizmoGameMode::StopActorReplication(AActor* ReplicatedActor)
{
	if (!ReplicatedActor)
	{
		UE_LOG(LogTemp, Error, TEXT("AGizmoGameMode::StopActorReplication ReplicatedActor = NULL"));
		return;
	}

	ReplicatedActor->SetReplicates(false);
	ReplicatedActors.Remove(ReplicatedActor);
	ReplicateActorIndex--;
	UE_LOG(LogTemp, Error, TEXT("AGizmoGameMode::StopActorReplication ReplicatedActor = %s"), *ReplicatedActor->GetName());
}

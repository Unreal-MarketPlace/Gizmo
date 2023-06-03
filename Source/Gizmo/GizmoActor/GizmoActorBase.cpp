// Fill out your copyright notice in the Description page of Project Settings.


#include "Gizmo/GizmoActor/GizmoActorBase.h"
#include "Gizmo/Library/GizmoMathLibrary.h"

AGizmoActorBase::AGizmoActorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	SetMobility(EComponentMobility::Movable);
}

void AGizmoActorBase::TrackGizmoActor_Implementation(AActor* GActor)
{
	if(!GActor) return;

	TrackerActor = GActor;

	MakeTranslucent(true);

	SetActorCollisionResponse(ECR_Ignore);

	GetWorld()->GetTimerManager().ClearTimer(GizmoTrace_Timer);
	GetWorld()->GetTimerManager().SetTimer(GizmoTrace_Timer, this, &AGizmoActorBase::TrackActor, 0.5, true);

}

TArray<UStaticMeshComponent*> AGizmoActorBase::GetMeshes_Implementation()
{
	TArray<UStaticMeshComponent*> OutMeshes;
	OutMeshes.Add(GetStaticMeshComponent());
	return OutMeshes;
}

void AGizmoActorBase::SetActorCollisionResponse(ECollisionResponse EResponse)
{
	for (UStaticMeshComponent* sm : GetMeshes())
	{
		sm->SetCollisionResponseToChannel(ECC_Visibility, EResponse);
		sm->SetCollisionResponseToChannel(ECC_GameTraceChannel1, EResponse);
	}
}

void AGizmoActorBase::BeginPlay()
{
	Super::BeginPlay();

	SetDynamicMaterials(GetMeshes(), OUT DyanicMaterials);

	FVector Center = FVector::ZeroVector;
	UGizmoMathLibrary::CalculateMeshBounds(this, OUT Center, OUT MeshExtention);

	TrackerDistance = FMath::Max3<float>(MeshExtention.X, MeshExtention.Y, MeshExtention.Z);
}

void AGizmoActorBase::SetDynamicMaterials(TArray<UStaticMeshComponent*> In_SM, TArray<UMaterialInstanceDynamic*>& Out_DM)
{
	for (UStaticMeshComponent* sm : In_SM)
	{
		TArray<UMaterialInterface*> MI = sm->GetMaterials();

		int32 index = 0;
		for (auto MItem : MI)
		{
			UMaterialInstanceDynamic* DM = sm->CreateAndSetMaterialInstanceDynamicFromMaterial(index, sm->GetMaterial(index));
			Out_DM.AddUnique(DM);
			++index;
			//UE_LOG(LogTemp, Error, TEXR("AGizmoActorBase::SetDynamicMaterials"));
		}
	}
}

void AGizmoActorBase::MakeTranslucent(bool bTranslucent)
{
	float value = bTranslucent ? Translucent : 1.f;

	for (UMaterialInstanceDynamic* DM : DyanicMaterials)
	{
		DM->SetScalarParameterValue(FName("Translucent"), value);
	}
}

void AGizmoActorBase::TrackActor()
{
	if(!TrackerActor) return;

	float Distance = FVector::DistSquared(TrackerActor->GetActorLocation(), GetActorLocation());

	if (Distance > (TrackerDistance * TrackerDistance))
	{
		GetWorld()->GetTimerManager().ClearTimer(GizmoTrace_Timer);
		MakeTranslucent(false);
		SetActorCollisionResponse(ECR_Block);
		OnEndTrack.Broadcast(this);
		//UE_LOG(LogTemp, Error, TEXT("AGizmoActorBase::TrackActor Track Finished !!!!"));
	}
}



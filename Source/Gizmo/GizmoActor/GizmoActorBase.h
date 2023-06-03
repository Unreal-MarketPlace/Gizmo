// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Gizmo/Interface/GizmoActorInterface.h"
#include "Gizmo/Gizmo.h"
#include "GizmoActorBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGizmoActorDelegate, AActor*, SelfActor);

/**
 * 
 */
UCLASS()
class GIZMO_API AGizmoActorBase : public AStaticMeshActor, public IGizmoActorInterface
{
	GENERATED_BODY()

public:

	AGizmoActorBase(const FObjectInitializer& ObjectInitializer);

	/********** Interface *************/
	virtual void TrackGizmoActor_Implementation(AActor* GActor) override;

	/*********** if need more Mesh override in BP *************/
	UFUNCTION(BlueprintNativeEvent, Category = "Gizmo Actor")
	TArray<UStaticMeshComponent*> GetMeshes();
	virtual TArray<UStaticMeshComponent*> GetMeshes_Implementation();

	UFUNCTION(BlueprintPure, Category = "Gizmo Actor")
	void GetDynamicMaterials(TArray<UMaterialInstanceDynamic*>& DM) { DM = DyanicMaterials; }

	void SetActorCollisionResponse(ECollisionResponse EResponse);

	UPROPERTY(BlueprintAssignable)
	FGizmoActorDelegate OnEndTrack;

protected:

	virtual void BeginPlay() override;

	void SetDynamicMaterials(TArray<UStaticMeshComponent*> In_SM, TArray<UMaterialInstanceDynamic*>& Out_DM);

private:
	
	void MakeTranslucent(bool bTranslucent);

	void TrackActor();

protected:

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.f, ClampMax = 1.f), Category = "Gizmo Actor")
	float Translucent = 0.3;

	FTimerHandle GizmoTrace_Timer;
	float TrackerDistance = 0.f;
	FVector MeshExtention = FVector::ZeroVector;
	AActor* TrackerActor = NULL;

	TArray<UMaterialInstanceDynamic*> DyanicMaterials;
};

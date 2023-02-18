// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gizmo/Player/GizmoCharacter.h"
#include "GizmoComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GIZMO_API UGizmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGizmoComponent();

	void GizmoTrace();

	void SetGizmoActorSettings(bool On_Off);

	void AttachGizmo();

	void MakeGizmoActorTranslucent(bool OnOff, AActor* GActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	bool CanGizmoAttach(const FHitResult& Hit);

protected:

	AGizmoCharacter* OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo | Settings")
	float GizmoTraceDistance = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo | Settings")
	FVector GizmoTraceOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo | Debug")
	bool bGizmoTraceVisible = false;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo | Debug")
	bool bDebugText = true;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo | Settings ", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float GizmoActorTranslucent = 0.5;


private:
	
	UPROPERTY()
	UPrimitiveComponent* AX_GizmoActive;
	UPROPERTY()
	UMaterialInstanceDynamic* MD_GizmoActive;
	UPROPERTY()
	EGizmo GizmoTouch = EGizmo::None;

	void MakeTranslucent(bool OnOff, UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material);
	void GetGizmoActorDM(UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material);

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DM_GizmoActor;
	UPROPERTY()
	UStaticMeshComponent* SM_GizmoActor;
		
};

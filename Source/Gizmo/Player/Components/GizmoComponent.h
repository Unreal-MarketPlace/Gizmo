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

	void SetGizmoInputMode(bool IsGizmoActive);
	void PressedGizmoTool();
	void ReleasedGizmoTool();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetGizmoMaterial();
	void ActivateGizmo(EGizmo ActiveTouch, bool DoActive);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	bool CanGizmoAttach(const FHitResult& Hit);

	void CoursorTrace(FHitResult& Hit);

protected:

	AGizmoCharacter* OwnerCharacter;
	APlayerController* OwnerController;

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
	EGizmo GizmoTouch = EGizmo::None;

	void MakeTranslucent(bool OnOff, UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material);
	void GetGizmoActorDM(UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material);

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DM_GizmoActor;
	UPROPERTY()
	UStaticMeshComponent* SM_GizmoActor;


	//
	FVector2D MouseTouchPoint = FVector2D::ZeroVector;
	FVector TouchLocation = FVector::ZeroVector;
	FVector TouchDirection = FVector::ZeroVector;



	UMaterialInstanceDynamic* MD_GizmoX;
	UMaterialInstanceDynamic* MD_GizmoY;
	UMaterialInstanceDynamic* MD_GizmoZ;
	UMaterialInstanceDynamic* MD_GizmoPitch;
	UMaterialInstanceDynamic* MD_GizmoRoll;
	UMaterialInstanceDynamic* MD_GizmoYaw;
		
};

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
	/********* Server ***********/
	void GizmoTrace();
	void SetGizmoActorSettings(bool On_Off, AActor* GActor = NULL);

	void AttachDetachGizmo(bool bAttach = true);

	/********* Only Client side *********/
	void MakeGizmoActorTranslucent(bool OnOff, AActor* GActor);
	// For other Gizmo Actors
	void MakeGizmoActorTranslucent(bool OnOff, AActor* GActor, TArray<UMaterialInstanceDynamic*>& DM_Materials);
	void AttachDeatachActorToGizmoActor(AActor* OtherGizmoActor);

	void SetGizmoInputMode(bool IsGizmoActive);
	void PressedGizmoTool();
	void ReleasedGizmoTool();

	void UpdatedGizmoActorTransform(float DeltaTime);

	void DeleteGizmoActor();

	void RemoveAllAttachedGizmoActor();

	ESnapping GetSnappingMothod() { return SnappingMethod; }
	void SetSnappingMethod(ESnapping NewMethod) { SnappingMethod = NewMethod; }
	void GetKeySnappingByTransition(EGizmoTransition GTransition, TArray<ESnapping>& KeySnappings);

	float GetMouseSensitive() { return MouseSensitive; }
	void SetMouseSenstive(float NewSensitive) { MouseSensitive = NewSensitive; }

	EGizmoTransition GetGizmoTransition() { return GizmoTransition; }
	void SetGizmoTransition(EGizmoTransition NewGizmoTransition);
	// Call only Server side
	void ServerSetGizmoTransition(EGizmoTransition NewGizmoTransition) { GizmoTransition = NewGizmoTransition; }

protected:
	
	void RemoveAttachedGizmoActor(AActor* GActor);
	UFUNCTION(Server, Reliable)
	void SR_RemoveAttachedGizmoActor(AActor* GActor);

	// Called when the game starts
	virtual void BeginPlay() override;


	void SetGizmoMaterial();
	void ActivateGizmo(EGizmo ActiveTouch, EGizmoTransition GTransition, bool DoActive);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	bool CanGizmoAttach(const FHitResult& Hit);

	void SearchGizmoTool(FHitResult& Hit);

	void CoursorTrace(FHitResult& Hit, FVector2D& TouchPixel, FVector& TouchPixelLocation, FVector& TouchPixelDirection);

	float UpdateMousePosition(float CurrentPixel, float PassedPixel);

	void MakeTranslucent(bool OnOff, UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material);
	void GetGizmoActorDM(UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material);
	/* Only for MainGizmo Actor */
	UStaticMeshComponent* SetGizmoActorCollisionResponse(bool OnOff, AActor* GActor);
	/* */
	float GetMoveStep(EGizmo TouchAxis, EGizmoTransition GTransitoin, float DeltaTime, const FGizmoMovementData& GizmoMovement);

	UFUNCTION(Server, Reliable)
	void SR_UpdateGizmoActorTransform(const FTransform& NewTransform);

	UFUNCTION(NetMulticast, Reliable)
	void MC_UpdateGizmoActorTransform(const FTransform& NewTransform, ACharacter* InstigatorCharacter, AActor* TargetActor);

	FString GetEnumString(const FString& EnumString, uint8 EnemElement);

	float GetSnappingRate(EGizmo TouchAxis, ESnapping Snapping, EGizmoTransition GTransition);

	TMap<ESnapping, float>* GetSnappingCategoryByTransition(EGizmoTransition GTransition);
	float GetDefaultSnappingByTransition(EGizmoTransition GTransition);

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

	float DefaultSnappingLocation = 5.f;
	float DefaultSnappingRotatoin = 5.f;
	float DefaultSnappingScale    = 0.2;

	/******** Gizmo Transition *********/
	EGizmoTransition GizmoTransition = EGizmoTransition::Location;

	/********* Gizmo Touch *********/
	EGizmo GizmoTouch = EGizmo::None;
	FGizmoMovementData GizmoMovementData;



	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DM_GizmoActor;
	UPROPERTY()
	UStaticMeshComponent* SM_GizmoActor;
	
	UPROPERTY()
	TMap<AActor*, FGizmoActorProperty> TM_OtherGizmoActor;


	/************** Coursor Trace *************/
	FVector2D MouseTouchPoint = FVector2D::ZeroVector;
	FVector TouchLocation = FVector::ZeroVector;
	FVector TouchDirection = FVector::ZeroVector;



	UMaterialInstanceDynamic* MD_GizmoX;
		UMaterialInstanceDynamic* MD_GizmoSX;
	UMaterialInstanceDynamic* MD_GizmoY;
		UMaterialInstanceDynamic* MD_GizmoSY;
	UMaterialInstanceDynamic* MD_GizmoZ;
		UMaterialInstanceDynamic* MD_GizmoSZ;
	UMaterialInstanceDynamic* MD_GizmoPitch;
	UMaterialInstanceDynamic* MD_GizmoRoll;
	UMaterialInstanceDynamic* MD_GizmoYaw;

	/******* Gizmo Snapping **********/
	UPROPERTY()
	ESnapping SnappingMethod = ESnapping::Off;
	UPROPERTY()
	TMap<ESnapping, float> TM_SnappingLocation;
	UPROPERTY()
	TMap<ESnapping, float> TM_SnappingRotation;
	UPROPERTY()
	TMap<ESnapping, float> TM_SnappingScale;

	/************ Per Pixel: 0 = High Sensitve***************/
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, ClampMax = 100.f), Category = "Gizmo | Settings")
	float MouseSensitive = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo | Settings")
	bool bHideCursorDuringCapture = false;

	bool bPressedGizmoTool = false;
		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gizmo/Gizmo.h"
#include "GizmoDetectorComponent.generated.h"

class AGizmoCharacter;
class UGizmoComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGizmoOverlapDelegate, bool, IsOverlap);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GIZMO_API UGizmoDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGizmoDetectorComponent();

	void Init(UGizmoComponent* GComponent);

public:

	UPROPERTY(BlueprintAssignable)
	FGizmoOverlapDelegate GizmoOverlapDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	UFUNCTION()
	void TraceLauncher(EGizmoActiveStatus GActiveStatus);
	void StartTrace();
	void StopTrace();

	void OverlapGizmo(EGizmo ActiveTouch, EGizmoTransition GTransition, bool DoActive);

	void LightOffGizmo();


protected:

	UPROPERTY(EditDefaultsOnly, Category = "Default Settings")
	float Frequency = 0.5f;

private:

	AGizmoCharacter* GizmoCharacter;
	UGizmoComponent* OwnerComponent;
	UMaterialInstanceDynamic* DM_Active;
	EGizmo GizmoOverlap = EGizmo::None;
	EGizmo SavedGizmoOverlap = EGizmo::None;


	FTimerHandle Timer_GizmoDetector;
		
};

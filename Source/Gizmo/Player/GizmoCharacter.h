// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gizmo/Gizmo.h"
#include "GizmoCharacter.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UGizmoComponent;
class AGizmoActorBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGizmoDelegate, EGizmoActiveStatus, GizmoActiveStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGizmoTransitionDelegate, EGizmoTransition, GTransition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnyDelegate);


UCLASS(config=Game)
class AGizmoCharacter : public ACharacter
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Main", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* GSphere;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gizmo Main", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gizmo Main", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	// Gizmo
	// Gizmo Arrow
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class UStaticMeshComponent* GArrowX;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class UStaticMeshComponent* GArrowSX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class UStaticMeshComponent* GArrowY;
		
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class UStaticMeshComponent* GArrowSY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class UStaticMeshComponent* GArrowZ;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class UStaticMeshComponent* GArrowSZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class UStaticMeshComponent* GPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class UStaticMeshComponent* GRoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class UStaticMeshComponent* GYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class USceneComponent* GPivot;

	UPROPERTY(BlueprintAssignable)
	FGizmoDelegate GizmoDelegate;

	UPROPERTY(BlueprintAssignable)
	FGizmoTransitionDelegate GizmoTransitionDelegate;

	UPROPERTY(BlueprintAssignable)
	FAnyDelegate ToolTipDelegate;



	/****** Enable GizmoActor movement *********/
	bool CanUpdateGizmoActorTransform = false;

protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
	class UGizmoComponent* GizmoComponent;


	UPROPERTY(VisibleAnywhere, Category = "Gizmo Tool")
	FGizmo GizmoTool;

	// Update only Instigator Client
	UPROPERTY(ReplicatedUsing = OnRep_GizmoActor)
	AActor* GizmoActor = NULL;
	UFUNCTION()
	void OnRep_GizmoActor(AActor* OldGizmoActor);

	UPROPERTY(ReplicatedUsing = OnRep_CopyGizmoActor)
	AActor* CopiedGizmoActor = NULL;
	UFUNCTION()
	void OnRep_CopyGizmoActor();
	UPROPERTY(Replicated)
	TArray<AActor*> CopiedOtherGizmoActors;

	UFUNCTION(Server, Reliable)
	void SR_UpdateCopiedActorCollision(AActor* CopiedActor);

	UPROPERTY(Replicated)
	TArray<AActor*> OtherGizmoActors;

	// Update every Client
	UPROPERTY(Replicated)
	bool IsGizmoActorValid = false;

	UPROPERTY(Replicated)
	bool bCTRL;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gizmo Tool")
	EGizmoActiveStatus GizmoActvationStatus = EGizmoActiveStatus::None;

public:

	AGizmoCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo Tool | Debug")
	bool bDefaultVisibleGizmo = false;

public:

	// Gizmo
	UFUNCTION(BlueprintPure, Category = "Gizmo Tool")
	FGizmo GetGizmoTool() { return GizmoTool; }

	UFUNCTION(BlueprintPure, Category = "Gizmo")
	AActor* GetGizmoActor() { return GizmoActor; }

	UFUNCTION(BlueprintPure, Category = "Gizmo")
	TArray<AActor*> GetAttachedGizmoActors() { return OtherGizmoActors; }

	UFUNCTION(BlueprintPure, Category = "Gizmo")
	FORCEINLINE bool IsControlPressed() { return bCTRL; }

	UFUNCTION(BlueprintPure, Category = "Gizmo Tool")
	ESnapping GetGizmoSnappingMethod();

	UFUNCTION(BlueprintCallable, Category = "Gizmo Tool")
	void SetGizmoSnappingMethod(ESnapping NewMethod);

	UFUNCTION(BlueprintPure, Category = "Gizmo Tool")
	void GetKeySnappingsByTransition(EGizmoTransition GTransition, TArray<ESnapping>& KeySnappings);

	UFUNCTION(BlueprintPure, Category = "Gizmo Tool")
	float GetGizmoMouseSensitive();

	UFUNCTION(BlueprintCallable, Category = "Gizmo Tool")
	void SetGizmoMouseSensitive(float NewSensitive);

	UFUNCTION(BlueprintPure, Category = "Gizmo Tool")
	EGizmoTransition GetGizmoTransition();

	UFUNCTION(BlueprintCallable, Category = "Gizmo Tool")
	void SetGizmoTransition(EGizmoTransition NewTransition);

	UFUNCTION(Client, Reliable)
	void CL_PressedGizmoTool();


	// Call On Server side
	void SetMainGizmoActor(AActor* GActor, const EGizmoActiveStatus GActiveStatus = EGizmoActiveStatus::None);

	// Call on Server
	void SetOtherGizmoActors(AActor* OtherActor);

	/******** Debug **********/
	void PrintLocalRole();

	UFUNCTION(BlueprintImplementableEvent, Category = "Gizmo Tool")
	void BP_GizmoTouch(bool bTouch, EGizmo TouchedAxis);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gizmo Tool")
	void BP_GizmoOverlap(bool bTouch, EGizmo TouchedAxis);

protected:

	UFUNCTION(Server, Reliable)
	void SR_SetGizmoTransition(EGizmoTransition NewTransition);

	UFUNCTION(Server, Reliable)
	void SR_UpdateCTRL(bool Status);

	UFUNCTION(Client, Reliable)
	void CL_SetOtherGizmoActors(AActor* OtherGizmoActor);


public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/******** Called for Up/down input **************/
	void MoveUp(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	 /*** PC ****/
	void Turn(float Rate);
	void LookUp(float Rate);
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void LeftMousePressed();
	void LeftMouseReleased();

	void CTRL_Pressed();
	void CTRL_Released();

	void DeleteGizmoActor();
	UFUNCTION(Server, Reliable)
	void SR_DeleteGizmoActor();

	void DropGizmoActor();
	UFUNCTION(Server, Reliable)
	void SR_DropGizmoActor();

	void CopyGizmoActor();
	UFUNCTION(Server, Reliable)
	void SR_CopyGizmoActor();
	UFUNCTION(Server, Reliable)
	void SR_StopCopyActorReplication();

	void SetGizmoLocationTransition();
	void SetGizmoRotationTransition();
	void SetGizmoScaleTransition();

	void ToolTip();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	FTimerHandle CopyActor_Timer;
	UPROPERTY(Replicated)
	bool bCopyCooldown = false;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


};


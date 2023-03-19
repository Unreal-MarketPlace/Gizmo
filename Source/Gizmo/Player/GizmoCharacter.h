// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gizmo/Gizmo.h"
#include "GizmoCharacter.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UGizmoComponent;



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
		class UStaticMeshComponent* GArrowY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class UStaticMeshComponent* GArrowZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class UStaticMeshComponent* GPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class UStaticMeshComponent* GRoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class UStaticMeshComponent* GYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gizmo Tool")
		class USceneComponent* GPivot;


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

	// Update every Client
	UPROPERTY(Replicated)
	bool IsGizmoActorValid = false;

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
	FORCEINLINE FGizmo GetGizmoTool() { return GizmoTool; }

	UFUNCTION(BlueprintPure, Category = "Gizmo")
	FORCEINLINE AActor* GetGizmoActor() { return GizmoActor; }

	UFUNCTION(Client, Reliable)
	void CL_PressedGizmoTool();


	// Call On Server side
	void SetGizmoActor(AActor* GActor);

	/******** Debug **********/
	void PrintLocalRole();

protected:

	UFUNCTION(Server, Reliable)
	void SR_GizmoTrace();

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

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


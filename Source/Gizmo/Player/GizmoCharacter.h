// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GizmoCharacter.generated.h"

class UStaticMeshComponent;
class USceneComponent;

USTRUCT(BlueprintType)
struct FGizmo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoArrowX;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoArrowY;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoArrowZ;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoPitch;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoRoll;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoYaw;

	UPROPERTY(BlueprintReadOnly)
	class USceneComponent* GizmoPivot;

public:

	void Initilize(UStaticMeshComponent* GX, UStaticMeshComponent* GY, UStaticMeshComponent* GZ, UStaticMeshComponent* GP, UStaticMeshComponent* GR, UStaticMeshComponent* GYa, USceneComponent* GPi)
	{
		GizmoArrowX = GX;
		GizmoArrowY = GY;
		GizmoArrowZ = GZ;
		GizmoPitch  = GP;
		GizmoRoll   = GR;
		GizmoYaw    = GYa;
		GizmoPivot  = GPi;
	}


	void SetGizmoToolVisibility(bool bVisible)
	{
		GizmoArrowX->SetVisibility(bVisible);
		GizmoArrowY->SetVisibility(bVisible);
		GizmoArrowZ->SetVisibility(bVisible);
		GizmoPitch->SetVisibility(bVisible);
		GizmoRoll->SetVisibility(bVisible);
		GizmoYaw->SetVisibility(bVisible);
		GizmoPivot->SetVisibility(bVisible);
	}
	
};

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

protected:

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


	UPROPERTY(VisibleAnywhere, Category = "Gizmo Tool")
	FGizmo GizmoTool;

	UPROPERTY(EditDefaultsOnly, Category = "Gizmo Tool | Debub")
	bool bDefaultVisibleGizmo = false;

public:

	AGizmoCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	// Gizmo
	UFUNCTION(BlueprintPure, Category = "Gizmo Tool")
	FGizmo GetGizmoTool() { return GizmoTool; }

protected:

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


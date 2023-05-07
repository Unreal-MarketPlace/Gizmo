// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Gizmo.generated.h"

#define OUT

UENUM(BlueprintType)
enum class ESnapping : uint8
{
	//
	Off         UMETA(DisplayName = "Off"),
	Mini        UMETA(DisplayName = "Half Unit"),
	One         UMETA(DisplayName = "One Unit"),
	Ten         UMETA(DisplayName = "Ten Unit"),
	Twenty      UMETA(DisplayName = "Twenty Unit"),
	Fifty       UMETA(DisplayName = "Fifty Unit"),
	Hundred     UMETA(DisplayName = "Hundred Unit"),
	Auto        UMETA(DisplayName = "Auto"),

	// Rotation
	Fiveteen    UMETA(DisplayName = "Fiveteen Unit"),
	FourtyFive  UMETA(DisplayName = "FourtyFive Unit"),

	// Scale
	Two         UMETA(DisplayName = "Two Unit"),
	Three       UMETA(DisplayName = "Three Unit")

};

UENUM(BlueprintType)
enum class EGizmo : uint8
{
	None,
	X,
	Y,
	Z,
	Pitch,
	Roll,
	Yaw
};


UENUM(BlueprintType)
enum class EGizmoActiveStatus : uint8
{
	None,
	Attached   UMETA(DisplayName = "Attached Actor"),
	Change     UMETA(DisplayName = "Change Actor"),
	Drop       UMETA(DisplayName = "Drop Actor"),
	Delete     UMETA(DisplayName = "Delete Actor"),
};



UENUM(BlueprintType)
enum class EGizmoTransition : uint8
{
	Location,
	Rotation,
	Scale,
	None
};


USTRUCT(BlueprintType)
struct FGizmo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoArrowX;
		
		UPROPERTY(BlueprintReadOnly)
		class UStaticMeshComponent* GizmoArrowSX;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoArrowY;
		
		UPROPERTY(BlueprintReadOnly)
		class UStaticMeshComponent* GizmoArrowSY;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoArrowZ;

		UPROPERTY(BlueprintReadOnly)
		class UStaticMeshComponent* GizmoArrowSZ;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoPitch;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoRoll;

	UPROPERTY(BlueprintReadOnly)
	class UStaticMeshComponent* GizmoYaw;

	UPROPERTY(BlueprintReadOnly)
	class USceneComponent* GizmoPivot;


public:

	void Initilize(UStaticMeshComponent* GX, UStaticMeshComponent* GSX, UStaticMeshComponent* GY, UStaticMeshComponent* GSY, UStaticMeshComponent* GZ, UStaticMeshComponent* GSZ, UStaticMeshComponent* GP, UStaticMeshComponent* GR, UStaticMeshComponent* GYa, USceneComponent* GPi)
	{
		GizmoArrowX = GX;
			GizmoArrowSX = GSX;
		GizmoArrowY = GY;
			GizmoArrowSY = GSY;
		GizmoArrowZ = GZ;
			GizmoArrowSZ = GSZ;
		GizmoPitch = GP;
		GizmoRoll = GR;
		GizmoYaw = GYa;
		GizmoPivot = GPi;
	}


	void SetGizmoToolVisibility(bool bVisible)
	{

		GizmoArrowX->SetVisibility(bVisible, false);
			GizmoArrowSX->SetVisibility(bVisible, false);
		GizmoArrowY->SetVisibility(bVisible, false);
			GizmoArrowSY->SetVisibility(bVisible, false);
		GizmoArrowZ->SetVisibility(bVisible, false);
			GizmoArrowSZ->SetVisibility(bVisible, false);
		GizmoPitch->SetVisibility(bVisible, false);
		GizmoRoll->SetVisibility(bVisible, false);
		GizmoYaw->SetVisibility(bVisible, false);

		GizmoPivot->SetVisibility(bVisible, false);
	}

	void SetGizmoToolVisibilityByTransition(EGizmoTransition GTransition, bool bVisible)
	{
		SetGizmoToolVisibility(false);
		GizmoPivot->SetVisibility(bVisible);
		switch (GTransition)
		{
		case EGizmoTransition::Location:
			GizmoArrowX->SetVisibility(bVisible);
			GizmoArrowY->SetVisibility(bVisible);
			GizmoArrowZ->SetVisibility(bVisible);
			break;
		case EGizmoTransition::Rotation:
			GizmoPitch->SetVisibility(bVisible);
			GizmoRoll->SetVisibility(bVisible);
			GizmoYaw->SetVisibility(bVisible);
			break;
		case EGizmoTransition::Scale:
			GizmoArrowX->SetVisibility(bVisible);
			GizmoArrowSX->SetVisibility(bVisible);
			GizmoArrowY->SetVisibility(bVisible);
			GizmoArrowSY->SetVisibility(bVisible);
			GizmoArrowZ->SetVisibility(bVisible);
			GizmoArrowSZ->SetVisibility(bVisible);
			break;
		default:
			break;
		}
	}

	bool IsGizmoValid()
	{
		return (GizmoArrowX && GizmoArrowSX && GizmoArrowY && GizmoArrowSY && GizmoArrowZ && GizmoArrowSZ && GizmoPitch && GizmoRoll && GizmoYaw && GizmoPivot) ? true : false;
	}


	EGizmo GetGizmoTouch(UPrimitiveComponent* TouchComponent)
	{
		EGizmo temp = EGizmo::None;
		if (TouchComponent == GizmoArrowX || TouchComponent == GizmoArrowSX)
			temp = EGizmo::X;
		else if (TouchComponent == GizmoArrowY || TouchComponent == GizmoArrowSY)
			temp = EGizmo::Y;
		else if (TouchComponent == GizmoArrowZ || TouchComponent == GizmoArrowSZ)
			temp = EGizmo::Z;
		else if (TouchComponent == GizmoPitch)
			temp = EGizmo::Pitch;
		else if (TouchComponent == GizmoRoll)
			temp = EGizmo::Roll;
		else if (TouchComponent == GizmoYaw)
			temp = EGizmo::Yaw;

		return temp;

	}

	USceneComponent* GetGizmoPivot() { return GizmoPivot; }

};


USTRUCT(BlueprintType)
struct FGizmoMovementData
{

	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly)
	float GizmoAxisDirection;
	UPROPERTY(BlueprintReadOnly)
	float MouseUpdateDirection;
	UPROPERTY(BlueprintReadOnly)
	bool IsAxisForward;
	UPROPERTY(BlueprintReadOnly)
	FQuat GizmoQuat = FQuat::Identity;
	UPROPERTY(BlueprintReadOnly)
	FVector GizmoLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	FVector GizmoScale = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly)
	FTransform GizmoTransform = FTransform::Identity;
	UPROPERTY(BlueprintReadOnly)
	FVector2D UpdateMouseTouch;

	UPROPERTY(BlueprintReadOnly)
	bool IsDataValid = false;

public:

	void ClearData()
	{
		GizmoAxisDirection = 0.f;
		MouseUpdateDirection = 0.f;
		IsAxisForward = false;
		GizmoQuat = FQuat::Identity;
		GizmoLocation = FVector::ZeroVector;
		GizmoTransform = FTransform::Identity;
		UpdateMouseTouch = FVector2D::ZeroVector;

		IsDataValid = false;
	}
};


USTRUCT()
struct FGizmoActorProperty
{
	GENERATED_USTRUCT_BODY()
public:

	TArray<UMaterialInstanceDynamic*> DM_OtherActor;
	AActor* OtherGizmoObject;

	FGizmoActorProperty() {}

	void ClearData()
	{
		OtherGizmoObject = nullptr;
		DM_OtherActor.Empty();
	}
};
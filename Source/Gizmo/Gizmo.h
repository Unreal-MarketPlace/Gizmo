// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gizmo.generated.h"

#define OUT


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

protected:

	UMaterialInstanceDynamic* MD_GizmoX;
	UMaterialInstanceDynamic* MD_GizmoY;
	UMaterialInstanceDynamic* MD_GizmoZ;
	UMaterialInstanceDynamic* MD_GizmoPitch;
	UMaterialInstanceDynamic* MD_GizmoRoll;
	UMaterialInstanceDynamic* MD_GizmoYaw;

public:

	void Initilize(UStaticMeshComponent* GX, UStaticMeshComponent* GY, UStaticMeshComponent* GZ, UStaticMeshComponent* GP, UStaticMeshComponent* GR, UStaticMeshComponent* GYa, USceneComponent* GPi)
	{
		GizmoArrowX = GX;
		GizmoArrowY = GY;
		GizmoArrowZ = GZ;
		GizmoPitch = GP;
		GizmoRoll = GR;
		GizmoYaw = GYa;
		GizmoPivot = GPi;
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

	void SetGizmoMaterial()
	{
		if(!IsGizmoValid()) return;

		MD_GizmoX     = GizmoArrowX->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GizmoArrowX->GetMaterial(0));
		MD_GizmoY     = GizmoArrowY->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GizmoArrowY->GetMaterial(0));
		MD_GizmoZ     = GizmoArrowZ->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GizmoArrowZ->GetMaterial(0));
		MD_GizmoPitch = GizmoPitch->CreateAndSetMaterialInstanceDynamicFromMaterial(0,  GizmoPitch->GetMaterial(0));
		MD_GizmoRoll  = GizmoRoll->CreateAndSetMaterialInstanceDynamicFromMaterial(0,   GizmoRoll->GetMaterial(0));
		MD_GizmoYaw   = GizmoYaw->CreateAndSetMaterialInstanceDynamicFromMaterial(0,    GizmoYaw->GetMaterial(0));
		UE_LOG(LogTemp, Error, TEXT("FGizmo::SetGizmoMaterial"));
	}

	USceneComponent* GetGizmoPivot() { return GizmoPivot; }


	bool IsGizmoValid()
	{
		return (GizmoArrowX && GizmoArrowY && GizmoArrowZ && GizmoPitch && GizmoRoll && GizmoYaw && GizmoPivot) ? true : false;
	}

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
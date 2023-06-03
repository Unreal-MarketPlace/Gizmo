// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Gizmo/Player/GizmoCharacter.h"
#include "Gizmo/Gizmo.h"
#include "GizmoMathLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GIZMO_API UGizmoMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	static void GetGizmoAxisDirection(EGizmo GizmoTouch, FVector Touchpoint, AGizmoCharacter* InstigatorCharacter, FGizmoMovementData& GizmoTouchData, bool bShowDebugData = false, bool bShowDebugLines = false);

	UFUNCTION(BlueprintCallable)
	static FQuat GetGizmoQuat(EGizmo Axis, float roll, float pitch, float yaw);

	UFUNCTION(BlueprintCallable)
	static float GetAutoSnappingRate(EGizmo TouchAxis, AActor* SnappingActor, bool bDrawDebugBox);

	UFUNCTION(BlueprintCallable)
	static void CalculateMeshBounds(AActor* Object, FVector& Min, FVector& Max);

private:

	static void DrawDotProductLines(AGizmoCharacter* InstigatorCharacter, FVector TouchPoint, FVector TouchArrowLocation, FVector ArrowUnitVector, FVector MMUnitVector);
	static void PrintDebugData(AGizmoCharacter* InstigatorCharacter, EGizmo GizmoState, float dProduct1, float dProduct2 = 0.f, float dProduct3 = 0.f, float dProduct4 = 0, float dProduct5 = 0, float dProduct6 = 0);
	static bool IsArrowForward(FVector VNormal, bool bShowDebugData = false);
	
};

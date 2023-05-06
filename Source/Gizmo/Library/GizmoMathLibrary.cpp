// Fill out your copyright notice in the Description page of Project Settings.


#include "Gizmo/Library/GizmoMathLibrary.h"
#include "Gizmo/GizmoActor/GizmoActorBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UGizmoMathLibrary::GetGizmoAxisDirection(EGizmo GizmoTouch, FVector Touchpoint, AGizmoCharacter* InstigatorCharacter, FGizmoMovementData& GizmoTouchData, bool bShowDebugData /* false */, bool bShowDebugLines /* false */)
{


	if (!InstigatorCharacter)
	{
		GizmoTouchData.IsDataValid = false;
		return;
	}

	float MouseDirection = 1;
	float dProduct = 0;

	// X     // Yaw      // Roll
	float dProductUR = 0; // MM up && GarrowX right
	float dProductFF = 0; // MM forward && GarrowX forward

	// Y     // Yaw     // Pitch
	float dProductUF = 0; // MM up && GarrowY forward
	float dProductFR = 0; // MM forward && GarrowY right
	float dProductUU = 0; // MM up && GarrawZ up

	//Pitch  // Roll
	float dProductRU = 0; // MM right && GarrowZ up
	float dProductRF = 0; // MM right && GarrowX forward

	//Roll
	float dProductRR = 0; // MM right && GarrowY right

	bool bForward = false;

	FVector MMLocation = InstigatorCharacter->GetActorLocation();
	FVector TouchPointDir = UKismetMathLibrary::GetDirectionUnitVector(MMLocation, Touchpoint);
	FVector End2 = MMLocation + (TouchPointDir * 500);
	//End2 = MMLocation + (UKismetMathLibrary::GetDirectionUnitVector(MMLocation, MMLocation + InstigatorCharacter->GetActorForwardVector()) * 500);
	//FVector End3 = MMLocation + (UKismetMathLibrary::GetDirectionUnitVector(MMLocation, MMLocation + InstigatorCharacter->GetActorUpVector()) * 500);

	switch (GizmoTouch)
	{
	case EGizmo::X:

		bForward = IsArrowForward(InstigatorCharacter->GArrowX->GetForwardVector(), bShowDebugData);

		/// Need for debug is or not bForward -- FUture
		/*dProductUF = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(),    InstigatorCharacter->GArrowX->GetForwardVector());
		dProductRF = FVector::DotProduct(InstigatorCharacter->GetActorRightVector(),   InstigatorCharacter->GArrowX->GetForwardVector());
		dProductFF = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowX->GetForwardVector());

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UGizmoComponent::GetMouseMoveDirection dProductUF = %f"), dProductUF));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("UGizmoComponent::GetMouseMoveDirection dProductRF = %f"), dProductRF));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("UGizmoComponent::GetMouseMoveDirection dProductFF = %f"), dProductFF));

		PG_LOG(Error, TEXT("UGizmoComponent::GetMouseMoveDirection bForward = %i"), bForward);*/

		if (bForward)
		{

			dProductRF = FVector3d::DotProduct(InstigatorCharacter->GetActorRightVector(),   InstigatorCharacter->GArrowX->GetForwardVector());
			dProductFF = FVector3d::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowX->GetForwardVector());

			if (bShowDebugLines)
			{
				DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowX->GetComponentLocation(), InstigatorCharacter->GArrowX->GetForwardVector(), InstigatorCharacter->GetActorForwardVector());
				DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowX->GetComponentLocation(), InstigatorCharacter->GArrowX->GetForwardVector(), InstigatorCharacter->GetActorRightVector());
			}

			if (FMath::Abs(dProductRF) > FMath::Abs(dProductFF))
			{
				dProductUU = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(),      InstigatorCharacter->GArrowZ->GetUpVector());
				dProductFR = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowY->GetRightVector());

				if (dProductUU > 0)
					dProduct = -dProductFR;
				else if (dProductUU < 0)
					dProduct = dProductFR;

				if (bShowDebugData)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection Looking X Axis from the RIGHT ")));
					PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, dProductFF, dProductRF, dProductUU, dProductFR);
				}

			}
			else
			{
				dProduct = dProductRF;

				if (bShowDebugData)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("UGizmoComponent::GetMouseMoveDirection Looking X Axis FORWARD ")));
					PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, 0.f, dProductRF);
				}
			}


			dProduct = dProduct > 0 ? 1 : -1;
			MouseDirection = dProduct;


		}
		else
		{
			MouseDirection = -1;
			MouseDirection *= (InstigatorCharacter->GArrowX->GetForwardVector().Z > 0 ? 1 : -1);
		}


		break;
	case EGizmo::Y:

		bForward = IsArrowForward(InstigatorCharacter->GArrowY->GetRightVector(), bShowDebugData);

		if (bForward)
		{
			dProductRR = FVector::DotProduct(InstigatorCharacter->GetActorRightVector(),   InstigatorCharacter->GArrowY->GetRightVector());
			dProductFR = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowY->GetRightVector());

			if (bShowDebugLines)
			{
				DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowY->GetComponentLocation(), InstigatorCharacter->GArrowY->GetRightVector(), InstigatorCharacter->GetActorRightVector());
				DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowY->GetComponentLocation(), InstigatorCharacter->GArrowY->GetRightVector(), InstigatorCharacter->GetActorForwardVector());
			}

			if (FMath::Abs(dProductRR) > FMath::Abs(dProductFR))
			{
				dProductUU = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(),      InstigatorCharacter->GArrowZ->GetUpVector());
				dProductFF = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowX->GetForwardVector());

				if (dProductUU > 0)
					dProduct = dProductFF;
				else
					dProduct = -dProductFF;

				if (bShowDebugData)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection Looking Y Axis from the RIGHT ")));
					PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, dProductRR, dProductFR, dProductUU, dProductFF);
				}

			}
			else
			{
				dProduct = dProductRR;
				if (bShowDebugData)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection Looking Y Axis FORWARD ")));
					PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, dProductRR);
				}
			}


			dProduct = dProduct > 0 ? 1 : -1;
			MouseDirection = dProduct;
		}
		else
		{
			MouseDirection = -1;
			MouseDirection *= (InstigatorCharacter->GArrowY->GetRightVector().Z > 0 ? 1 : -1);
		}

		break;
	case EGizmo::Z:

		bForward = IsArrowForward(InstigatorCharacter->GArrowZ->GetUpVector(), bShowDebugData);

		if (bForward)
		{
			if (FMath::Abs(InstigatorCharacter->GArrowZ->GetForwardVector().Z) > FMath::Abs(InstigatorCharacter->GArrowZ->GetRightVector().Z)) // Up is GarrowZ.X axis
			{
				dProduct = FVector::DotProduct(TouchPointDir, InstigatorCharacter->GArrowZ->GetRightVector());

				if (bShowDebugLines)
					DrawDotProductLines(InstigatorCharacter, Touchpoint, InstigatorCharacter->GArrowZ->GetComponentLocation(), InstigatorCharacter->GArrowZ->GetRightVector(), FVector::ZeroVector);

				if (bShowDebugData)
				{
					PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection Forward Vector is UP")));
				}

				dProduct = dProduct > 0 ? 1 : -1;
				dProduct *= (InstigatorCharacter->GArrowZ->GetForwardVector().Z > 0 ? 1 : -1);
				MouseDirection = dProduct;

			}
			else if (FMath::Abs(InstigatorCharacter->GArrowZ->GetRightVector().Z) > FMath::Abs(InstigatorCharacter->GArrowZ->GetForwardVector().Z)) // Up is GarrowZ.R axis
			{
				dProduct = FVector::DotProduct(TouchPointDir, InstigatorCharacter->GArrowZ->GetForwardVector());

				if (bShowDebugLines)
					DrawDotProductLines(InstigatorCharacter, Touchpoint, InstigatorCharacter->GArrowZ->GetComponentLocation(), InstigatorCharacter->GArrowZ->GetForwardVector(), FVector::ZeroVector);

				if (bShowDebugData)
				{
					PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection Right Vector is UP")));
				}

				dProduct = dProduct < 0 ? 1 : -1;
				dProduct *= (InstigatorCharacter->GArrowZ->GetRightVector().Z > 0 ? 1 : -1);
				MouseDirection = dProduct;

			}
		}
		else
		{
			MouseDirection = -1;
		}
		break;
	case EGizmo::Yaw:

		// ROTATE only X axis

		dProductUU = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(), InstigatorCharacter->GArrowZ->GetUpVector());

		if (FMath::Abs(InstigatorCharacter->GArrowZ->GetUpVector().Z) < 0.5 && dProductUU < 0.8) // Object is Vertical and our Up vectors are Perpendicular
		{
			bForward = true;

			dProductUR = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(), InstigatorCharacter->GArrowY->GetRightVector());
			dProductUF = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(), InstigatorCharacter->GArrowX->GetForwardVector());

			if (bShowDebugLines)
			{
				DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowX->GetComponentLocation(), InstigatorCharacter->GArrowX->GetForwardVector(), InstigatorCharacter->GetActorUpVector());
				DrawDebugLine(InstigatorCharacter->GetWorld(), InstigatorCharacter->GArrowY->GetComponentLocation(), (InstigatorCharacter->GArrowY->GetComponentLocation() + (InstigatorCharacter->GArrowY->GetRightVector() * 500)), FColor(0, 255, 0), false, 10.f, 0.f, 5.f);
			}


			if ((dProductUR < 0) && (dProductUF < 0))
				dProduct = 1;
			else if ((dProductUR > 0) && (dProductUF > 0))
				dProduct = -1;
			else
				dProduct = -(FMath::Abs(dProductUF) > FMath::Abs(dProductUR) ? dProductUF : dProductUR);


			MouseDirection = dProduct > 0 ? 1 : -1;


			if (bShowDebugData)
			{
				PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, dProductUR, dProductUF);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection [VERTICAL && Up Vectors are not Parallel] MouseDirection = %f"), MouseDirection));
			}
		}
		else // Object is horizontal or Vertical but our Up vectors are Parallel
		{
			bForward = true;

			dProductFF = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowX->GetForwardVector());
			dProductFR = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowY->GetRightVector());
			// Not used
			dProductRF = FVector::DotProduct(InstigatorCharacter->GetActorRightVector(), InstigatorCharacter->GArrowY->GetForwardVector());

			if (bShowDebugLines)
			{
				DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowX->GetComponentLocation(), InstigatorCharacter->GArrowX->GetForwardVector(), InstigatorCharacter->GetActorForwardVector());
				DrawDebugLine(InstigatorCharacter->GetWorld(), InstigatorCharacter->GArrowY->GetComponentLocation(), (InstigatorCharacter->GArrowY->GetComponentLocation() + (InstigatorCharacter->GArrowY->GetRightVector() * 500)), FColor(0, 255, 0), false, 10.f, 0.f, 5.f);
			}

			if ((dProductFR < 0) && (dProductFF < 0))
				dProduct = 1;
			else if ((dProductFR > 0) && (dProductFF > 0))
				dProduct = -1;
			else
				dProduct = -(FMath::Abs(dProductFF) > FMath::Abs(dProductFR) ? dProductFF : dProductFR);


			MouseDirection = dProduct > 0 ? 1 : -1;
			MouseDirection *= UKismetMathLibrary::SignOfFloat(dProductUU);

			if (bShowDebugData)
			{
				PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, dProductFR, dProductFF, dProductRF, dProductUU);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection [HORIZONTAL or Vertical but our Up vectors are Parallel]  MouseDirection = %f"), MouseDirection));
			}
		}

		break;
	case EGizmo::Pitch:

		// ROTATE only X axis

		dProductFR = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowY->GetRightVector());

		dProductRF = FVector::DotProduct(InstigatorCharacter->GetActorRightVector(), InstigatorCharacter->GArrowX->GetForwardVector());
		dProductRU = FVector::DotProduct(InstigatorCharacter->GetActorRightVector(), InstigatorCharacter->GArrowZ->GetUpVector());

		dProductUU = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(), InstigatorCharacter->GArrowZ->GetUpVector());
		dProductUF = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(), InstigatorCharacter->GArrowX->GetForwardVector());

		if (dProductUU > 0 && dProductUU > dProductUF)
			dProduct = dProductFR * -1;
		else if (dProductUF > 0 && dProductUF > dProductUU)
			dProduct = dProductFR * 1;
		else if (dProductUU < 0)
			dProduct = FMath::Abs(dProductRF) > FMath::Abs(dProductRU) ? dProductRF : dProductRU;

		MouseDirection = dProduct > 0 ? 1 : -1;

		if (bShowDebugLines)
		{
			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowZ->GetComponentLocation(), InstigatorCharacter->GArrowZ->GetUpVector(),      InstigatorCharacter->GetActorUpVector()); // UU
			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowX->GetComponentLocation(), InstigatorCharacter->GArrowX->GetForwardVector(), InstigatorCharacter->GetActorUpVector()); // UF

			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowY->GetComponentLocation(), InstigatorCharacter->GArrowY->GetRightVector(),   InstigatorCharacter->GetActorForwardVector()); // FR

			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowX->GetComponentLocation(), InstigatorCharacter->GArrowX->GetForwardVector(), InstigatorCharacter->GetActorRightVector()); // RF
			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowZ->GetComponentLocation(), InstigatorCharacter->GArrowZ->GetUpVector(),      InstigatorCharacter->GetActorRightVector()); // RU
		}

		if (bShowDebugData)
		{
			PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, dProductUU, dProductUF, dProductFR, dProductRF, dProductRU);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection MouseDirection = %f"), MouseDirection));
		}


		break;
	case EGizmo::Roll:

		// ROTATE only X axis

		dProductFF = FVector::DotProduct(InstigatorCharacter->GetActorForwardVector(), InstigatorCharacter->GArrowX->GetForwardVector());

		dProductUU = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(), InstigatorCharacter->GArrowZ->GetUpVector());
		dProductUR = FVector::DotProduct(InstigatorCharacter->GetActorUpVector(), InstigatorCharacter->GArrowY->GetRightVector());

		dProductRR = FVector::DotProduct(InstigatorCharacter->GetActorRightVector(), InstigatorCharacter->GArrowY->GetRightVector());
		dProductRU = FVector::DotProduct(InstigatorCharacter->GetActorRightVector(), InstigatorCharacter->GArrowZ->GetUpVector());

		if (dProductUU > 0 && dProductUU > dProductUR)
			dProduct = dProductFF * -1;
		else if (dProductUR > 0 && dProductUR > dProductUU)
			dProduct = dProductFF * 1;
		else if (dProductUU < 0)
			dProduct = -(FMath::Abs(dProductRR) > FMath::Abs(dProductRU) ? dProductRR : dProductRU);

		MouseDirection = dProduct > 0 ? 1 : -1;

		if (bShowDebugData)
		{
			PrintDebugData(InstigatorCharacter, GizmoTouch, dProduct, dProductUU, dProductUR, dProductFF, dProductRR, dProductRU);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UGizmoMathLibrary::GetGizmoAxisDirection MouseDirection = %f"), MouseDirection));
		}

		if (bShowDebugLines)
		{
			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowZ->GetComponentLocation(), InstigatorCharacter->GArrowZ->GetUpVector(),    InstigatorCharacter->GetActorUpVector()); // UU
			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowY->GetComponentLocation(), InstigatorCharacter->GArrowY->GetRightVector(), InstigatorCharacter->GetActorUpVector()); // UR

			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowX->GetComponentLocation(), InstigatorCharacter->GArrowX->GetForwardVector(), InstigatorCharacter->GetActorForwardVector()); // FF

			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowY->GetComponentLocation(), InstigatorCharacter->GArrowY->GetRightVector(), InstigatorCharacter->GetActorRightVector()); // RR
			DrawDotProductLines(InstigatorCharacter, FVector::ZeroVector, InstigatorCharacter->GArrowZ->GetComponentLocation(), InstigatorCharacter->GArrowZ->GetUpVector(),    InstigatorCharacter->GetActorRightVector()); // RU
		}



		break;
	case EGizmo::None:
		break;
	}


	GizmoTouchData.GizmoAxisDirection = MouseDirection;
	GizmoTouchData.IsAxisForward = bForward;
	GizmoTouchData.IsDataValid = true;

}


FQuat UGizmoMathLibrary::GetGizmoQuat(EGizmo Axis, float roll, float pitch, float yaw)
{
	FQuat Delta;
	FVector YawAxis = FVector(0.f, 0.f, 1.f);
	FVector RollAxis = FVector(1.f, 0.f, 0.f);
	FVector PitchAxis = FVector(0.f, 1.f, 0.f);
	switch (Axis)
	{
	case EGizmo::Pitch:
		Delta = FQuat(PitchAxis, FMath::DegreesToRadians(roll)) * FQuat(PitchAxis, FMath::DegreesToRadians(pitch)) * FQuat(YawAxis, FMath::DegreesToRadians(yaw));

		break;
	case EGizmo::Roll:
		Delta = FQuat(RollAxis, FMath::DegreesToRadians(roll)) * FQuat(PitchAxis, FMath::DegreesToRadians(pitch)) * FQuat(YawAxis, FMath::DegreesToRadians(yaw));
		break;
	case EGizmo::Yaw:
		Delta = FQuat(YawAxis, FMath::DegreesToRadians(roll)) * FQuat(PitchAxis, FMath::DegreesToRadians(pitch)) * FQuat(YawAxis, FMath::DegreesToRadians(yaw));
		break;
	case EGizmo::None:
		break;
	}
	return Delta;
}


float UGizmoMathLibrary::GetAutoSnappingRate(EGizmo TouchAxis, AActor* SnappingActor, bool bDrawDebugBox)
{

	AGizmoActorBase* GizmoBase = Cast<AGizmoActorBase>(SnappingActor);
	if(!GizmoBase) return -1;

	FVector Min;
	FVector Max;

	if(!GizmoBase->GetStaticMeshComponent()) return -1;

	GizmoBase->GetStaticMeshComponent()->GetLocalBounds(OUT Min, OUT Max);

	if (bDrawDebugBox)
	{
		UKismetSystemLibrary::DrawDebugBox(SnappingActor, SnappingActor->GetActorLocation(), Max, FLinearColor::Black, SnappingActor->GetActorRotation(), 0.1, 12.f);
	}

	float SnappingRate;
	switch (TouchAxis)
	{
	case EGizmo::X:
		SnappingRate = Max.X * 2;
		break;
	case EGizmo::Y:
		SnappingRate = Max.Y * 2;
		break;
	case EGizmo::Z:
		SnappingRate = Max.Z * 2;
		break;
	case EGizmo::None:
	default:
		SnappingRate = -1;
		break;
	}

	return SnappingRate;

}

void UGizmoMathLibrary::DrawDotProductLines(AGizmoCharacter* InstigatorCharacter, FVector TouchPoint, FVector TouchArrowLocation, FVector ArrowUnitVector, FVector MMUnitVector)
{
	FVector ArrowStart = TouchArrowLocation;
	FVector ArrowEnd = ArrowStart + (UKismetMathLibrary::GetDirectionUnitVector(ArrowStart, ArrowStart + ArrowUnitVector) * 500);

	DrawDebugLine(InstigatorCharacter->GetWorld(), ArrowStart, ArrowEnd, FColor(255, 0, 0), false, 10.f, 0.f, 5.f);

	FVector MMLocation = InstigatorCharacter->GetActorLocation();
	if (TouchPoint != FVector::ZeroVector)
	{
		FVector TouchPointDir = UKismetMathLibrary::GetDirectionUnitVector(MMLocation, TouchPoint);
		FVector TouchVector = MMLocation + (TouchPointDir * 500);
		DrawDebugLine(InstigatorCharacter->GetWorld(), MMLocation, TouchVector, FColor(0, 255, 0), false, 10.f, 0.f, 5.f);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("UGizmoMathLibrary::DrawDotProductLines TouchPoint Part -- GArrowX Rotation = %s"), *InstigatorCharacter->GArrowX->GetComponentRotation().ToString()));
	}
	else if (MMUnitVector != FVector::ZeroVector)
	{
		//FVector MMForwardVector = MMLocation + (UKismetMathLibrary::GetDirectionUnitVector(MMLocation, MMLocation + InstigatorCharacter->GetActorForwardVector()) * 500);
		FVector MMEndVector = MMLocation + (UKismetMathLibrary::GetDirectionUnitVector(MMLocation, MMLocation + MMUnitVector) * 500);
		DrawDebugLine(InstigatorCharacter->GetWorld(), MMLocation, MMEndVector, FColor(0, 255, 255), false, 10.f, 0.f, 5.f);
	}

}

void UGizmoMathLibrary::PrintDebugData(AGizmoCharacter* InstigatorCharacter, EGizmo GizmoState, float dProduct1, float dProduct2 /*= 0.f*/, float dProduct3 /*= 0.f*/, float dProduct4 /*= 0*/, float dProduct5 /*= 0*/, float dProduct6 /*= 0*/)
{
	switch (GizmoState)
	{
	case EGizmo::X:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductFF = %f"), dProduct2));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,  FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductRF = %f"), dProduct3));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black,  FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductUU = %f"), dProduct4));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductFR = %f"), dProduct5));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,  FString::Printf(TEXT("==============================================================")));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProduct = %f"), dProduct1));
		break;
	case EGizmo::Y:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductRR = %f"), dProduct2));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,  FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductFR = %f"), dProduct3));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black,  FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductUU = %f"), dProduct4));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductFF = %f"), dProduct5));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,  FString::Printf(TEXT("==============================================================")));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProduct = %f"), dProduct1));
		break;
	case EGizmo::Z:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,   FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProduct = %f"), dProduct1));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData GArrowZ Rotation = %s"), *InstigatorCharacter->GArrowZ->GetComponentRotation().ToString()));
		break;
	case EGizmo::Pitch:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProduct = %f"), dProduct1));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductUU = %f,  dProductUF = %f    dProductFR = %f"), dProduct2, dProduct3, dProduct4));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductRF = %f,  dProductRU = %f"), dProduct5, dProduct6));
		break;
	case EGizmo::Roll:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProduct = %f"), dProduct1));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductUU = %f,  dProductUR = %f    dProductFF = %f"), dProduct2, dProduct3, dProduct4));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData dProductRR = %f,  dProductRU = %f,   MouseDirection = %f"), dProduct5, dProduct6));
		break;
	case EGizmo::Yaw:
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,    FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData result dProduct = %f"), dProduct1));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData  dProductUR || FR = %f"), dProduct2));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData  dProductUF || FF = %f"), dProduct3));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData  RF = %f"), dProduct4));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData  UU = %f"), dProduct5));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,  FString::Printf(TEXT("UGizmoMathLibrary::PrintDebugData GYaw Rotation = %s"), *InstigatorCharacter->GYaw->GetComponentRotation().ToString()));
		break;
	default:
		break;
	}
}

bool UGizmoMathLibrary::IsArrowForward(FVector VNormal, bool bShowDebugData /* false */)
{
	if (FMath::Abs(VNormal.Z) > FMath::Abs(VNormal.X) && FMath::Abs(VNormal.Z) > FMath::Abs(VNormal.Y))
	{
		if (bShowDebugData)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("UGizmoMathLibrary::IsArrowForward Arrow is UP = %s"), *VNormal.ToString()));

		return false;
	}
	else
	{
		if (bShowDebugData)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("UGizmoMathLibrary::IsArrowForward Arrow is Forward = %s"), *VNormal.ToString()));

		return true;
	}
}

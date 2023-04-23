// Fill out your copyright notice in the Description page of Project Settings.


#include "Gizmo/Player/Components/GizmoComponent.h"
#include "Gizmo/Library/GizmoMathLibrary.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UGizmoComponent::UGizmoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}




// Called when the game starts
void UGizmoComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCharacter = Cast<AGizmoCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->GetGizmoTool().SetGizmoToolVisibility(OwnerCharacter->bDefaultVisibleGizmo);
		SetGizmoMaterial();
		OwnerController = Cast<APlayerController>(OwnerCharacter->GetController());
	}
	
}



// Called every frame
void UGizmoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UGizmoComponent::CanGizmoAttach(const FHitResult& Hit)
{
	if(!Hit.GetActor()) return false;
	
	AActor* GActorOwner = Hit.GetActor()->GetOwner();
	if (Hit.bBlockingHit && Hit.GetActor() && (GActorOwner == NULL || GActorOwner == OwnerCharacter))
		return true;
	else
		return false;
}




void UGizmoComponent::GizmoTrace()
{
	if(!OwnerCharacter) return;

	//Do a line trace to find something to pickup
	FVector Start = OwnerCharacter->GetActorLocation();
	Start += GizmoTraceOffset;
	FVector End = Start + (OwnerCharacter->GetControlRotation().Quaternion().GetForwardVector() * GizmoTraceDistance);
	FCollisionQueryParams Params = FCollisionQueryParams();
	Params.AddIgnoredActor(OwnerCharacter);
	FCollisionResponseParams RParams = FCollisionResponseParams();
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, Params, RParams);

	if (bGizmoTraceVisible)
	{
		DrawDebugLine(OwnerCharacter->GetWorld(), Start, End, FColor(255, 0, 0), false, 10.f, 0.f, 5.f);
	}

	if (bDebugText)
	{
		if (Hit.GetActor())
		{
			UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::GizmoTrace Hit Actor = %s"), *Hit.GetActor()->GetName());
		}
		else if (Hit.bBlockingHit)
		{
			UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::GizmoTrace Hit Location = %s"), *Hit.Location.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::GizmoTrace Hit None"));
		}
	}

	if (CanGizmoAttach(Hit))
	{
		if (OwnerCharacter->IsControlPressed() && OwnerCharacter->GetGizmoActor())
		{
			OwnerCharacter->SetOtherGizmoActors(Hit.GetActor());
		}
		else
		{
			if (OwnerCharacter->GetAttachedGizmoActors().Num() > 0)
			{
				for (AActor* AttachedGizmoActor : OwnerCharacter->GetAttachedGizmoActors())
				{
					// Remove Attached actors
					OwnerCharacter->SetOtherGizmoActors(AttachedGizmoActor);
				}
			}
			EGizmoActiveStatus GActive = OwnerCharacter->GetGizmoActor() ? EGizmoActiveStatus::Change : EGizmoActiveStatus::Attached;
			OwnerCharacter->SetMainGizmoActor(Hit.GetActor(), GActive);
		}
		
	}
	else
	{
		if (OwnerCharacter->GetGizmoActor())
		{
			if (UKismetSystemLibrary::IsServer(OwnerCharacter) && OwnerCharacter->IsLocallyControlled())
			{
				PressedGizmoTool();
			}
			else
			{
				OwnerCharacter->CL_PressedGizmoTool();
			}
		}

		//OwnerCharacter->SetGizmoActor(NULL);
	}

}

void UGizmoComponent::SetGizmoActorSettings(bool On_Off, AActor* GActor /* NULL */)
{
	if (GActor == NULL)
	{
		if(!OwnerCharacter->GetGizmoActor()) return;
		if (On_Off)
		{
			OwnerCharacter->GetGizmoActor()->SetReplicateMovement(true);
			OwnerCharacter->GetGizmoActor()->SetReplicates(true);
			OwnerCharacter->GetGizmoActor()->SetOwner(OwnerCharacter);
			OwnerCharacter->GetGizmoActor()->SetNetDormancy(DORM_Awake);
		}
		else
		{
			OwnerCharacter->GetGizmoActor()->SetReplicateMovement(false);
			OwnerCharacter->GetGizmoActor()->SetReplicates(false);
			OwnerCharacter->GetGizmoActor()->SetOwner(NULL);
			OwnerCharacter->GetGizmoActor()->SetNetDormancy(DORM_DormantAll);
		}

		SetGizmoActorCollisionResponse(On_Off, OwnerCharacter->GetGizmoActor());
	}
	else
	{
		if (On_Off)
		{
			GActor->SetReplicateMovement(true);
			GActor->SetReplicates(true);
			GActor->SetOwner(OwnerCharacter);
			GActor->SetNetDormancy(DORM_Awake);
		}
		else
		{
			GActor->SetReplicateMovement(false);
			GActor->SetReplicates(false);
			GActor->SetOwner(NULL);
			GActor->SetNetDormancy(DORM_DormantAll);
		}
	}
}

void UGizmoComponent::AttachDetachGizmo(bool bAttach /* true */)
{
	if(!OwnerCharacter->GetGizmoActor() && !OwnerCharacter->GetGizmoTool().IsGizmoValid()) return;

	if (bAttach && OwnerCharacter->GetGizmoActor())
	{
		OwnerCharacter->GetGizmoTool().GetGizmoPivot()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		OwnerCharacter->GetGizmoTool().GetGizmoPivot()->SetWorldTransform(OwnerCharacter->GetGizmoActor()->GetTransform());

		OwnerCharacter->GetGizmoTool().GetGizmoPivot()->AttachToComponent(OwnerCharacter->GetGizmoActor()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
	else
	{
		OwnerCharacter->GetGizmoTool().GetGizmoPivot()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		OwnerCharacter->GetGizmoTool().GetGizmoPivot()->SetWorldTransform(OwnerCharacter->GetTransform());

		OwnerCharacter->GetGizmoTool().GetGizmoPivot()->AttachToComponent(OwnerCharacter->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
}



void UGizmoComponent::MakeGizmoActorTranslucent(bool OnOff, AActor* GActor)
{

	/*if (OnOff && GActor)
	{
		SM_GizmoActor = GActor->FindComponentByClass<UStaticMeshComponent>();
		if (SM_GizmoActor)
		{

			SM_GizmoActor->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			SM_GizmoActor->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		}
	}
	else
	{
		if (SM_GizmoActor)
		{
			SM_GizmoActor->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			SM_GizmoActor->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
			SM_GizmoActor = NULL;
		}
	}*/

	SM_GizmoActor = SetGizmoActorCollisionResponse(OnOff, GActor);


	MakeTranslucent(OnOff, SM_GizmoActor, OUT DM_GizmoActor);
}


UStaticMeshComponent* UGizmoComponent::SetGizmoActorCollisionResponse(bool OnOff, AActor* GActor)
{
	if(!GActor) return NULL;

	UStaticMeshComponent* GMesh = NULL;
	if (OnOff)
	{
		GMesh = GActor->FindComponentByClass<UStaticMeshComponent>();

		GMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		GMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	}
	else
	{
		GMesh = GActor->FindComponentByClass<UStaticMeshComponent>();

		GMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		GMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

		GMesh = NULL;
	}

	bool isValid = GMesh ? true : false;
	UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::SetGizmoActorCollisionResponse GMesh = %i"), isValid);

	return GMesh;
}



void UGizmoComponent::MakeGizmoActorTranslucent(bool OnOff, AActor* GActor, TArray<UMaterialInstanceDynamic*>& DM_Materials)
{
	if(!GActor) return;

	UStaticMeshComponent* SM = GActor->FindComponentByClass<UStaticMeshComponent>();

	if(!SM) return;

	MakeTranslucent(OnOff, SM, OUT DM_Materials);
}


void UGizmoComponent::MakeTranslucent(bool OnOff, UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material)
{
	if (OnOff && GizmoMesh)
	{

		GetGizmoActorDM(GizmoMesh, OUT DM_Material);
		if (DM_Material.Num() > 0)
		{
			for (UMaterialInstanceDynamic* DMItem : DM_Material)
			{
				DMItem->SetScalarParameterValue(FName("Translucent"), GizmoActorTranslucent);
			}
		}
	}
	else
	{
		if (DM_Material.Num() > 0)
		{
			for (UMaterialInstanceDynamic* DMItem : DM_Material)
			{
				DMItem->SetScalarParameterValue(FName("Translucent"), 1.f);

			}
			DM_Material.Empty();
		}
	}
	bool isValid = GizmoMesh ? true : false;
	UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::MakeObjectTranslucent DM_GizmoActor Num = %i, GMesh = %i"), DM_GizmoActor.Num(), isValid);
}



void UGizmoComponent::GetGizmoActorDM(UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material)
{
	if (!GizmoMesh) return;

	TArray<UMaterialInterface*> MI = GizmoMesh->GetMaterials();

	int32 index = 0;
	for (auto MItem : MI)
	{
		UMaterialInstanceDynamic* DM = GizmoMesh->CreateAndSetMaterialInstanceDynamicFromMaterial(index, GizmoMesh->GetMaterial(index));
		DM_Material.AddUnique(DM);
		++index;
		//UE_LOG(LogTemp, Error, TEXR("UGizmoComponent::GetGizmoActorDM"));
	}

}





void UGizmoComponent::AttachDeatachActorToGizmoActor(AActor* OtherGizmoActor)
{

	if(!OtherGizmoActor) return;

	if (TM_OtherGizmoActor.Contains(OtherGizmoActor))
	{
		FGizmoActorProperty* FG = TM_OtherGizmoActor.Find(OtherGizmoActor);
		MakeGizmoActorTranslucent(false, OtherGizmoActor, FG->DM_OtherActor);
		OtherGizmoActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		FG->ClearData();
		TM_OtherGizmoActor.Remove(OtherGizmoActor);

		if(TM_OtherGizmoActor.Num() == 0)
			TM_OtherGizmoActor.Empty();
	}
	else
	{
		OtherGizmoActor->AttachToComponent(OwnerCharacter->GetGizmoActor()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

		TArray<UMaterialInstanceDynamic*> DM;
		MakeGizmoActorTranslucent(true, OtherGizmoActor, OUT DM);

		FGizmoActorProperty GProperty;
		GProperty.DM_OtherActor = DM;
		GProperty.OtherGizmoObject = OtherGizmoActor;
		TPair<AActor*, FGizmoActorProperty> Pair(OtherGizmoActor, GProperty);
		TM_OtherGizmoActor.Add(Pair);
	}
	UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::AttachDeatachActorToGizmoActor OtherGizmoActors Num = %i"), TM_OtherGizmoActor.Num());
}

void UGizmoComponent::SetGizmoInputMode(bool IsGizmoActive)
{
	if (IsGizmoActive)
	{
		FInputModeGameAndUI Mode;
		Mode.SetHideCursorDuringCapture(false);
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		OwnerController->SetInputMode(Mode);
		OwnerController->bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly Mode;
		OwnerController->SetInputMode(Mode);
		OwnerController->bShowMouseCursor = false;
	}
}

void UGizmoComponent::PressedGizmoTool()
{
	FHitResult Hit;
	CoursorTrace(Hit);

	if (Hit.GetComponent() && Hit.GetComponent()->GetAttachmentRootActor() && Hit.GetComponent()->GetAttachmentRootActor() == OwnerCharacter->GetGizmoActor())
	{

	}

	GizmoTouch = OwnerCharacter->GetGizmoTool().GetGizmoTouch(Hit.GetComponent());
	ActivateGizmo(GizmoTouch, true);

	UGizmoMathLibrary::GetGizmoAxisDirection(GizmoTouch, Hit.ImpactPoint, OwnerCharacter, OUT GizmoMovementData, bDebugText, bGizmoTraceVisible);
	GizmoMovementData.GizmoLocation = OwnerCharacter->GetGizmoActor()->GetActorLocation();
	GizmoMovementData.GizmoQuat = OwnerCharacter->GetGizmoActor()->GetActorTransform().GetRotation();

	OwnerCharacter->CanUpdateGizmoActorTransform = true;

	if (GizmoTouch != EGizmo::None)
	{
		OwnerCharacter->BP_GizmoTouch(true);
	}

}

void UGizmoComponent::ReleasedGizmoTool()
{
	OwnerCharacter->CanUpdateGizmoActorTransform = false;
	ActivateGizmo(GizmoTouch, false);
	GizmoTouch = EGizmo::None;
	GizmoMovementData.ClearData();
	OwnerCharacter->BP_GizmoTouch(false);
}

void UGizmoComponent::UpdatedGizmoActorTransform(float DeltaTime)
{
	if(!OwnerCharacter) return;

	bool bPressing;
	float moveStep;
	bPressing = OwnerController->GetMousePosition(OUT GizmoMovementData.UpdateMouseTouch.X, OUT GizmoMovementData.UpdateMouseTouch.Y);

	FString EnumString = "EGizmo";
	FString EnumValue = GetEnumString(EnumString, (uint8)GizmoTouch);
	UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::UpdatedGizmoActorTransform GizmoTouch = %s"), *EnumValue);

	switch (GizmoTouch)
	{
	case EGizmo::X:
		
		if(GizmoMovementData.IsAxisForward)
			GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.X, MouseTouchPoint.X);
		else
			GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.Y, MouseTouchPoint.Y);
			
		moveStep = GetMoveStep(GizmoTouch, DeltaTime, GizmoMovementData.GizmoAxisDirection, GizmoMovementData.MouseUpdateDirection);
		GizmoMovementData.GizmoLocation += moveStep * OwnerCharacter->GetGizmoActor()->GetActorRotation().Quaternion().GetForwardVector();
		GizmoMovementData.GizmoTransform = OwnerCharacter->GetGizmoActor()->GetTransform();
		GizmoMovementData.GizmoTransform.SetLocation(GizmoMovementData.GizmoLocation);
		SR_UpdateGizmoActorTransform(GizmoMovementData.GizmoTransform);

		break;
	case EGizmo::Y:
		
		if (GizmoMovementData.IsAxisForward)
			GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.X, MouseTouchPoint.X);
		else
			GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.Y, MouseTouchPoint.Y);

		moveStep = GetMoveStep(GizmoTouch, DeltaTime, GizmoMovementData.GizmoAxisDirection, GizmoMovementData.MouseUpdateDirection);
		GizmoMovementData.GizmoLocation += moveStep * OwnerCharacter->GetGizmoActor()->GetActorRotation().Quaternion().GetRightVector();
		GizmoMovementData.GizmoTransform = OwnerCharacter->GetGizmoActor()->GetTransform();
		GizmoMovementData.GizmoTransform.SetLocation(GizmoMovementData.GizmoLocation);
		SR_UpdateGizmoActorTransform(GizmoMovementData.GizmoTransform);

		break;
	case EGizmo::Z:

		if (GizmoMovementData.IsAxisForward)
			GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.X, MouseTouchPoint.X);
		else
			GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.Y, MouseTouchPoint.Y);

		moveStep = GetMoveStep(GizmoTouch, DeltaTime, GizmoMovementData.GizmoAxisDirection, GizmoMovementData.MouseUpdateDirection);
		GizmoMovementData.GizmoLocation += moveStep * OwnerCharacter->GetGizmoActor()->GetActorRotation().Quaternion().GetUpVector();
		GizmoMovementData.GizmoTransform = OwnerCharacter->GetGizmoActor()->GetTransform();
		GizmoMovementData.GizmoTransform.SetLocation(GizmoMovementData.GizmoLocation);
		SR_UpdateGizmoActorTransform(GizmoMovementData.GizmoTransform);

		break;
	case EGizmo::Pitch:

		GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.X, MouseTouchPoint.X);

		moveStep = GetMoveStep(GizmoTouch, DeltaTime, GizmoMovementData.GizmoAxisDirection, GizmoMovementData.MouseUpdateDirection);
		GizmoMovementData.GizmoQuat *= UGizmoMathLibrary::GetGizmoQuat(GizmoTouch, 0.f, moveStep, 0.f);
		GizmoMovementData.GizmoTransform = OwnerCharacter->GetGizmoActor()->GetTransform();
		GizmoMovementData.GizmoTransform.SetRotation(GizmoMovementData.GizmoQuat);
		SR_UpdateGizmoActorTransform(GizmoMovementData.GizmoTransform);
		
		break;
	case EGizmo::Roll:
		
		GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.X, MouseTouchPoint.X);

		moveStep = GetMoveStep(GizmoTouch, DeltaTime, GizmoMovementData.GizmoAxisDirection, GizmoMovementData.MouseUpdateDirection);
		GizmoMovementData.GizmoQuat *= UGizmoMathLibrary::GetGizmoQuat(GizmoTouch, moveStep, 0.f, 0.f);
		GizmoMovementData.GizmoTransform = OwnerCharacter->GetGizmoActor()->GetTransform();
		GizmoMovementData.GizmoTransform.SetRotation(GizmoMovementData.GizmoQuat);
		SR_UpdateGizmoActorTransform(GizmoMovementData.GizmoTransform);

		break;
	case EGizmo::Yaw:

		GizmoMovementData.MouseUpdateDirection = UpdateMousePosition(GizmoMovementData.UpdateMouseTouch.X, MouseTouchPoint.X);

		moveStep = GetMoveStep(GizmoTouch, DeltaTime, GizmoMovementData.GizmoAxisDirection, GizmoMovementData.MouseUpdateDirection);
		GizmoMovementData.GizmoQuat *= UGizmoMathLibrary::GetGizmoQuat(GizmoTouch, 0.f, 0.f, moveStep);
		GizmoMovementData.GizmoTransform = OwnerCharacter->GetGizmoActor()->GetTransform();
		GizmoMovementData.GizmoTransform.SetRotation(GizmoMovementData.GizmoQuat);
		SR_UpdateGizmoActorTransform(GizmoMovementData.GizmoTransform);

		break;
	case EGizmo::None:
	default:
		break;
	}
}


void UGizmoComponent::DeleteGizmoActor()
{
	SM_GizmoActor = NULL;
	DM_GizmoActor.Empty();

	ActivateGizmo(GizmoTouch, false);
	GizmoTouch = EGizmo::None;
	GizmoMovementData.ClearData();

	if (TM_OtherGizmoActor.Num() > 0)
	{
		TArray<AActor*> GActorKeys;
		TM_OtherGizmoActor.GetKeys(GActorKeys);
		for (AActor* GKey : GActorKeys)
		{
			FGizmoActorProperty* Object = TM_OtherGizmoActor.Find(GKey);
			Object->ClearData();
			TM_OtherGizmoActor.Remove(GKey);
		}
		TM_OtherGizmoActor.Empty();
	}

}

void UGizmoComponent::RemoveAllAttachedGizmoActor()
{
	if(TM_OtherGizmoActor.Num() < 1) return;

	TArray<AActor*> Keys;
	TM_OtherGizmoActor.GetKeys(Keys);
	for (AActor* GItem : Keys)
	{
		RemoveAttachedGizmoActor(GItem);
	}
}


void UGizmoComponent::RemoveAttachedGizmoActor(AActor* GActor)
{
	if (!GActor) return;

	// Detach from Server
	SR_RemoveAttachedGizmoActor(GActor);

	// Detach on Client side
	AttachDeatachActorToGizmoActor(GActor);

}


void UGizmoComponent::SR_RemoveAttachedGizmoActor_Implementation(AActor* GActor)
{
	if(!GActor) return;
	GActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetGizmoActorSettings(false, GActor);
}

void UGizmoComponent::SR_UpdateGizmoActorTransform_Implementation(const FTransform& NewTransform)
{
	OwnerCharacter->GetGizmoActor()->SetActorTransform(NewTransform);
}


void UGizmoComponent::CoursorTrace(FHitResult& Hit)
{
	if(!OwnerCharacter) return;

	bool bPressed;

	if(!OwnerController)
		OwnerController = Cast<APlayerController>(OwnerCharacter->GetController());

	bPressed = OwnerController->GetMousePosition(OUT MouseTouchPoint.X, OUT MouseTouchPoint.Y);
	OwnerController->DeprojectScreenPositionToWorld(MouseTouchPoint.X, MouseTouchPoint.Y, OUT TouchLocation, OUT TouchDirection);

	FVector Start = TouchLocation;
	FVector End = TouchLocation + (TouchDirection * GizmoTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, QueryParams);

	GizmoMovementData.UpdateMouseTouch = MouseTouchPoint;

	if (Hit.GetComponent())
	{
		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::CoursorTrace Touch Axis = %s"), *Hit.GetComponent()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::CoursorTrace Not Found Gizmo"));
	}

	if (bGizmoTraceVisible)
		DrawDebugLine(GetWorld(), Start, End, FColor(0, 255, 0), false, 10.f, 0.f, 5.f);
}




float UGizmoComponent::UpdateMousePosition(float CurrentPixel, float PassedPixel)
{
	// Sum Snapping each mouse Update
	SnappingLocation += FMath::Abs(CurrentPixel - PassedPixel);
	//PG_LOG(Error, TEXT("UGizmoComponent::UpdateMousePosition UpldateMousePixel = %f, MouseTouchPixel = %f U - M = %f SnappingLocation = %f"), UpldateMousePixel, MouseTouchPixel, (UpldateMousePixel - MouseTouchPixel), SnappingLocation);
	float UMD = 0;
	if (CurrentPixel > PassedPixel)
	{
		UMD = 1;
		MouseTouchPoint = GizmoMovementData.UpdateMouseTouch;
	}
	else if (CurrentPixel < PassedPixel)
	{
		UMD = -1;
		MouseTouchPoint = GizmoMovementData.UpdateMouseTouch;
	}
	else
	{
		UMD = 0;
		MouseTouchPoint = GizmoMovementData.UpdateMouseTouch;
	}

	return UMD;
}



float UGizmoComponent::GetMoveStep(EGizmo TouchAxis, float DeltaTime, float MouseDirection, float AxisDirection)
{
	float movestep = 0.f;
	movestep = (MoveRate * AxisDirection * MouseDirection * DeltaTime) * MovePower;
	return movestep;
}



FString UGizmoComponent::GetEnumString(const FString& EnumString, uint8 EnemElement)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumString, true);
	if (!EnumPtr) return FString("Invalid");
	return EnumPtr->GetNameStringByIndex(EnemElement);
}

/************* GizmoTool Dynamic Material ****************/


void UGizmoComponent::SetGizmoMaterial()
{
	MD_GizmoX = OwnerCharacter->GArrowX->CreateAndSetMaterialInstanceDynamicFromMaterial(0, OwnerCharacter->GArrowX->GetMaterial(0));
	MD_GizmoY = OwnerCharacter->GArrowY->CreateAndSetMaterialInstanceDynamicFromMaterial(0, OwnerCharacter->GArrowY->GetMaterial(0));
	MD_GizmoZ = OwnerCharacter->GArrowZ->CreateAndSetMaterialInstanceDynamicFromMaterial(0, OwnerCharacter->GArrowZ->GetMaterial(0));
	MD_GizmoPitch = OwnerCharacter->GPitch->CreateAndSetMaterialInstanceDynamicFromMaterial(0, OwnerCharacter->GPitch->GetMaterial(0));
	MD_GizmoRoll = OwnerCharacter->GRoll->CreateAndSetMaterialInstanceDynamicFromMaterial(0, OwnerCharacter->GRoll->GetMaterial(0));
	MD_GizmoYaw = OwnerCharacter->GYaw->CreateAndSetMaterialInstanceDynamicFromMaterial(0, OwnerCharacter->GYaw->GetMaterial(0));

	UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::SetGizmoMaterial"));
}


void UGizmoComponent::ActivateGizmo(EGizmo ActiveTouch, bool DoActive)
{

	float fValue = DoActive ? 1.f : 0.f;
	switch (ActiveTouch)
	{
	case EGizmo::X:

		if (MD_GizmoX)
			MD_GizmoX->SetScalarParameterValue(FName("Flickering"), fValue);

		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::ActivateGizmo Pressed W"));
		
		break;
	case EGizmo::Y:
		
		if (MD_GizmoY)
			MD_GizmoY->SetScalarParameterValue(FName("Flickering"), fValue);

		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::ActivateGizmo Pressed Y"));

		break;
	case EGizmo::Z:
		
		if (MD_GizmoZ)
			MD_GizmoZ->SetScalarParameterValue(FName("Flickering"), fValue);

		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::ActivateGizmo Pressed Z"));
		break;
	case EGizmo::Pitch:
		
		if (MD_GizmoPitch)
			MD_GizmoPitch->SetScalarParameterValue(FName("Flickering"), fValue);

		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::ActivateGizmo Pressed Pitch"));
		break;
	case EGizmo::Roll:
		
		if (MD_GizmoRoll)
			MD_GizmoRoll->SetScalarParameterValue(FName("Flickering"), fValue);

		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::ActivateGizmo Pressed Roll"));
		break;
	case EGizmo::Yaw:
		
		if (MD_GizmoYaw)
			MD_GizmoYaw->SetScalarParameterValue(FName("Flickering"), fValue);

		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::ActivateGizmo Pressed Yaw"));
		break;
	case EGizmo::None:
		UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::ActivateGizmo Pressed None"));
		break;
	}

}
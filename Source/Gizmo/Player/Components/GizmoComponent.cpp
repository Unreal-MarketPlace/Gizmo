// Fill out your copyright notice in the Description page of Project Settings.


#include "Gizmo/Player/Components/GizmoComponent.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UGizmoComponent::UGizmoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
	if (Hit.bBlockingHit && Hit.GetActor() && !Hit.GetActor()->GetOwner())
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
		OwnerCharacter->SetGizmoActor(Hit.GetActor());
	}
	else
	{
		if (OwnerCharacter->GetGizmoActor())
		{
			if (UKismetSystemLibrary::IsServer(OwnerCharacter))
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

void UGizmoComponent::SetGizmoActorSettings(bool On_Off)
{
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
}

void UGizmoComponent::AttachGizmo()
{
	if(!OwnerCharacter->GetGizmoActor() && !OwnerCharacter->GetGizmoTool().IsGizmoValid()) return;

	OwnerCharacter->GetGizmoTool().GetGizmoPivot()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	OwnerCharacter->GetGizmoTool().GetGizmoPivot()->SetWorldTransform(OwnerCharacter->GetGizmoActor()->GetTransform());

	OwnerCharacter->GetGizmoTool().GetGizmoPivot()->AttachToComponent(OwnerCharacter->GetGizmoActor()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
}



void UGizmoComponent::MakeGizmoActorTranslucent(bool OnOff, AActor* GActor)
{
	if (OnOff && GActor)
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
		}
	}

	MakeTranslucent(OnOff, SM_GizmoActor, OUT DM_GizmoActor);
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
}

void UGizmoComponent::ReleasedGizmoTool()
{
	ActivateGizmo(GizmoTouch, false);
	GizmoTouch = EGizmo::None;
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




void UGizmoComponent::MakeTranslucent(bool OnOff, UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material)
{
	if (OnOff && GizmoMesh)
	{

		GetGizmoActorDM(GizmoMesh, OUT DM_Material);
		if (DM_GizmoActor.Num() > 0)
		{
			for (UMaterialInstanceDynamic* DMItem : DM_GizmoActor)
			{
				DMItem->SetScalarParameterValue(FName("Translucent"), GizmoActorTranslucent);
			}
		}
	}
	else
	{
		if (DM_GizmoActor.Num() > 0)
		{
			for (UMaterialInstanceDynamic* DMItem : DM_GizmoActor)
			{
				DMItem->SetScalarParameterValue(FName("Translucent"), 1.f);

			}
			DM_GizmoActor.Empty();
		}
	}
	UE_LOG(LogTemp, Error, TEXT("UGizmoComponent::MakeObjectTranslucent DM_GizmoActor Num = %i"), DM_GizmoActor.Num());
}



void UGizmoComponent::GetGizmoActorDM(UStaticMeshComponent* GizmoMesh, TArray<UMaterialInstanceDynamic*>& DM_Material)
{
	if(!GizmoMesh) return;

	TArray<UMaterialInterface*> MI = GizmoMesh->GetMaterials();

	int32 index = 0;
	for (auto MItem : MI)
	{
		UMaterialInstanceDynamic* DM = GizmoMesh->CreateAndSetMaterialInstanceDynamicFromMaterial(index, GizmoMesh->GetMaterial(index));
		DM_GizmoActor.AddUnique(DM);
		++index;
		//UE_LOG(LogTemp, Error, TEXR("UGizmoComponent::GetGizmoActorDM"));
	}

}





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
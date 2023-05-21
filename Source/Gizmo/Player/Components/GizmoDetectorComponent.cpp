// Fill out your copyright notice in the Description page of Project Settings.


#include "Gizmo/Player/Components/GizmoDetectorComponent.h"
#include "Gizmo/Player/GizmoCharacter.h"
#include "Gizmo/Player/Components/GizmoComponent.h"

// Sets default values for this component's properties
UGizmoDetectorComponent::UGizmoDetectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGizmoDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UGizmoDetectorComponent::Init(UGizmoComponent* GComponent)
{
	if(!GComponent) return;

	OwnerComponent = GComponent;
	if(AGizmoCharacter* GA = Cast<AGizmoCharacter>(GComponent->GetOwner())) GizmoCharacter = GA;

	if(!GizmoCharacter) return;

	GizmoCharacter->GizmoDelegate.AddDynamic(this, &UGizmoDetectorComponent::TraceLauncher);
}

// Called every frame
void UGizmoDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGizmoDetectorComponent::TraceLauncher(EGizmoActiveStatus GActiveStatus)
{
	if(!GizmoCharacter) return;

	switch (GActiveStatus)
	{
	case EGizmoActiveStatus::Attached:
		GizmoCharacter->GetWorldTimerManager().ClearTimer(Timer_GizmoDetector);
		GizmoCharacter->GetWorldTimerManager().SetTimer(Timer_GizmoDetector, this, &UGizmoDetectorComponent::StartTrace, Frequency, true);
		break;
	case EGizmoActiveStatus::Drop:
	case EGizmoActiveStatus::Delete:
		StopTrace();
		break;
	default:
		break;
	}
}

void UGizmoDetectorComponent::StartTrace()
{
	if (!GizmoCharacter || !OwnerComponent) return;

	FHitResult GHit;
	OwnerComponent->CoursorTrace(OUT GHit);

	GizmoOverlap = GizmoCharacter->GetGizmoTool().GetGizmoTouch(GHit.GetComponent());

	if (SavedGizmoOverlap != GizmoOverlap)
	{
		LightOffGizmo();

		bool Overlap = GizmoOverlap == EGizmo::None ? false : true;

		OverlapGizmo(GizmoOverlap, OwnerComponent->GetGizmoTransition(), Overlap);

		GizmoCharacter->BP_GizmoOverlap(Overlap, GizmoOverlap);

		SavedGizmoOverlap = GizmoOverlap;
	}

}



void UGizmoDetectorComponent::StopTrace()
{
	if (!GizmoCharacter) return;
	GizmoCharacter->GetWorldTimerManager().ClearTimer(Timer_GizmoDetector);

	GizmoOverlap = SavedGizmoOverlap = EGizmo::None;

}


void UGizmoDetectorComponent::OverlapGizmo(EGizmo ActiveTouch, EGizmoTransition GTransition, bool DoActive)
{
	float fValue = DoActive ? 1.f : 0.f;
	switch (ActiveTouch)
	{
	case EGizmo::X:

		if (OwnerComponent->MD_GizmoX)
			OwnerComponent->MD_GizmoX->SetScalarParameterValue(FName("Overlap"), fValue);

		if (GTransition == EGizmoTransition::Scale && OwnerComponent->MD_GizmoSX)
			OwnerComponent->MD_GizmoSX->SetScalarParameterValue(FName("Overlap"), fValue);

		DM_Active = OwnerComponent->MD_GizmoX;

		UE_LOG(LogTemp, Error, TEXT("UGizmoDetectorComponent::OverlapGizmo Overlap X"));

		break;
	case EGizmo::Y:

		if (OwnerComponent->MD_GizmoY)
			OwnerComponent->MD_GizmoY->SetScalarParameterValue(FName("Overlap"), fValue);

		if (GTransition == EGizmoTransition::Scale && OwnerComponent->MD_GizmoSY)
			OwnerComponent->MD_GizmoSY->SetScalarParameterValue(FName("Overlap"), fValue);

		DM_Active = OwnerComponent->MD_GizmoY;

		UE_LOG(LogTemp, Error, TEXT("UGizmoDetectorComponent::OverlapGizmo Overlap Y"));

		break;
	case EGizmo::Z:

		if (OwnerComponent->MD_GizmoZ)
			OwnerComponent->MD_GizmoZ->SetScalarParameterValue(FName("Overlap"), fValue);

		if (GTransition == EGizmoTransition::Scale && OwnerComponent->MD_GizmoSZ)
			OwnerComponent->MD_GizmoSZ->SetScalarParameterValue(FName("Overlap"), fValue);

		DM_Active = OwnerComponent->MD_GizmoZ;

		UE_LOG(LogTemp, Error, TEXT("UGizmoDetectorComponent::OverlapGizmo Overlap Z"));
		break;
	case EGizmo::Pitch:

		if (OwnerComponent->MD_GizmoPitch)
			OwnerComponent->MD_GizmoPitch->SetScalarParameterValue(FName("Overlap"), fValue);

		DM_Active = OwnerComponent->MD_GizmoPitch;

		UE_LOG(LogTemp, Error, TEXT("UGizmoDetectorComponent::OverlapGizmo Overlap Pitch"));
		break;
	case EGizmo::Roll:

		if (OwnerComponent->MD_GizmoRoll)
			OwnerComponent->MD_GizmoRoll->SetScalarParameterValue(FName("Overlap"), fValue);

		DM_Active = OwnerComponent->MD_GizmoRoll;

		UE_LOG(LogTemp, Error, TEXT("UGizmoDetectorComponent::OverlapGizmo Overlap Roll"));
		break;
	case EGizmo::Yaw:

		if (OwnerComponent->MD_GizmoYaw)
			OwnerComponent->MD_GizmoYaw->SetScalarParameterValue(FName("Overlap"), fValue);

		DM_Active = OwnerComponent->MD_GizmoYaw;

		UE_LOG(LogTemp, Error, TEXT("UGizmoDetectorComponent::OverlapGizmo Overlap Yaw"));
		break;
	case EGizmo::None:

		if (DM_Active)
		{
			DM_Active->SetScalarParameterValue(FName("Overlap"), fValue);

			if (GTransition == EGizmoTransition::Scale && OwnerComponent)
			{
				OwnerComponent->MD_GizmoSX->SetScalarParameterValue(FName("Overlap"), fValue);
				OwnerComponent->MD_GizmoSY->SetScalarParameterValue(FName("Overlap"), fValue);
				OwnerComponent->MD_GizmoSZ->SetScalarParameterValue(FName("Overlap"), fValue);
			}
		}
		UE_LOG(LogTemp, Error, TEXT("UGizmoDetectorComponent::OverlapGizmo Overlap None"));
		break;
	}
}

void UGizmoDetectorComponent::LightOffGizmo()
{
	if(!OwnerComponent) return;

	OwnerComponent->MD_GizmoX->SetScalarParameterValue(FName("Overlap"), 0.f);
	OwnerComponent->MD_GizmoY->SetScalarParameterValue(FName("Overlap"), 0.f);
	OwnerComponent->MD_GizmoZ->SetScalarParameterValue(FName("Overlap"), 0.f);

	OwnerComponent->MD_GizmoSX->SetScalarParameterValue(FName("Overlap"), 0.f);
	OwnerComponent->MD_GizmoSY->SetScalarParameterValue(FName("Overlap"), 0.f);
	OwnerComponent->MD_GizmoSZ->SetScalarParameterValue(FName("Overlap"), 0.f);

	OwnerComponent->MD_GizmoPitch->SetScalarParameterValue(FName("Overlap"), 0.f);
	OwnerComponent->MD_GizmoRoll->SetScalarParameterValue(FName("Overlap"), 0.f);
	OwnerComponent->MD_GizmoYaw->SetScalarParameterValue(FName("Overlap"), 0.f);
}


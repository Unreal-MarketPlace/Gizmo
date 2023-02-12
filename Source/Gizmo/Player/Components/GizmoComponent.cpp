// Fill out your copyright notice in the Description page of Project Settings.


#include "Gizmo/Player/Components/GizmoComponent.h"

#include "DrawDebugHelpers.h"

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

	if (CanGizmoAttach())
	{
		Hit.GetActor()->SetReplicateMovement(true);
		Hit.GetActor()->SetReplicates(true);
		Hit.GetActor()->SetOwner(OwnerCharacter);

		OwnerCharacter->SetGizmoActor(Hit.GetActor());
	}
}

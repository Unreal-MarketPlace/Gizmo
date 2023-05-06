// Copyright Epic Games, Inc. All Rights Reserved.

#include "GizmoCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

#include "Gizmo/Player/Components/GizmoComponent.h"



//////////////////////////////////////////////////////////////////////////
// AGizmoCharacter

AGizmoCharacter::AGizmoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(40.f, 40.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	GSphere->SetupAttachment(RootComponent);


	//Gizmo
	GPivot = CreateDefaultSubobject<USceneComponent>("PlacementPoint");
	GPivot->SetupAttachment(RootComponent);

	GArrowX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowX"));
		GArrowSX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowSX"));
		GArrowSX->SetupAttachment(GArrowX, FName("Scale"));
	GArrowY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowY"));
		GArrowSY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowSY"));
		GArrowSY->SetupAttachment(GArrowY, FName("Scale"));
	GArrowZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowZ"));
		GArrowSZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowSZ"));
		GArrowSZ->SetupAttachment(GArrowZ, FName("Scale"));
	GArrowX->SetupAttachment(GPivot);
	GArrowY->SetupAttachment(GPivot);
	GArrowZ->SetupAttachment(GPivot);
	GPitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pitch"));
	GRoll = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Roll"));
	GYaw = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Yaw"));
	GPitch->SetupAttachment(GPivot);
	GRoll->SetupAttachment(GPivot);
	GYaw->SetupAttachment(GPivot);

	GizmoComponent = CreateDefaultSubobject<UGizmoComponent>(TEXT("GizmoComponent"));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 600;
	GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	GetCharacterMovement()->DefaultWaterMovementMode = EMovementMode::MOVE_Flying;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetMesh()->DestroyComponent();

	GetMovementComponent()->SetIsReplicated(true);

	GizmoTool.Initilize(GArrowX, GArrowSX, GArrowY, GArrowSY, GArrowZ, GArrowSZ, GPitch, GRoll, GYaw, GPivot);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}



void AGizmoCharacter::BeginPlay()
{
	Super::BeginPlay();

	//GizmoTool.Initilize(GArrowX, GArrowSX, GArrowY, GArrowSY, GArrowZ, GArrowSZ, GPitch, GRoll, GYaw, GPivot);

}



void AGizmoCharacter::CL_SetOtherGizmoActors_Implementation(AActor* OtherGizmoActor)
{
	if(!GizmoComponent || !OtherGizmoActor) return;

	GizmoComponent->AttachDeatachActorToGizmoActor(OtherGizmoActor);
	
}

void AGizmoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GizmoComponent && CanUpdateGizmoActorTransform)
	{
		GizmoComponent->UpdatedGizmoActorTransform(DeltaTime);
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void AGizmoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AGizmoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AGizmoCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp / Down", this, &AGizmoCharacter::MoveUp);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &AGizmoCharacter::Turn);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AGizmoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &AGizmoCharacter::LookUp);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AGizmoCharacter::LookUpAtRate);

	// Bind Action
	PlayerInputComponent->BindAction("LeftMouse", IE_Pressed, this, &AGizmoCharacter::LeftMousePressed);
	PlayerInputComponent->BindAction("LeftMouse", IE_Released, this, &AGizmoCharacter::LeftMouseReleased);
	PlayerInputComponent->BindAction("CTRL", IE_Pressed, this, &AGizmoCharacter::CTRL_Pressed);
	PlayerInputComponent->BindAction("CTRL", IE_Released, this, &AGizmoCharacter::CTRL_Released);
	PlayerInputComponent->BindAction("Delete", IE_Pressed, this, &AGizmoCharacter::DeleteGizmoActor);
	PlayerInputComponent->BindAction("DropGizmoActor", IE_Pressed, this, &AGizmoCharacter::DropGizmoActor);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGizmoCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AGizmoCharacter::TouchStopped);
}

void AGizmoCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AGizmoCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}


void AGizmoCharacter::Turn(float Rate)
{
	if(CanUpdateGizmoActorTransform) return;
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGizmoCharacter::LookUp(float Rate)
{
	if (CanUpdateGizmoActorTransform) return;
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}


void AGizmoCharacter::TurnAtRate(float Rate)
{
	if (CanUpdateGizmoActorTransform) return;
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGizmoCharacter::LookUpAtRate(float Rate)
{
	if (CanUpdateGizmoActorTransform) return;
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}




void AGizmoCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGizmoCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AGizmoCharacter::MoveUp(float Value)
{
	if (Value != 0)
	{
		GetMovementComponent()->AddInputVector(GetControlRotation().Quaternion().GetUpVector() * Value);
	}
}


void AGizmoCharacter::LeftMousePressed()
{
	SR_GizmoTrace();
}

void AGizmoCharacter::LeftMouseReleased()
{
	if (GizmoComponent)
	{
		GizmoComponent->ReleasedGizmoTool();
	}
}

void AGizmoCharacter::CTRL_Pressed()
{
	SR_UpdateCTRL(true);
}

void AGizmoCharacter::CTRL_Released()
{
	SR_UpdateCTRL(false);
}

void AGizmoCharacter::DeleteGizmoActor()
{
	if (!GizmoActor) return;

	CanUpdateGizmoActorTransform = false;
	GetGizmoTool().SetGizmoToolVisibility(false);
	GetGizmoTool().GetGizmoPivot()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	if (GizmoComponent)
	{
		GizmoComponent->DeleteGizmoActor();
		GizmoComponent->SetGizmoInputMode(false /* ActiveGizmo Mode */);
	}

	SR_DeleteGizmoActor();
}

void AGizmoCharacter::DropGizmoActor()
{
	if (!GizmoActor) return;

	// first Drop all attached actors
	if (GizmoComponent)
	{
		GizmoComponent->RemoveAllAttachedGizmoActor();
	}

	SR_DropGizmoActor();
}


void AGizmoCharacter::SR_DropGizmoActor_Implementation()
{
	if(!GizmoActor) return;

	OtherGizmoActors.Empty();

	// Second Drop Main Gizmo Actor
	SetMainGizmoActor(NULL, EGizmoActiveStatus::Drop);

}

void AGizmoCharacter::SR_DeleteGizmoActor_Implementation()
{
	if (!GizmoActor) return;

	GetGizmoTool().GetGizmoPivot()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	for (AActor* OtherGActor : OtherGizmoActors)
	{
		OtherGActor->Destroy();
	}

	OtherGizmoActors.Empty();
	GizmoActor->Destroy();

	SetMainGizmoActor(NULL, EGizmoActiveStatus::Delete);
}

void AGizmoCharacter::SR_UpdateCTRL_Implementation(bool Status)
{
	bCTRL = Status;
}

void AGizmoCharacter::SR_GizmoTrace_Implementation()
{
	if (GizmoComponent)
	{
		GizmoComponent->GizmoTrace();
	}
	UE_LOG(LogTemp, Error, TEXT("AGizmoCharacter::SR_GizmoTrace"));
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("AGizmoCharacter::SR_GizmoTrace")));
}



void AGizmoCharacter::CL_PressedGizmoTool_Implementation()
{
	if (GizmoComponent)
	{
		GizmoComponent->PressedGizmoTool();
	}
}

ESnapping AGizmoCharacter::GetGizmoSnappingMethod()
{
	if (GizmoComponent) return GizmoComponent->GetSnappingMothod(); 
	else return ESnapping::Off;
}

void AGizmoCharacter::SetGizmoSnappingMethod(ESnapping NewMethod)
{
	if (GizmoComponent) GizmoComponent->SetSnappingMethod(NewMethod);
}

float AGizmoCharacter::GetGizmoMouseSensitive()
{
	if (GizmoComponent) return GizmoComponent->GetMouseSensitive(); 
	else return -1;
}

void AGizmoCharacter::SetGizmoMouseSensitive(float NewSensitive)
{
	if (GizmoComponent) GizmoComponent->SetMouseSenstive(NewSensitive);
}

void AGizmoCharacter::SetMainGizmoActor(AActor* GActor, const EGizmoActiveStatus GActiveStatus)
{
	if(GizmoActor == GActor) return;

	GizmoActvationStatus = GActiveStatus;
	
	if (GActor)
	{
		// Remove Old Actor settings
		if (GizmoActor)
		{
			GizmoComponent->SetGizmoActorSettings(false);
		}

		AActor* OldGizmoActor = GizmoActor;
		GizmoActor = GActor;

		// Set New Actor settings
		if (GizmoActor && GizmoComponent)
		{
			GizmoComponent->SetGizmoActorSettings(true);
			GizmoComponent->AttachDetachGizmo();
		}

		IsGizmoActorValid = GizmoActor ? true : false;

		if (IsLocallyControlled())
		{
			// Server - Client
			OnRep_GizmoActor(OldGizmoActor);
		}
	}
	else
	{
		if (GizmoComponent)
		{
			GizmoComponent->SetGizmoActorSettings(false);
			GizmoComponent->AttachDetachGizmo(false);
		}

		
		AActor* OldGizmoActor = GizmoActor;
		GizmoActor = NULL;

		if (IsLocallyControlled())
		{
			// Server - Client
			OnRep_GizmoActor(OldGizmoActor);
		}
	}
}


void AGizmoCharacter::OnRep_GizmoActor(AActor* OldGizmoActor)
{
	// Only Client
	if (IsLocallyControlled())
	{
		bool bVisible = GizmoActor ? true : false;
		if (!bDefaultVisibleGizmo)
		{
			//GizmoTool.SetGizmoToolVisibility(bVisible);
			GizmoTool.SetGizmoToolVisibilityByTransition(GizmoComponent->GetGizmoTransition(), bVisible);
		}
		GizmoComponent->AttachDetachGizmo(bVisible);
		// 1
		if (OldGizmoActor)
			GizmoComponent->MakeGizmoActorTranslucent(false, OldGizmoActor);
		// 2
		if(GizmoActor)
			GizmoComponent->MakeGizmoActorTranslucent(true, GizmoActor);

		GizmoComponent->SetGizmoInputMode(bVisible /* ActiveGizmo Mode */);

		GizmoDelegate.Broadcast(GizmoActvationStatus);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AGizmoCharacter::OnRep_GizmoActor")));
	if (OldGizmoActor)
		UE_LOG(LogTemp, Error, TEXT("AGizmoCharacter::OnRep OldGName = %s"), *OldGizmoActor->GetName());
	if (GizmoActor)
		UE_LOG(LogTemp, Error, TEXT("AGizmoCharacter::OnRep NewGName = %s"), *GizmoActor->GetName());
	PrintLocalRole();
}


void AGizmoCharacter::SetOtherGizmoActors(AActor* OtherActor)
{
	if(!OtherActor || !GizmoComponent) return;

	if (OtherGizmoActors.Contains(OtherActor))
	{
		OtherActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GizmoComponent->SetGizmoActorSettings(false, OtherActor);
		OtherGizmoActors.Remove(OtherActor);
	}
	else
	{
		OtherGizmoActors.Add(OtherActor);
		OtherActor->AttachToComponent(GizmoActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		GizmoComponent->SetGizmoActorSettings(true, OtherActor);
	}


	if(IsLocallyControlled())
		GizmoComponent->AttachDeatachActorToGizmoActor(OtherActor);
	else
		CL_SetOtherGizmoActors(OtherActor);
}

void AGizmoCharacter::PrintLocalRole()
{
	if (GetLocalRole() == ROLE_Authority)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Role Authority")));
	if (GetLocalRole() == ROLE_AutonomousProxy)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Role AuthomousProxy")));
	if (GetLocalRole() == ROLE_SimulatedProxy)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Role SimulatedProxy")));
}

void AGizmoCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGizmoCharacter, GizmoActor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGizmoCharacter, bCTRL, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGizmoCharacter, OtherGizmoActors, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGizmoCharacter, GizmoActvationStatus, COND_OwnerOnly);
	//DOREPLIFETIME(AGizmoCharacter, GizmoActor);
	DOREPLIFETIME(AGizmoCharacter, IsGizmoActorValid);

}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "GizmoGameMode.h"
#include "Player/GizmoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGizmoGameMode::AGizmoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GizmoPlayer/BP_GzimoCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

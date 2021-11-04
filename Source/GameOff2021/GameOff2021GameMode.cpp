// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameOff2021GameMode.h"
#include "GameOff2021Character.h"
#include "UObject/ConstructorHelpers.h"

AGameOff2021GameMode::AGameOff2021GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

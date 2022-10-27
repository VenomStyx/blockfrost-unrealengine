// Copyright Epic Games, Inc. All Rights Reserved.

#include "unreal_blockfrostGameMode.h"
#include "unreal_blockfrostCharacter.h"
#include "UObject/ConstructorHelpers.h"

Aunreal_blockfrostGameMode::Aunreal_blockfrostGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

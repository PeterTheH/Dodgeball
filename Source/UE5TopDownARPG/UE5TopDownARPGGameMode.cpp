// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5TopDownARPGGameMode.h"
#include "UE5TopDownARPGPlayerController.h"
#include "UE5TopDownARPGCharacter.h"
#include "UE5TopDownARPGGameStateBase.h"
#include "ScoreHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "UE5TopDownARPG.h"
#include "SavePlayerState.h"
#include "Net/UnrealNetwork.h"


AUE5TopDownARPGGameMode::AUE5TopDownARPGGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AUE5TopDownARPGPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AScoreHUD> ScoreHUDBP(TEXT("/Game/TopDown/Blueprints/BP_ScoreHUD"));
	if (ScoreHUDBP.Class != NULL)
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Score HUD Spawned"));
		HUDClass = ScoreHUDBP.Class;
	}
	else {
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Score HUD Not Found"));
	}

	static ConstructorHelpers::FClassFinder<AUE5TopDownARPGGameStateBase> GameStateBaseBP(TEXT("/Game/TopDown/Blueprints/BP_GameStateBase"));
	if (GameStateBaseBP.Class != NULL)
	{
		GameStateClass = GameStateBaseBP.Class;
	}

	bReplicates = true;
}

void AUE5TopDownARPGGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUE5TopDownARPGGameMode, blueScore);
	DOREPLIFETIME(AUE5TopDownARPGGameMode, redScore);

}

void AUE5TopDownARPGGameMode::EndGame(bool IsWin)
{
	if (IsWin)
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Win"));
	}
	else
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Lose"));
	}
}
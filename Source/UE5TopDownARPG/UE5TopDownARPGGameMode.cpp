// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5TopDownARPGGameMode.h"
#include "UE5TopDownARPGPlayerController.h"
#include "UE5TopDownARPGCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "UE5TopDownARPG.h"
#include "SavePlayerState.h"

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

void AUE5TopDownARPGGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	bIsBlueTeam = ASavePlayerState::GetPlayerTeam();


	if (NewPlayer)
	{
		APawn* PlayerPawn = NewPlayer->GetPawn();
		AUE5TopDownARPGCharacter* Player = Cast<AUE5TopDownARPGCharacter>(PlayerPawn);

		if (PlayerPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("IsBlue: %d"), bIsBlueTeam);

			if (bIsBlueTeam) 
			{
				Player->bIsBlueTeam = true;
				PlayerPawn->SetActorLocation(FVector(0.0f, 700.0f, 100.0f));
			}
			else 
			{
				Player->bIsBlueTeam = false;
				PlayerPawn->SetActorLocation(FVector(0.0f, -700.0f, 100.0f));

			}
		}
	}
}
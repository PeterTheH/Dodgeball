// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UE5TopDownARPGGameMode.generated.h"

UCLASS(minimalapi)
class AUE5TopDownARPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUE5TopDownARPGGameMode();

	UPROPERTY()
	bool bIsBlueTeam = true;

	void EndGame(bool IsWin);

	virtual void PostLogin(APlayerController* NewPlayer) override;
};




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

	UPROPERTY(Replicated)
	int blueScore = 0;
	UPROPERTY(Replicated)
	int redScore = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void EndGame(bool IsWin);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UE5TopDownARPGPlayerState.generated.h"

/**
 *
 */
UCLASS()
class UE5TOPDOWNARPG_API AUE5TopDownARPGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AUE5TopDownARPGPlayerState();

	bool isBlueTeam;
};

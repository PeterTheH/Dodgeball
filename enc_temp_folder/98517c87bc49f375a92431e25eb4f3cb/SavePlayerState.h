// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SavePlayerState.generated.h"

UCLASS()
class UE5TOPDOWNARPG_API ASavePlayerState : public AActor
{
	GENERATED_BODY()

public:
	static bool GetPlayerTeam();
	static void SetPlayerTeam(bool bNew);

private:
	static bool bIsBlueTeam;
};

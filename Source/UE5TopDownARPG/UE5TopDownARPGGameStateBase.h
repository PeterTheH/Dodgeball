// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "UE5TopDownARPGGameStateBase.generated.h"

/**
 *
 */
UCLASS()
class UE5TOPDOWNARPG_API AUE5TopDownARPGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	AUE5TopDownARPGGameStateBase();

	UPROPERTY(Replicated)
	bool isBlueTeam = true;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};

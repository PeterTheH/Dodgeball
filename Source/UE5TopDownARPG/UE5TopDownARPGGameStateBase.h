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

	UPROPERTY(ReplicatedUsing = OnRep_Score)
	int blueScore = 0;
	UPROPERTY(ReplicatedUsing = OnRep_Score)
	int redScore = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Score();

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "UE5TopDownARPGGameStateBase.h"
#include "Net/UnrealNetwork.h"

AUE5TopDownARPGGameStateBase::AUE5TopDownARPGGameStateBase()
{
	bReplicates = true;  // Ensure the GameState itself is replicating
}

void AUE5TopDownARPGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Add the isBlueTeam variable to the list of replicated properties
	DOREPLIFETIME(AUE5TopDownARPGGameStateBase, isBlueTeam);
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "SavePlayerState.h"

bool ASavePlayerState::bIsBlueTeam = true;

bool ASavePlayerState::GetPlayerTeam()
{
	return bIsBlueTeam;
}

void ASavePlayerState::SetPlayerTeam(bool bNew)
{
	bIsBlueTeam = bNew;
}
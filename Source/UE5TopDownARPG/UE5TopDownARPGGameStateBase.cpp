// Fill out your copyright notice in the Description page of Project Settings.


#include "UE5TopDownARPGGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "ScoreHUD.h"

AUE5TopDownARPGGameStateBase::AUE5TopDownARPGGameStateBase()
{
	bReplicates = true; 
}

void AUE5TopDownARPGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUE5TopDownARPGGameStateBase, blueScore);
	DOREPLIFETIME(AUE5TopDownARPGGameStateBase, redScore);
}

void AUE5TopDownARPGGameStateBase::OnRep_Score()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	AScoreHUD* ScoreHUD = Cast<AScoreHUD>(PlayerController->GetHUD());
	ScoreHUD->VisualUpdate();
}

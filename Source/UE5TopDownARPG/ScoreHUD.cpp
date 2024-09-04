// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreHUD.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ScoreWidget.h"
#include "UE5TopDownARPG.h"
#include "UI/CreateSession.h"
#include "Net/UnrealNetwork.h"
#include "UE5TopDownARPGGameMode.h"
#include "UE5TopDownARPGGameStateBase.h"


void AScoreHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		ScoreWidget = CreateWidget<UScoreWidget>(GetWorld(), ScoreWidgetClass);
		if (IsValid(ScoreWidget))
		{
			ScoreWidget->AddToViewport();
			ScoreWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AScoreHUD::UpdateScoreText(bool isBlue)
{
	if (this)
	{
		//AUE5TopDownARPGGameMode* GameMode = Cast<AUE5TopDownARPGGameMode>(GetWorld()->GetAuthGameMode());
		AUE5TopDownARPGGameStateBase* GameState = Cast<AUE5TopDownARPGGameStateBase>(GetWorld()->GetGameState());


		if (isBlue)
			GameState->blueScore++;
		else
			GameState->redScore++;

		VisualUpdate();
	}
	else {
		UE_LOG(LogUE5TopDownARPG, Warning, TEXT("No ScoreHud Found"));
	}
}

void AScoreHUD::VisualUpdate()
{
	AUE5TopDownARPGGameStateBase* GameState = Cast<AUE5TopDownARPGGameStateBase>(GetWorld()->GetGameState());

	if (ScoreWidget)
	{
		UE_LOG(LogUE5TopDownARPG, Warning, TEXT("ScoreCalled Visual"));
		FText ScoreText = FText::FromString(FString::Printf(TEXT("%d:%d"), GameState->blueScore, GameState->redScore));
		ScoreWidget->SetScoreText(ScoreText);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/ScoreWidget.h"
#include "ScoreHUD.generated.h"

/**
 *
 */
UCLASS()
class UE5TOPDOWNARPG_API AScoreHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void UpdateScoreText(bool isBlue);

	void VisualUpdate();

	int blueScore = 0;
	int redScore = 0;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> ScoreWidgetClass;

	UPROPERTY()
	UScoreWidget* ScoreWidget;
};

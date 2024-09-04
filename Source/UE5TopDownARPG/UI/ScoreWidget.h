// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5TOPDOWNARPG_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetScoreText(FText NewText);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreText;
	
};

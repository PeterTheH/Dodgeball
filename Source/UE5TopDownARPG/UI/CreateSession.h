// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Delegates/IDelegateInstance.h"

#include "CreateSession.generated.h"

/**
 *
 */
UCLASS()
class UE5TOPDOWNARPG_API UCreateSession : public UUserWidget
{
	GENERATED_BODY()
private:
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
public:

	UFUNCTION(BlueprintCallable)
	void CreateSession(int publicConectionns);

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION()
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION()
	void OpenLevelAsListenServer();
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "CreateSession.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"


void UCreateSession::CreateSession(int publicConectionns = 2)
{
    // Get OnlineSubsystem and SessionInterface
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (!OnlineSubsystem) return;

    IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;
    
    FName SessionName = NAME_GameSession;

    // Check if the session already exists
    FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);
    if (ExistingSession)
    {
        UE_LOG(LogTemp, Warning, TEXT("Session '%s' already exists!"), *SessionName.ToString());

        // Optionally destroy the existing session before creating a new one
        SessionInterface->DestroySession(SessionName, FOnDestroySessionCompleteDelegate::CreateUObject(this, &UCreateSession::OnDestroySessionComplete));
        return;
    }


    // Bind the delegate
    OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UCreateSession::OnCreateSessionComplete)
    );

    // Set up session settings
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = true;  // Set to false for an online session
    SessionSettings.NumPublicConnections = publicConectionns;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinInProgress = true;

    // Create the session
    SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UCreateSession::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("Session %s created: %s"), *SessionName.ToString(), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));

    // Get the OnlineSubsystem so we can get the Session Interface
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        // Get the Session Interface to call the StartSession function
        IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

        if (Sessions.IsValid())
        {
            // Clear the SessionComplete delegate handle, since we finished this call
            Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
            if (bWasSuccessful)
            {
                // Set the StartSession delegate handle
                //OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

                // Our StartSessionComplete delegate should get called after this
                //Sessions->StartSession(SessionName);
                OpenLevelAsListenServer();
            }
        }

    }



}

void UCreateSession::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Session '%s' destroyed successfully. Creating a new session..."), *SessionName.ToString());
        CreateSession(); // Attempt to create the session again
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to destroy session '%s'."), *SessionName.ToString());
    }
}

void UCreateSession::OpenLevelAsListenServer()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("World is not valid."));
        return;
    }

    // Name of the level you want to open
    FName LevelName = "DodgeballMap";

    // Options string to specify this is a listen server
    FString Options = "listen";

    // Open the level as a listen server
    UGameplayStatics::OpenLevel(World, LevelName, true, Options);
}

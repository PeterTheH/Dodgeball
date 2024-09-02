// Fill out your copyright notice in the Description page of Project Settings.

#include "CreateSession.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "../UE5TopDownARPGPlayerController.h"
#include "../UE5TopDownARPGGameMode.h"
#include "../SavePlayerState.h"

void UCreateSession::CreateSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		// Destroy any existing session with the same name
		if (Sessions->GetNamedSession(SessionName))
		{
			UE_LOG(LogTemp, Log, TEXT("Session already exists, destroying it first."));
			Sessions->DestroySession(SessionName);
		}

		if (Sessions.IsValid() && UserId.IsValid())
		{
			/*
				Fill in all the Session Settings that we want to use.

				There are more with SessionSettings.Set(...);
				For example the Map or the GameMode/Type.
			*/
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
			SessionSettings->bIsDedicated = false;

			SessionSettings->Set(SETTING_MAPNAME, FString("DodgeballMap"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
				FOnCreateSessionCompleteDelegate::CreateUObject(this, &UCreateSession::OnCreateSessionComplete)
			);

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			//return Sessions->CreateSession(0, SessionName, *SessionSettings);

			if (!Sessions->CreateSession(*UserId, SessionName, *SessionSettings))
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to create session!"));
				// Handle the error or retry creating the session
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}
}

void UCreateSession::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

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

				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(
					FOnStartSessionCompleteDelegate::CreateUObject(this, &UCreateSession::OnStartOnlineGameComplete)
				);

				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession(SessionName);
			}
		}
	}
}

void UCreateSession::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}

		UE_LOG(LogTemp, Log, TEXT("Session State Before: %s"), ToString(Sessions->GetNamedSession(SessionName)->SessionState));
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful)
	{
		// Open the level
		UGameplayStatics::OpenLevel(GetWorld(), FName("DodgeballMap"), true, FString("listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to start session %s."), *SessionName.ToString());
	}
}

void UCreateSession::FindGameSessions()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem) return;

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	// Create a search settings object
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 25;  // Limit the number of search results
	SessionSearch->bIsLanQuery = true;     // LAN or Internet?
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Bind the delegate
	OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UCreateSession::OnFindSessionsComplete)
	);

	// Start the search
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UCreateSession::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem) return;

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	// Remove the delegate
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	if (bWasSuccessful && SessionSearch->SearchResults.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Found %d sessions."), SessionSearch->SearchResults.Num());

		FName ExistingSessionName = "SessionName";
		if (SessionInterface->GetNamedSession(ExistingSessionName))
		{
			SessionInterface->DestroySession(ExistingSessionName);
		}

		// Join the first session found
		const FOnlineSessionSearchResult& SearchResult = SessionSearch->SearchResults[0];

		// Bind the delegate for OnJoinSessionComplete
		OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UCreateSession::OnJoinSessionComplete)
		);

		if (!SessionInterface->JoinSession(0, TEXT("SessionName"), SearchResult))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to join session %s."), *SearchResult.GetSessionIdStr());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No sessions found."));
	}
}

void UCreateSession::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem) return;

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

	// Check if the join was successful
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectString;
		if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
		{
			// Open the level with the connect string
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->ClientTravel(TEXT("26.230.212.13"), TRAVEL_Absolute);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get connect string."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to join session: %s"), *SessionName.ToString());
	}
}

void UCreateSession::CreateSessionBP(FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	// Creating a local player where we can get the UserID from
	const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	CreateSession(netID, SessionName, bIsLAN, bIsPresence, MaxNumPlayers);
}

void UCreateSession::ChangeTeams(bool bIsBlue)
{
	//APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//AUE5TopDownARPGGameMode* GM = Cast<AUE5TopDownARPGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	ASavePlayerState::SetPlayerTeam(bIsBlue);
}
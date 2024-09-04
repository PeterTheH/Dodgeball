// Fill out your copyright notice in the Description page of Project Settings.

#include "CreateSession.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "../UE5TopDownARPGPlayerController.h"
#include "../UE5TopDownARPGGameMode.h"
#include "../UE5TopDownARPGGameStateBase.h"
#include "../SavePlayerState.h"

void UCreateSession::CreateSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions->GetNamedSession(SessionName))
		{
			UE_LOG(LogTemp, Log, TEXT("Session already exists, destroying it first."));
			Sessions->DestroySession(SessionName);
		}

		if (Sessions.IsValid() && UserId.IsValid())
		{
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

			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
				FOnCreateSessionCompleteDelegate::CreateUObject(this, &UCreateSession::OnCreateSessionComplete)
			);


			if (!Sessions->CreateSession(*UserId, SessionName, *SessionSettings))
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to create session!"));
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

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(
					FOnStartSessionCompleteDelegate::CreateUObject(this, &UCreateSession::OnStartOnlineGameComplete)
				);

				Sessions->StartSession(SessionName);
			}
		}
	}
}

void UCreateSession::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}

		UE_LOG(LogTemp, Log, TEXT("Session State Before: %s"), ToString(Sessions->GetNamedSession(SessionName)->SessionState));
	}

	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("DodgeballMap"), true, FString("listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to start session %s."), *SessionName.ToString());
	}
}

void UCreateSession::FindGameSessions(FString OptionalIPAddr)
{
	OptionalIP = OptionalIPAddr;

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem) return;

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 25;  // Limit the number of search results
	SessionSearch->bIsLanQuery = true;     // LAN or Internet?
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UCreateSession::OnFindSessionsComplete)
	);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UCreateSession::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem) return;

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	if (bWasSuccessful && SessionSearch->SearchResults.Num() > 0)
	{

		FName ExistingSessionName = "SessionName";
		if (SessionInterface->GetNamedSession(ExistingSessionName))
		{
			SessionInterface->DestroySession(ExistingSessionName);
		}

		const FOnlineSessionSearchResult& SearchResult = SessionSearch->SearchResults[0];

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

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectString;


		if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
		{
			// Open the level with the connect string
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			if (OptionalIP != "") 
			{
				ConnectString = OptionalIP;
			}

			UE_LOG(LogTemp, Warning, TEXT("ConnectString: %s"), *ConnectString);

			if (PlayerController)
			{
				PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
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
	const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();

	CreateSession(netID, SessionName, bIsLAN, bIsPresence, MaxNumPlayers);
}

void UCreateSession::ChangeTeams(bool bIsBlue)
{
	ASavePlayerState::SetPlayerTeam(bIsBlue);
}
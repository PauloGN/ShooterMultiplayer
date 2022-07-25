// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

namespace
{
	void PrintScreenM(const FString& text, FColor color = FColor::Cyan)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				color,
				text
			);
		}
	}
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	createSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	findSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
	joinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	destroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	startSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();

	if (subsystem)
	{
		sessionInterface = subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConections, FString MatchType)
{

	if (!sessionInterface.IsValid())
	{
		return;
	}

	auto existingSession = sessionInterface->GetNamedSession(NAME_GameSession);

	if (existingSession != nullptr)
	{
		sessionInterface->DestroySession(NAME_GameSession);
	}

	//bind and store delegate in FDelegateHandle so it can be removed later from the delegate list
	createSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);

	//Setting Up the session
	lastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	lastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSettings->NumPublicConnections = NumPublicConections;
	lastSessionSettings->bAllowJoinInProgress = true;
	lastSessionSettings->bAllowJoinViaPresence = true;
	lastSessionSettings->bShouldAdvertise = true;
	lastSessionSettings->bUsesPresence = true;
	lastSessionSettings->bUseLobbiesIfAvailable = true;
	lastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//Creating the session
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *lastSessionSettings))
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
		
		//Broadcast our own custom delegate
		multiplayerOnCreateSessionComplete.Broadcast(false);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Yellow,
			TEXT("Create Session Called...")
		);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{

	if (!sessionInterface.IsValid())
	{
		return;
	}

	findSessionsCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegate);

	lastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	lastSessionSearch->MaxSearchResults = MaxSearchResults;
	lastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool bWasSessionFound = (sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), lastSessionSearch.ToSharedRef()));

	if (!bWasSessionFound)
	{
		
		PrintScreenM("FIND SESSIONS on multiplayer system class Failed", FColor::Red);
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);
		multiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!sessionInterface.IsValid())
	{
		multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	joinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool bJoinedSession = (sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult));

	if (!bJoinedSession)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
		multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{

	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
	}

	multiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	
	PrintScreenM("ON_FIND_SESSION_COMPLETE on multiplayer system class");
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);
	}

	if (lastSessionSearch->SearchResults.Num() <= 0)
	{
		
		PrintScreenM("ON_FIND_SESSION_COMPLETE on multiplayer system class lastSessionSearch->SearchResults.Num() <= 0", FColor::Red);
		multiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	multiplayerOnFindSessionsComplete.Broadcast(lastSessionSearch->SearchResults, bWasSuccessful);

}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{

	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
	}

	multiplayerOnJoinSessionComplete.Broadcast(Result);

}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UMultiplayerSessionsSubsystem();

	//To handle session functionalities, that can be accessed by anyclasses that include MultiplayerSessionSubsystem class
	void CreateSession(int32 NumPublicConections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

protected:

	//
	// Internal Callbacks for the delegates tha are added to the online session interface delegate list
	// these do not need to be called outside this class
	//

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OndestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr sessionInterface;

	//
	// To add to the online session interface delegate list
	// These delegates will be bind to the multiplayerSessionSubsystem's internal call backs 
	//
	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	FDelegateHandle createSessiomCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate findSessionsCompleteDelegate;
	FDelegateHandle findSessiomCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate joinSessionCompleteDelegate;
	FDelegateHandle joinSessiomCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate destroySessionCompleteDelegate;
	FDelegateHandle destroySessiomCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate startSessionCompleteDelegate;
	FDelegateHandle startSessiomCompleteDelegateHandle;

};

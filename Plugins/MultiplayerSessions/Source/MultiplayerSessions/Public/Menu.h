// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumPublicConnections = 4,	FString MatchType = FString(TEXT("FreeForAll")));

protected:

	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* Inlevel, UWorld* InWorld) override;

	//
	// CallBacks for the Custom delegates on the MultiplayerSessionSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);//not dynamic delegate
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);// not dynamic delegate
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* Host_btn;

	UPROPERTY(meta = (BindWidget))
	UButton* Join_btn;
	
	UFUNCTION()
	void Host_btnClicked();

	UFUNCTION()
	void Join_btnClicked();

	void MenuTearDown();

	//inner function
	void OpenLobby();

	//Access to the Multiplayer Subsystem
	class UMultiplayerSessionsSubsystem* multiplayerSessionsSubsystem;

	int32 numPublicConnections{4};
	FString matchType{TEXT("FreeForAll")};

};

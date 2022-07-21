// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

private:

	UPROPERTY(Meta = (BindWidget))
	class UButton* Host_btn;

	UPROPERTY(Meta = (BindWidget))
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"


void UMenu::MenuSetup(int32 NumPublicConnections, FString MatchType)
{
	//Setting Up member variables
	numPublicConnections = NumPublicConnections;
	matchType = MatchType;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();

	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(true);
		}
	}

	//Geting the MultiplayerSessionsSubsystem through the UGameInstance Class

	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		//through this subsystem we can call functions that are into MultiplayerSessionSubsystem class
		multiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	//binding the callback to the delegate.
	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->multiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		multiplayerSessionsSubsystem->multiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		multiplayerSessionsSubsystem->multiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		multiplayerSessionsSubsystem->multiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		multiplayerSessionsSubsystem->multiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}

}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (Host_btn)
	{
		Host_btn->OnClicked.AddDynamic(this, &UMenu::Host_btnClicked);
	}

	if (Join_btn)
	{
		Join_btn->OnClicked.AddDynamic(this, &UMenu::Join_btnClicked);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* Inlevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(Inlevel, InWorld);
}

//Callbacks
void UMenu::OnCreateSession(bool bWasSuccessful)
{

	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Green,
				TEXT("CallBackFunction called On Create Session GOOD...")
			);
		}

		OpenLobby();
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				TEXT("CallBackFunction On Create Session Faild...")
			);
		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{

	if (multiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	for (auto result : SessionResults)
	{
		FString settingsValue;//this FString will return and be filled up acording to the key "MatchType"
		result.Session.SessionSettings.Get(FName("MatchType"), settingsValue);

		if (settingsValue == matchType)
		{

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.0f,
					FColor::Red,
					FString::Printf(TEXT("ACHOU: %s"), *settingsValue)
				);
			}

			multiplayerSessionsSubsystem->JoinSession(result);
			return;
		}
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();

	if (subsystem)
	{
		IOnlineSessionPtr sessionInterface = subsystem->GetSessionInterface();

		if (sessionInterface)
		{
			FString Address;
			sessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					1,//Replace
					60.0f,
					FColor::Yellow,
					FString::Printf(TEXT("Pegou IP: %s"),*Address)
				);
			}

			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						1,//Replace
						30.0f,
						FColor::Green,
						FString::Printf(TEXT("Mandou Entrar"), *Address)
					);
				}

				playerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::Host_btnClicked()
{
	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->CreateSession(numPublicConnections, matchType);
	}
}

void UMenu::Join_btnClicked()
{

	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();

	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeData;
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(false);
		}
	}

}

void UMenu::OpenLobby()
{
	UWorld* world = GetWorld();
	if (world)
	{
		//opens the level nane ? as a listen server
		world->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		//D:/Disk D / Unreal / Projetos / MultiplayerShooter / Content / ThirdPerson / Maps / Lobby.umap
	}
}

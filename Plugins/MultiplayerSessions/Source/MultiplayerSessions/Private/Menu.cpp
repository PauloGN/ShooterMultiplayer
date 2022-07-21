// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

void UMenu::MenuSetup()
{
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

	//Geting the MultiplayerSessionsSubsystem throug the UGameInstance Class

	UGameInstance* gameInstance = GetGameInstance();

	if (gameInstance)
	{
		multiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
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

	return false;
}

void UMenu::Host_btnClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			- 1,
			15.0f,
			FColor::Blue,
			TEXT("Host Button Clicked...")
			);
	}

	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->CreateSession(4, FString("FreeForAll"));
	}
}

void UMenu::Join_btnClicked()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		15.0f,
		FColor::Blue,
		TEXT("Join Button Clicked...")
	);
}

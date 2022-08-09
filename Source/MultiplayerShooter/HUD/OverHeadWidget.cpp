// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverHeadWidget::SetDisplayText(FString textToDisplay)
{
	if (displayText)
	{
		displayText->SetText(FText::FromString(textToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole ELocalRole = InPawn->GetLocalRole();
	FString Role;

	switch (ELocalRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	case ROLE_MAX:
		Role = FString("MAX");
		break;
	default:
		break;
	}

	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	SetDisplayText(LocalRoleString);
}

void UOverHeadWidget::ShowPlayerName()
{
	
	APlayerState* PlayerState = GetGameInstance()->GetFirstLocalPlayerController()->GetPlayerState<APlayerState>();

	if (PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();
		SetDisplayText(PlayerName);
	}
}

void UOverHeadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{

	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

}

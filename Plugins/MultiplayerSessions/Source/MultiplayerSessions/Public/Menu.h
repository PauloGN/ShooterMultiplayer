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
	void MenuSetup();

protected:

	virtual bool Initialize() override;

private:

	UPROPERTY(Meta = (BindWidget))
	class UButton* Host_btn;

	UPROPERTY(Meta = (BindWidget))
	UButton* Join_btn;
	
	UFUNCTION()
	void Host_btnClicked();

	UFUNCTION()
	void Join_btnClicked();

	//Access to the Multiplayer Subsystem
	class UMultiplayerSessionsSubsystem* multiplayerSessionsSubsystem;

};

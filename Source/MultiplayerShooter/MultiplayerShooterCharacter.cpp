// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


//////////////////////////////////////////////////////////////////////////
// AMultiplayerShooterCharacter

AMultiplayerShooterCharacter::AMultiplayerShooterCharacter() :
	createSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	findSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	joinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnJoinSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// need to include "OnlineSubsystem.h"
	IOnlineSubsystem* onlineSubSystem = IOnlineSubsystem::Get();

	if (onlineSubSystem)
	{
		//The session interface handles creating, managing and destroying game sessions.It also handles searching for sessions and other matchmaking functionality.
		onlineSessionInterface = onlineSubSystem->GetSessionInterface();
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage
			(
				-1,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("Found Subsystem %s"), *onlineSubSystem->GetSubsystemName().ToString())
			);
		}
		*/
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMultiplayerShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMultiplayerShooterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMultiplayerShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMultiplayerShooterCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiplayerShooterCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiplayerShooterCharacter::TouchStopped);
}

void AMultiplayerShooterCharacter::OpenLobby()
{
	UWorld* world = GetWorld();
	if (world)
	{
		//opens the level nane ? as a listen server
		world->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		//D:/Disk D / Unreal / Projetos / MultiplayerShooter / Content / ThirdPerson / Maps / Lobby.umap
	}
}

void AMultiplayerShooterCharacter::CallOpenLevel(const FString& Address)
{
	//need to include "Kismet/GameplayStatics.h"
	UGameplayStatics::OpenLevel(this, *Address);
}

void AMultiplayerShooterCharacter::CallClientTravel(const FString& Address)
{
	APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (playerController)
	{
		playerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void AMultiplayerShooterCharacter::CreateGameSession()
{
	//Called when pressed the key 1
	if (!onlineSessionInterface.IsValid())
	{
		return;
	}

	FNamedOnlineSession* existingSession = onlineSessionInterface->GetNamedSession(NAME_GameSession);

	if (existingSession != nullptr)
	{
		onlineSessionInterface->DestroySession(NAME_GameSession);
	}

	onlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> sessionSettings = MakeShareable(new FOnlineSessionSettings());
	sessionSettings->bIsLANMatch = false;
	sessionSettings->NumPublicConnections = 4;
	sessionSettings->bAllowJoinInProgress = true;
	sessionSettings->bAllowJoinViaPresence = true;
	sessionSettings->bShouldAdvertise = true;
	sessionSettings->bUsesPresence = true;
	sessionSettings->bUseLobbiesIfAvailable = true;
	sessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();


	onlineSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *sessionSettings);
}

void AMultiplayerShooterCharacter::JoinGameSession()
{
	//Find Game Session
	if (!onlineSessionInterface.IsValid())
	{
		return;
	}

	onlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegate);

	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->MaxSearchResults = 10000;
	sessionSearch->bIsLanQuery = false;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	onlineSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), sessionSearch.ToSharedRef());

}

void AMultiplayerShooterCharacter::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage
			(
				-1,
				15.0f,
				FColor::Blue,
				FString::Printf(TEXT("Created Session %s"), *sessionName.ToString())
			);
		}

		OpenLobby();
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage
			(
				-1,
				15.0f,
				FColor::Red,
				FString(TEXT("Faild to create session..."))
			);
		}
	}

}

void AMultiplayerShooterCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{

	if (!onlineSessionInterface.IsValid())
	{
		return;
	}

	for (auto results:sessionSearch->SearchResults)
	{
		FString id = results.GetSessionIdStr();
		FString user = results.Session.OwningUserName;

		FString matchType;
		results.Session.SessionSettings.Get(FName("MatchType"), matchType);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Cyan,
				FString::Printf(TEXT("Id: %s, User: %s"), *id, *user)
			);
		}

		if (matchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.0f,
					FColor::Cyan,
					FString::Printf(TEXT("Match Type: %s"), *matchType)
				);
			}
			//Join Session
			onlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);
			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			onlineSessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, results);
		}
	}
}

void AMultiplayerShooterCharacter::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (!onlineSessionInterface.IsValid())
	{
		return;
	}

	FString address;
	if (onlineSessionInterface->GetResolvedConnectString(NAME_GameSession, address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Yellow,
				FString::Printf(TEXT("Connect string..."), *address)
			);
		}
		CallClientTravel(address);
	}

}

void AMultiplayerShooterCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMultiplayerShooterCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMultiplayerShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerShooterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMultiplayerShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayerShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
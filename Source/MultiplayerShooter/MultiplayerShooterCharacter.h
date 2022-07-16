// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerShooterCharacter.generated.h"

UCLASS(config = Game)
class AMultiplayerShooterCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	AMultiplayerShooterCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}

	UFUNCTION(BlueprintCallable)
		void OpenLobby();

	UFUNCTION(BlueprintCallable)
		void CallOpenLevel(const FString& Address);

	UFUNCTION(BlueprintCallable)
		void CallClientTravel(const FString& Address);

public:

	//Pointer to the online session Interface
	//TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> onlineSessionInterface; //forward declaration
	//need to #include "Interfaces/OnlineSessionInterface.h"
	IOnlineSessionPtr onlineSessionInterface;
protected:

	UFUNCTION(BlueprintCallable)
		void CreateGameSession();

	UFUNCTION(BlueprintCallable)
		void JoinGameSession();

	//this function will be bound to the delegate variable
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);

private:

	//this is a delegate variable and a function will be bind to this
	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate findSessionsCompleteDelegate;

	TSharedPtr<FOnlineSessionSearch> sessionSearch;
};
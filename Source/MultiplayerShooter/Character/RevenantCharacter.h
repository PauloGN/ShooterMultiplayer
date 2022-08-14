// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RevenantCharacter.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API ARevenantCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARevenantCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* followCamera;


	//Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* overHeadWidget;

	//Weapon section
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* overlappingWeapon;
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* combatComp;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

public:	

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRight(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();

};

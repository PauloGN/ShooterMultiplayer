// Fill out your copyright notice in the Description page of Project Settings.


#include "RevenantCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "MultiplayerShooter/CombatComponents/CombatComponent.h"

// Sets default values
ARevenantCharacter::ARevenantCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(GetMesh());
	cameraBoom->TargetArmLength = 600.f;
	cameraBoom->bUsePawnControlRotation = true;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	overHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	overHeadWidget->SetupAttachment(RootComponent);

	combatComp = CreateAbstractDefaultSubobject<UCombatComponent>(TEXT("CombateComponent"));
	combatComp->SetIsReplicated(true);
}

// Called every frame
void ARevenantCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARevenantCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ARevenantCharacter::Jump);

	PlayerInputComponent->BindAxis(TEXT("Move Forward / Backward"), this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move Right / Left"), this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn Right / Left Gamepad"), this, &ThisClass::TurnRight);
	PlayerInputComponent->BindAxis(TEXT("Turn Right / Left Mouse"), this, &ThisClass::TurnRight);
	PlayerInputComponent->BindAxis(TEXT("Look Up / Down Gamepad"), this, &ThisClass::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Look Up / Down Mouse"), this, &ThisClass::LookUp);

	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &ARevenantCharacter::EquipButtonPressed);

}
void ARevenantCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARevenantCharacter, overlappingWeapon, COND_OwnerOnly);
}

void ARevenantCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (combatComp)
	{
		combatComp->characterREF = this;
	}
}


// Called when the game starts or when spawned
void ARevenantCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

void ARevenantCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ARevenantCharacter::ServerEquipButtonPressed_Implementation()
{
	if (combatComp)
	{
		combatComp->EquipWeapon(overlappingWeapon);
	}
}

void ARevenantCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		//getting the forward (X) direction based on controller direction
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ARevenantCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		//getting the Right (Y) direction based on controller direction
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ARevenantCharacter::TurnRight(float Value)
{
	AddControllerYawInput(Value);
}

void ARevenantCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ARevenantCharacter::EquipButtonPressed()
{
	if (combatComp)
	{
		if (HasAuthority())
		{
			combatComp->EquipWeapon(overlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ARevenantCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(false);
	}

	overlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (overlappingWeapon)
		{
			overlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ARevenantCharacter::IsWeaponEquipped()
{
	return (combatComp && combatComp->equippedWeapon);
}



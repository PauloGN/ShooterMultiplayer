// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "MultiplayerShooter/Character/RevenantCharacter.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquipe)
{
	if (characterREF == nullptr || WeaponToEquipe == nullptr)
	{
		return;
	}

	equippedWeapon = WeaponToEquipe;
	equippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket = characterREF->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(equippedWeapon, characterREF->GetMesh());
	}

	equippedWeapon->SetOwner(characterREF);
	equippedWeapon->ShowPickupWidget(false);

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



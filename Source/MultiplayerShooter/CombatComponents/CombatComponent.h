// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class ARevenantCharacter;//giving the class full access to this class

	void EquipWeapon(AWeapon* WeaponToEquipe);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:

	class ARevenantCharacter* characterREF;
	AWeapon* equippedWeapon;

public:	


		
};
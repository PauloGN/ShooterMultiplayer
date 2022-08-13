// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MultiplayerShooter/Character/RevenantCharacter.h"
#include "Net/UnrealNetWork.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	weaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(weaponMesh);

	weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	areaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	areaSphere->SetupAttachment(RootComponent);
	areaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	pickupWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	

	if (pickupWidget)
	{
		pickupWidget->SetVisibility(false);
	}

	//GetLocalRole() == ENetRole::ROLE_Authority is the same as HasAuthority();
	if (HasAuthority())
	{
		areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		areaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		areaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		areaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlapedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARevenantCharacter* RevenantCharREF = Cast<ARevenantCharacter>(OtherActor);
	if (RevenantCharREF)
	{
		RevenantCharREF->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlapedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARevenantCharacter* RevenantCharREF = Cast<ARevenantCharacter>(OtherActor);
	if (RevenantCharREF)
	{
		RevenantCharREF->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnRep_WeaponState()
{

	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	}

}

void AWeapon::SetWeaponState(EWeaponState State)
{
	weaponState = State;
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, weaponState);
}

void AWeapon::ShowPickupWidget(bool bSwowWidget)
{
	if (pickupWidget)
	{
		pickupWidget->SetVisibility(bSwowWidget);
	}
}


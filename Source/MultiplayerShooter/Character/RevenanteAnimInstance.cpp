// Fill out your copyright notice in the Description page of Project Settings.


#include "RevenanteAnimInstance.h"
#include "RevenantCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
void URevenanteAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	revenantREF = Cast<ARevenantCharacter>(TryGetPawnOwner());

}

void URevenanteAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (revenantREF == nullptr)
	{
		revenantREF = Cast<ARevenantCharacter>(TryGetPawnOwner());
	}
	if (revenantREF == nullptr)
	{
		return;
	}

	FVector velocity = revenantREF->GetVelocity();
	velocity.Z = 0.0f;
	speed = velocity.Size();

	bIsInAir = revenantREF->GetCharacterMovement()->IsFalling();
	bIsAccelerating = (revenantREF->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false);
	bWeaponEquipped = revenantREF->IsWeaponEquipped();
	bIsCrouched = revenantREF->bIsCrouched;
	bAiming = revenantREF->IsAiming();

	//Offset Yaw for strafing
	FRotator AimRotation = revenantREF->GetBaseAimRotation();
	FRotator MovimentRotation = UKismetMathLibrary::MakeRotFromX(revenantREF->GetVelocity());
	//UE_LOG(LogTemp, Warning, TEXT("Aiming yaw %f"), AimRotation.Yaw);
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovimentRotation, AimRotation);

	deltaRotation = FMath::RInterpTo(deltaRotation, DeltaRot, DeltaTime, 6.0f);
	yawOffSet = deltaRotation.Yaw;

	characterRotationLastFrame = characterRotation;
	characterRotation = revenantREF->GetActorRotation();

	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(characterRotation, characterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;

	const float Interp = FMath::FInterpTo(lean, Target, DeltaTime, 6.0f);
	lean = FMath::Clamp(Interp, -90.0f, 90.f );
}

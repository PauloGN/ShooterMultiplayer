// Fill out your copyright notice in the Description page of Project Settings.


#include "RevenanteAnimInstance.h"
#include "RevenantCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

}

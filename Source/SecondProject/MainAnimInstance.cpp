// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		// If it has a pawn then grab it
		Pawn = TryGetPawnOwner();
	}
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		// Get the speed that the pawn is moving
		FVector Speed = Pawn->GetVelocity();

		// Strip out the z speed
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		
		// Get magnitude of vector and set it to movement speed
		MovementSpeed = LateralSpeed.Size();

		// If is falling, bIsInAir becomes true
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
}

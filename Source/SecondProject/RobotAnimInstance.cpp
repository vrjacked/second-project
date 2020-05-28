/** Fill out your copyright notice in the Description page of Project Settings.*/
#include "RobotAnimInstance.h"
#include "GameFramework/Pawn.h"


void URobotAnimInstance::NativeInitializeAnimation()
{
    if (Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
        if (Pawn)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }
}

void URobotAnimInstance::UpdateAnimationProperties()
{
    if (Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
        if (Pawn)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }
    if(Pawn)
    {
        FVector Speed = Pawn->GetVelocity();
        FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
        MovementSpeed = LateralSpeed.Size();
    }
}

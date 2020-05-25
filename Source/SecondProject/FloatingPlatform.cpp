// Fill out your copyright notice in the Description page of Project Settings.

#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"


// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	StartPoint = FVector(0.0f);
	EndPoint = FVector(0.0f);
	StartPointer = &StartPoint;
	EndPointer = &EndPoint;
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Initialize off the actor location when we start
	*StartPointer = GetActorLocation();
	*EndPointer += *StartPointer;

	GetWorldTimerManager().SetTimer(InterpTime, this, &AFloatingPlatform::ToggleInterping, InterpTimer);
	Distance = (*EndPointer - *StartPointer).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, *EndPointer, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		float DistanceTraveled = (GetActorLocation() - *StartPointer).Size();
		if (Distance - DistanceTraveled <= 1.f)
		{
			ToggleInterping();
			GetWorldTimerManager().SetTimer(InterpTime, this, &AFloatingPlatform::ToggleInterping, InterpTimer);
			SwapVectors(StartPointer, EndPointer);
		}
	}

}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector* VecOne, FVector* VecTwo)
{
	FVector Temp = *VecOne;
	*VecOne = *VecTwo;
	*VecTwo = Temp;
}


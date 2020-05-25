// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SpawnVolume.h"
#include "Engine/Engine.h"
#include "Critter.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawiningBox"));
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	FVector Origin = SpawningBox->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	return Point;
}

void ASpawnVolume::SpawnOurPawn_Implementation(UClass* ToSpawn, const FVector& Location)
{
	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	if (ToSpawn)
	{
		ACritter* CritterSpawned =  World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.f), SpawnParams);
	}
}

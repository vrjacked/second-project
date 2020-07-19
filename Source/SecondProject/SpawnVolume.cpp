// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SpawnVolume.h"
#include "Engine/Engine.h"
#include "Critter.h"
#include "Enemy.h"
#include "AIController.h"

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

	if (Actor_1 && Actor_2 && Actor_3 && Actor_4)
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}
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

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);

		return SpawnArray[Selection];
	}
	else
	{
		return nullptr;
	}
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	if (ToSpawn)
	{
		AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams);

		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if(Enemy)
		{
			Enemy->SpawnDefaultController();

			AAIController* AICont = Cast<AAIController>(Enemy->GetController());
			if(AICont)
			{
				Enemy->AIController = AICont;
			}
		}
		
	}
}

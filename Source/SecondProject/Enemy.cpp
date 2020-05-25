/** Fill out your copyright notice in the Description page of Project Settings. */
#include "Enemy.h"
#include "AIController.h"
#include "Components/SphereComponent.h"

AEnemy::AEnemy()
{
    /** Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it. */
    PrimaryActorTick.bCanEverTick = true;
    AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ArgoSphere"));
    AggroSphere->SetupAttachment(GetRootComponent());
    AggroSphere->InitSphereRadius(600.f);
    

    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(GetRootComponent());
    CombatSphere->InitSphereRadius(75.f);
    
}

/** Called when the game starts or when spawned*/
void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    AIController = Cast<AAIController>(GetController());
}

/** Called every frame*/
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

/** Called to bind functionality to input*/
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::CombatSphereBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
}

void AEnemy::CombatSphereEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::AggroSphereBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        if(Main)
        {
            MoveToTarget(Main);
        }
    }
}

void AEnemy::AggroSphereEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
}

void AEnemy::MoveToTarget(AMain* Target)
{
    SetEnemyMovementStatus(EEnemyMovementStatus::Ems_Attacking);

    if(AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveToTarget()"));
    }
}

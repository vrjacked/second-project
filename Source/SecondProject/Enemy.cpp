/** Fill out your copyright notice in the Description page of Project Settings. */
#include "Enemy.h"
#include "AIController.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Color.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"

AEnemy::AEnemy()
{
    /** Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it. */
    PrimaryActorTick.bCanEverTick = true;
    AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ArgoSphere"));
    AggroSphere->SetupAttachment(GetRootComponent());
    AggroSphere->InitSphereRadius(600.f);

    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(GetRootComponent());
    CombatSphere->InitSphereRadius(5.5f);

    CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
    CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
                                       FName("GreatSword2"));

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
                                  FName("GreatSword2"));

    CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
    CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
    
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    bOverlappingCombatSphere = false;

    Health = 75.f;
    MaxHealth = 100.f;
    Damage = 10.f;
}

/** Called when the game starts or when spawned*/
void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    AIController = Cast<AAIController>(GetController());

    AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AggroSphereBegin);
    AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AggroSphereEnd);

    CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereBegin);
    CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereEnd);

    if (WeaponMesh)
    {
    }
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
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        {
            if (Main)
            {
                CombatTarget = Main;
                bOverlappingCombatSphere = true;
                Attack();
            }
        }
    }
}

void AEnemy::CombatSphereEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        {
            if (Main)
            {
                CombatTarget = Main;
                bOverlappingCombatSphere = false;
                if (EnemyMovementStatus != EEnemyMovementStatus::Attacking)
                {
                    MoveToTarget(Main);
                }
            }
        }
    }
}

void AEnemy::AggroSphereBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        if (Main)
        {
            MoveToTarget(Main);
            CombatTarget = nullptr;
        }
    }
}

void AEnemy::AggroSphereEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        if (Main)
        {
            SetEnemyMovementStatus(EEnemyMovementStatus::Idle);
            if (AIController)
            {
                AIController->StopMovement();
            }
        }
    }
}

void AEnemy::MoveToTarget(AMain* Target)
{
    SetEnemyMovementStatus(EEnemyMovementStatus::MoveToTarget);

    if (AIController)
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(Target);
        MoveRequest.SetAcceptanceRadius(25.0f);


        FNavPathSharedPtr NavPath;


        AIController->MoveTo(MoveRequest, &NavPath);


        if (NavPath != nullptr)
        {
            auto PathPoints = NavPath->GetPathPoints();
            for (auto Point : PathPoints)
            {
                FVector Location = Point.Location;
                UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 10.f, FLinearColor::Red, 1.5);
            }
        }
    }
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        if (Main)
        {
            if (Main->HitParticles)
            {
                const USkeletalMeshSocket* BloodSocket = GetMesh()->GetSocketByName("BloodSocket");
                if (BloodSocket)
                {
                    FVector SocketLocation = BloodSocket->GetSocketLocation(GetMesh());
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation,
                                                             FRotator(0.f), false);
                }
            }
            if (Main->HitSound)
            {
                UGameplayStatics::PlaySound2D(this, Main->HitSound);
            }
        }
    }
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
     bOverlappingCombatSphere = false;
}

void AEnemy::ActivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
    if (AIController)
    {
        AIController->StopMovement();
        SetEnemyMovementStatus(EEnemyMovementStatus::Attacking);
    }
    if (!bAttacking)
    {
        bAttacking = true;
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(CombatMontage, 1.35f);
            AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
        }
        if (SwingSound)
        {
            UGameplayStatics::PlaySound2D(this, SwingSound);
        }
    }
}

void AEnemy::AttackEnd()
{
    bAttacking = false;
    if (bOverlappingCombatSphere)
    {
        Attack();
    }
}

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
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "EdGraph/EdGraphNode.h"

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

    bOverlappingCombatSphere = false;

    Health = 75.f;
    MaxHealth = 100.f;
    Damage = 10.f;

    AttackMinTime = .5f;
    AttackMaxTime = 3.5f;

    EnemyMovementStatus = EEnemyMovementStatus::Idle;

    DeathDelay = 3.f;
}

/** Called every frame*/
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
    if (Health - DamageAmount <= 0.0f)
    {
        Health -= DamageAmount;
        Die();
    }
    else
    {
        Health -= DamageAmount;
    }

    return DamageAmount;
}

void AEnemy::Die()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        AnimInstance->Montage_Play(CombatMontage, 1.0f);
        AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
    }
    SetEnemyMovementStatus(EEnemyMovementStatus::Dead);

    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AggroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatSphere->SetCollisionEnabled((ECollisionEnabled::NoCollision));
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DeathEnd()
{
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;

    GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Dissapear, DeathDelay);
}

bool AEnemy::Alive()
{
    return GetEnemyMovementStatus() != EEnemyMovementStatus::Dead;
}

void AEnemy::Dissapear()
{
    Destroy();
}

/** Called when the game starts or when spawned*/
void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    AIController = Cast<AAIController>(GetController());

    AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AggroSphereEnter);
    AggroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AggroSphereExit);

    CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereEnter);
    CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereExit);

    CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::WeaponOverlapBegin);
    CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::WeaponOverlapEnd);

    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

/** Called to bind functionality to input*/
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::AggroSphereEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
    if (OtherActor && Alive())
    {
        AMain* Main = Cast<AMain>(OtherActor);
        if (Main)
        {
            MoveToTarget(Main);
        }
    }
}

void AEnemy::AggroSphereExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex)
{
    AMain* Main = Cast<AMain>(OtherActor);
    if (OtherActor && Main)
    {
        Main->SetHasCombatTarget(false);
        if (Main->MainPlayerController)
        {
            Main->MainPlayerController->RemoveEnemyHealthBar();
        }
        CombatTarget = nullptr;
        Main = nullptr;
        SetEnemyMovementStatus(EEnemyMovementStatus::Idle);
        if (AIController)
        {
            AIController->StopMovement();
        }
    }
}

void AEnemy::CombatSphereEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult)
{
    AMain* Main = Cast<AMain>(OtherActor);
    if (OtherActor && Alive() && Main)
    {
        Main->SetCombatTarget(this);
        Main->SetHasCombatTarget(true);
        if (Main->MainPlayerController)
        {
            Main->MainPlayerController->DisplayEnemyHealthBar();
        }
        CombatTarget = Main;
        bOverlappingCombatSphere = true;
        Attack();
    }
}

void AEnemy::CombatSphereExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AMain* Main = Cast<AMain>(OtherActor);
    if (OtherActor && Main)
    {
        {
            bOverlappingCombatSphere = false;
            if (EnemyMovementStatus != EEnemyMovementStatus::Attacking)
            {
                MoveToTarget(Main);
            }
            GetWorldTimerManager().ClearTimer(AttackTimer);
        }
    }
}

    void AEnemy::WeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
    {
        AMain* Main = Cast<AMain>(OtherActor);
        if (OtherActor && Main)
        {
            if (Main->HitParticles)
            {
                const USkeletalMeshSocket* BloodSocket = GetMesh()->GetSocketByName("BloodSpawner");
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
            if (DamageTypeClass)
            {
                UGameplayStatics::ApplyDamage(Main, Damage, AIController, this,
                                              DamageTypeClass);
            }
        }
    }

    void AEnemy::WeaponOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
    {
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
                    UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 10.f,
                                                          FLinearColor::Red, 1.5);
                }
            }
        }
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
        if (Alive())
        {
            if (AIController)
            {
                AIController->StopMovement();
            }
            SetEnemyMovementStatus(EEnemyMovementStatus::Attacking);
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                AnimInstance->Montage_Play(CombatMontage, 1.35f);
                AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
            }
            if (SwingSound)
            {
                UGameplayStatics::PlaySound2D(this, SwingSound);
            }
        }
    }

    void AEnemy::AttackEnd()
    {
        if (bOverlappingCombatSphere)
        {
            float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
            GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
        }
        else
        {
            MoveToTarget(CombatTarget);
        }
    }

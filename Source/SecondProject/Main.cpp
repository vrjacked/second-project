// Fill out your copyright notice in the Description page of Project Settings.
#include "Main.h"
#include "Enemy.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "MainPlayerController.h"

/** Sets default values */
AMain::AMain()
{
    /** Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it. */
    PrimaryActorTick.bCanEverTick = true;

    /** Create camera boom (pulls towards the player if there's a collision */
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance
    CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller
    // Set size for collision capsule
    GetCapsuleComponent()->SetCapsuleSize(92.192665f, 34.700401f);

    /** Create Follow Camera */
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    /** Attach the camera to the end of the boom and let the boom adjust to match
      * the controller orientation
      */
    FollowCamera->bUsePawnControlRotation = false;
    /** Set our turn rates for input */
    BaseTurnRate = 65.f;
    BaseLookUpRate = 65.f;
    
    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(GetRootComponent());
    CombatSphere->InitSphereRadius(5.5f);
    
    // Don't rotate character when controller rotates
    // Let that just affect the camera.
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.f, 350.f, 0.f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 650.f;
    GetCharacterMovement()->AirControl = 0.8f;

    Sprinting = false;
    SprintingMoving = false;
    Exhausted = false;
    ExhaustedRecoveryCutoff = 75;
    MaxHealth = 100.f;
    Health = 65.f;
    Stamina = 100.f;
    MaxStamina = 100.f;
    StaminaDrainRate = .5f;
    StaminaRecoveryRate = .5f;
    StaminaExhaustedRecoveryRate = .25f;
    StaminaColor = FLinearColor(0.223228, 0.401978f, 1.0f, 1.0f);
    Jackhammers = 0;
    VisibilityEnum = ESlateVisibility::Hidden;
    bLMBdown = false;
    EquippedWeapon = {nullptr};

    InterpSpeed = 15.f;
    bInterpToEnemey = false;

    bHasCombatTarget = false;
}

/** Called to bind functionality to input */
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
    PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookuUpAtRate);

    PlayerInputComponent->BindAxis("Sprint", this, &AMain::Sprint);
}


/** This will take care of player movement */
void AMain::MoveForward(float Value)
{
    /** Good practice to make sure not a null pointer
     *  also checking to make sure button is being pressed
     */
    if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking))
    {
        /** Find out which way is forward */
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

        /** Take the FRotator of the controller, sand convert it to an X Axis */
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        if (!Sprinting || Stamina == 0 || Exhausted == true)
        {
            SprintingMoving = false;
            Value /= 3.0;
        }
        else
        {
            SprintingMoving = true;
        }
        /** Add the value to the movement of the character, in the direction of our controllers yaw */
        AddMovementInput(Direction, Value);
    }
}

void AMain::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        if (!Sprinting || Stamina == 0 || Exhausted == true)
        {
            SprintingMoving = false;
            Value /= 3.0;
        }
        else
        {
            SprintingMoving = true;
        }
        AddMovementInput(Direction, Value);
    }
}

void AMain::TurnAtRate(float Rate)
{
    /** Takes the input value, multiplies it by base lookup rate, and normalizes on world delta
     *  seconds so it doesn't play like Fallout!
     */
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookuUpAtRate(float Rate)
{
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AMain::ShowPickupLocations()
{
    for (int32 i = 0; i < PickupLocations.Num(); i++)
    {
        UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.0f, 12, FLinearColor::Blue, 10.0f, 2.0f);
    }
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
    FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
    return LookAtRotationYaw;
}

void AMain::SetInterpToEnemy(bool Interp)
{
    bInterpToEnemey = Interp;
}

void AMain::DecrementHealth(float Amount)
{
    Health -= Amount;
    if (Health <= 0.f)
    {
        Die();
    }
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
    AActor* DamageCauser)
{
    DecrementHealth(DamageAmount);
    
    return DamageAmount;
}

void AMain::IncrementJackHammers(int32 Amount)
{
    this->Jackhammers += Amount;
}

void AMain::Die()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && CombatMontage)
    {
        AnimInstance->Montage_Play(CombatMontage, 1.0f);
        AnimInstance->Montage_JumpToSection(FName("Death"));
    } 
}

/** Called when the game starts or when spawned */
void AMain::BeginPlay()
{
    Super::BeginPlay();

    MainPlayerController = Cast<AMainPlayerController>(GetController());
}

/** Called every frame */
void AMain::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (SprintingMoving && Stamina > 0)
    {
        Stamina -= StaminaDrainRate * DeltaTime;
    }
    if (!SprintingMoving && Stamina < 100)
    {
        if (Exhausted)
        {
            Stamina += StaminaExhaustedRecoveryRate * DeltaTime;
        }
        else
        {
            Stamina += StaminaRecoveryRate * DeltaTime;
        }
    }
    /** Trigger to end exhausted state */
    if (Exhausted && Stamina > ExhaustedRecoveryCutoff)
    {
        Exhausted = false;
        StaminaColor = FLinearColor(0.223228, 0.401978, 1.0f, 1.0f);
    }
    
    if(bInterpToEnemey && CombatTarget)
    {
       FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation()); 
       FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

       SetActorRotation(InterpRotation);
    }

    if (CombatTarget)
    {
        CombatTargetLocation = CombatTarget->GetActorLocation();
        if (MainPlayerController)
        {
            MainPlayerController->EnemyLocation = CombatTargetLocation;
        }
    }
}


void AMain::Sprint(float Rate)
{
    if (Rate != 0.0f)
    {
        if (Stamina <= 0)
        {
            /** Trigger to start exhausted state */
            Exhausted = true;
            StaminaColor = FLinearColor(1.0f, 0.313989f, 0.0f, 1.0f);
            SprintingMoving = false;
            Sprinting = false;
        }
        else
        {
            Sprinting = true;
        }
    }
    else
    {
        Sprinting = false;
        SprintingMoving = false;
    }
}

void AMain::LMBDown()
{
    bLMBdown = true;
    if (ActiveOverlappingItem)
    {
        AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
        if (Weapon)
        {
            Weapon->Equip(this);
            SetActiveOverlappingItem(nullptr);
        }
    }
    else if (EquippedWeapon)
    {
        Attack();
    }
}

void AMain::LMBUp()
{
    bLMBdown = false;
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
    if (EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }

    EquippedWeapon = WeaponToSet;
}


void AMain::Attack()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!bAttacking)
    {
        bAttacking = true;
        SetInterpToEnemy(true);
        
        if (AnimInstance && CombatMontage)
        {
            int32 Section = FMath::RandRange(0, 1);
            switch (Section)
            {
            case 0:
                AnimInstance->Montage_Play(CombatMontage, .7f);
                AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
                break;
            case 1:
                AnimInstance->Montage_Play(CombatMontage, .9f);
                AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);

                break;

            default:
                ;
            }
        }
    }
}

void AMain::AttackEnd()
{
    bAttacking = false;
    SetInterpToEnemy(false);
    if (bLMBdown)
    {
        Attack();
    }
}

void AMain::PlaySwingSound()
{
    if(EquippedWeapon->SwingSound)
    {
        UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
    }
}

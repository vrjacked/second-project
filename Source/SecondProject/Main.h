/** Fill out your copyright notice in the Description page of Project Settings. */

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Math/Color.h"
#include "Components/SphereComponent.h"
#include "Main.generated.h"

UCLASS()
class SECONDPROJECT_API AMain : public ACharacter
{
    GENERATED_BODY()

public:
    /** Sets default values for this character's properties */
    AMain();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
    bool bHasCombatTarget;

    FORCEINLINE void SetHasCombatTarget(bool HasTarget) {bHasCombatTarget = HasTarget;}

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Controller")
    class AMainPlayerController* MainPlayerController;
        
    TArray<FVector> PickupLocations;

    UFUNCTION(BlueprintCallable)
    void ShowPickupLocations();

    /** Camera boom positioning the camera behind the player */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;
    
    /** Follow Camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
    class AWeapon* EquippedWeapon;
    
    UPROPERTY(BlueprintReadOnly, category = "PlayerStats | Stamina")
    FLinearColor StaminaColor;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
    class AItem* ActiveOverlappingItem;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
    class UAnimMontage* CombatMontage;

    /** Base turn rates to scale turning functions for the camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnRate;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats")
    float MaxHealth;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats")
    float Health;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats | Stamina")
    float Stamina;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats | Stamina")
    float MaxStamina;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats | Stamina")
    bool Exhausted;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats | Stamina")
    float ExhaustedRecoveryCutoff;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats | Stamina")
    float StaminaRecoveryRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "PlayerStats | Stamina")
    float StaminaExhaustedRecoveryRate;
    
    UPROPERTY(EditAnywhere, category = "PlayerStats | Stamina")
    float StaminaDrainRate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
    bool bAttacking;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particles")
    class UParticleSystem* HitParticles;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite = "PlayerStats")
    int32 Jackhammers;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite = "PlayerStats")
    ESlateVisibility VisibilityEnum;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
    class USoundCue* HitSound;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    USphereComponent* CombatSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat")
    FVector CombatTargetLocation;

    
    
    FRotator GetLookAtRotationYaw(FVector Target);
    bool Sprinting;
    bool SprintingMoving;
    float InterpSpeed;
    bool bInterpToEnemey;
    void SetInterpToEnemy(bool Interp);
    UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Combat")
    class AEnemy* CombatTarget;
    FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }
    void DecrementHealth(float Amount);
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                             class AController* EventInstigator, AActor* DamageCauser) override;
    
    UFUNCTION(BlueprintCallable)
    void IncrementJackHammers(int32 Amount);
    
    UFUNCTION(BlueprintCallable)
    void IncrementHealth(float Amount);
    
    void Die();
    bool bDead;
    virtual void Jump() override;


protected:
    /** Called when the game starts or when spawned */
    virtual void BeginPlay() override;

public:
    /** Called every frame */
    virtual void Tick(float DeltaTime) override;

    /** Called to bind functionality to input */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Called for forwards/backwards input */
    void MoveForward(float Value);

    /** Called for side to side input */
    void MoveRight(float Value);

    /** Called via input to turn at a given rate 
     * @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void TurnAtRate(float Rate);

    /** Called via input to look up/down at a given rate 
     * @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
     */
    void LookuUpAtRate(float Rate);
    void Sprint(float Rate);
    bool bLMBdown;
    void LMBDown();
    void LMBUp();

    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    void SetEquippedWeapon(AWeapon* WeaponToSet);
    FORCEINLINE AWeapon* GetEquipppedWeapon() { return EquippedWeapon; }
    FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }


    UFUNCTION(BlueprintCallable)
    void Attack();

    UFUNCTION(BlueprintCallable)
    void AttackEnd();

    UFUNCTION(BlueprintCallable)
    void PlaySwingSound();

    UFUNCTION(BlueprintCallable)
    void DeathEnd(); 

    UFUNCTION(BlueprintCallable)
    void UpdateCombatTarget();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    TSubclassOf<AEnemy> EnemyFilter;

    void SwitchLevel(FName LevelName);

    UFUNCTION(BlueprintCallable)
    void SaveGame();

    UFUNCTION(BlueprintCallable)
    void LoadGame(bool SetPosition);
};

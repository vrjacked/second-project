/** Fill out your copyright notice in the Description page of Project Settings. */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Main.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy.generated.h"

UENUM(BlueprintType) enum class EEnemyMovementStatus :uint8
{
    Idle UMETA(DisplayName = "Idle"),
    MoveToTarget UMETA(DisplayName = "MoveToTarget"),
    Attacking UMETA(DisplayName = "Attacking"),
    Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class SECONDPROJECT_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    /** Sets default values for this character's properties */
    AEnemy();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    EEnemyMovementStatus EnemyMovementStatus;

    FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus const Status) { EnemyMovementStatus = Status; }
    FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    class USphereComponent* AggroSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    USphereComponent* CombatSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category ="AI")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    class AAIController* AIController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    class UParticleSystem* HitParticles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    class USoundCue* HitSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
    USoundCue* SwingSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat")
    class UBoxComponent* CombatCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    class UAnimMontage* CombatMontage;

    FTimerHandle AttackTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackMinTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float AttackMaxTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    TSubclassOf<UDamageType> DamageTypeClass;

    FTimerHandle DeathTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    float DeathDelay;
    
    virtual auto TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                            class AController* EventInstigator, AActor* DamageCauser) -> float override;

   void Die();

   UFUNCTION(BlueprintCallable)
   void DeathEnd();

    UFUNCTION(BlueprintCallable)
   bool Alive();

    void Dissapear();
    
protected:
    /** Called when the game starts or when spawned */
    virtual void BeginPlay() override;

public:
    /** Called every frame */
    virtual void Tick(float DeltaTime) override;

    /** Called to bind functionality to input */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    virtual void CombatSphereBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult
    );

    UFUNCTION()
    virtual void CombatSphereEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
    );

    UFUNCTION()
    virtual void AggroSphereBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult
    );

    UFUNCTION()
    virtual void AggroSphereEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
    );

    UFUNCTION(BlueprintCallable)
    void MoveToTarget(class AMain* Target);

    UFUNCTION()
    void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex);
    
    UFUNCTION(BlueprintCallable)
    void ActivateCollision();
    
    UFUNCTION(BlueprintCallable)
    void DeactivateCollision();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI")
    bool bOverlappingCombatSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI")
    AMain* CombatTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
    bool bAttacking;

    void Attack();
    
    UFUNCTION(BlueprintCallable)
    void AttackEnd();
};

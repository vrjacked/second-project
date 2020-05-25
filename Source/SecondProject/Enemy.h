/** Fill out your copyright notice in the Description page of Project Settings. */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Main.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{
    Ems_Idle UMETA(DisplayName = "Idle"),
    Ems_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
    Ems_Attacking UMETA(DisplayName = "Attacking"),

    Ems_Max UMETA(DisplayName = "DefaultMAX")
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    class USphereComponent* AggroSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    USphereComponent* CombatSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    class AAIController* AIController;
    
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

    void MoveToTarget(class AMain* Target); 
};

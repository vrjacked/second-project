// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState :uint8
{
    Pickup UMETA(DisplayName = "Pickup"),
    Equipped UMETA(DisplayName = "Equipped"),
};

UCLASS()
class SECONDPROJECT_API AWeapon : public AItem
{
    GENERATED_BODY()

public:

    AWeapon();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | State")
    EWeaponState WeaponState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
    bool bWeaponParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
    class USoundCue* OnEquipSound;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
    class USoundCue* SwingSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
    class USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item | Combat")
    class UBoxComponent* CombatCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item | Combat")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
    TSubclassOf<UDamageType> DamageTypeClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
    AController* WeaponInstigator;

    FORCEINLINE void SetInstigator(AController* Inst) {WeaponInstigator=Inst;}
    
    protected:
    virtual void BeginPlay() override; 
    
public:
    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult& SweepResult) override;

    virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

    void Equip(class AMain* Char);

    FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
    FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

    UFUNCTION()
    void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult);

    UFUNCTION()
    void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    
    UFUNCTION(BlueprintCallable)
    void ActivateCollision();
    
    UFUNCTION(BlueprintCallable)
    void DeactivateCollision();
};

/** Fill out your copyright notice in the Description page of Project Settings. */

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class SECONDPROJECT_API APickup : public AItem
{
    GENERATED_BODY()

public:

    APickup();

    UFUNCTION()
    virtual void OnOverlapBegin
    (
        UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult
    );

    UFUNCTION()
    virtual void OnOverlapEnd
    (
        UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    UFUNCTION(BlueprintImplementableEvent, Category="Pickup")
    void OnPickupBP(class AMain* Targat);
};

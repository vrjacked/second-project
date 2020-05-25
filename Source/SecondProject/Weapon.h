// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Class.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Pickup		UMETA(DisplayName = "Pickup"),
	EWS_Equippped	UMETA(DisplayName = "Equipped"), 
	EWS_MAX			UMETA(DisplayName = "DefaultMax") 
};
/**
 *
 */
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

		virtual void OnOverlapBegin(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		) override;

		virtual void OnOverlapEnd
		(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		) override;

		void Equip(class AMain* Char);

		FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
		FORCEINLINE EWeaponState GetWeaponState() { return WeaponState;  }
};

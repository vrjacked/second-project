// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SECONDPROJECT_API AMainPlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    // Reference to the UMG asset in the editor
    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Widgets")
    TSubclassOf<class UUserWidget> HUDOverlayAsset;


    // Variable to hold the widget after creating it
    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Widgets")
    UUserWidget* HUDOverlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    TSubclassOf<UUserWidget> WEnemyHealthBar;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Widgets")
    UUserWidget* EnemyHealthBar;

    bool bEnemyHealthBarVisible;

    void DisplayEnemyHealthBar();
    void RemoveEnemyHealthBar();

    FVector EnemyLocation; 
    
protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;
};

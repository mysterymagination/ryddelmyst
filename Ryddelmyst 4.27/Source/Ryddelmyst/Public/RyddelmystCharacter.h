// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FawnCharacter.h"
#include "LookitYouPawn.h"
#include "Snowball.h"
#include "Item.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "RyddelmystCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;

UCLASS(config=Game)
class ARyddelmystCharacter : public AFawnCharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCameraComponent;

	/** Handle to an Actor the Character grabbed during interaction event */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	AActor* GrabbedActor;

	/** Handle to the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	class ARyddelmystHUD* HUD;

private:
	UPROPERTY()
	bool FirstPersonCameraMode = true;
	UPROPERTY()
	bool IsRunning = false;
	UPROPERTY()
	uint8 SelectedWeaponIdx = 0;
	UPROPERTY()
	FTimeline MyTimeline;
	UPROPERTY()
	float CurveFloatValue;
	UPROPERTY()
	float TimelineValue;
	UPROPERTY()
	FTimerHandle MagicTimerHandle;
	UPROPERTY()
	FTimerHandle InvincibilityTimerHandle;
	UPROPERTY()
	FTimerHandle InventoryTimerHandle;
	UPROPERTY()
	TArray<UObject*> Inventory;
	UPROPERTY()
	uint8 SelectedItemIdx = 0;
	UPROPERTY()
	bool IsInventorySleeping = false;

public:
	ARyddelmystCharacter();
	virtual void OnLostFollower(ILookitYou* lookitYou) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseLookUpRate;

	/** Handle to our LookitYou impl, which will be spawned when user inputs Free Cam Mode action.  */
	UPROPERTY()
	class ALookitYouPawn* LookitYouGo;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	// Distance at which the player can interact with objects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MaxInteractDistance = 200.f;

	// Max number of items that can be held in the inventory.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	uint8 MaxInventory = 4;

	// Distance at which the player will carry objects out from their mesh on X axis.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float CarryDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float FullMagic = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float Magic = FullMagic;

protected:
	// List of weapons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TArray<TSubclassOf<ASnowball>> Spells;

protected:
	/** Handles switching between our cam and the LookitYouPawn's "3rd person" cam */
	void CameraToggle();

	/** Handles switching player input control over to LookitYouGo */
	void SendControl();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to Run toggle event
	 */
	void Run();

	/**
	 * Called via input to Cycle Weapon Up event
	 */
	void CycleWeaponUp();

	/**
	 * Called via input to Cycle Weapon Down event
	 */
	void CycleWeaponDown();

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void LookUp(float Value);

	/**
	 * Pauses the game and raises the pause menu.
	 */ 
	UFUNCTION()
	void PauseGame();

	/**
	 * Handles firing projectiles.
	 */ 
	UFUNCTION()
	void Fire();

	/**
	 * Handles activating interactable objects.
	 */
	UFUNCTION()
	void Interact();

	/**
	 * Handles crouch position toggle.
	 */
	UFUNCTION()
	void HandleCrouch();

	/**
	 * Traces an interact ray from the first person camera and returns the hit result
	 * @return HitResult a HitResult containing any Actor hit by the interact ray
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FHitResult FireInteractRay();

	/**
	 * Adds a given Item to the inventory.
	 * @param ItemActor the Actor representing the item in the stage, whose ItemType field defines what actual Item object will be added to the player inventory.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddInventoryItemFromActor(class AItemActor* ItemActor);

	/**
	 * Adds a given Item to the inventory.
	 * @param ItemObj the Item object that will be added to the player inventory.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddInventoryItem(UObject* ItemObj);

	/** Update Health data and UI */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealth(float HealthChange);

	/** Update Magic data and UI */
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void UpdateMagic(float MagicChange);

	UFUNCTION()
	void HandleDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, class UDamageType* DamageType, AActor* DamageCauser);

	/** Damage Timer starts the invincibility clock after taking damage */
	UFUNCTION()
	void DamageInvincibilityTimer();

	/** Marks that the actor can be damaged again */
	UFUNCTION()
	void SetDamageState();

	/**
	 * Runs the selected Item's OnUse() iff an Item is selected.
	 */
	UFUNCTION()
	void UseItem();

	/**
	 * Cycles through the player's Inventory, selecting an item at the incremented index if the Value is positive and decremented index if Value is negative
	 * @param Value negative to cycle backwards/down and positive to cycle forwards/up
	 */
	UFUNCTION()
	void CycleItem(float Value);

	/** Wakes the inventory system to user input */
	UFUNCTION()
	void WakeInventory();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Get Health */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth();

	/** Get full Health value */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth();

	/** Get Health for display */
	UFUNCTION(BlueprintPure, Category = "Health")
	FText GetHealthText();

	/** Get Magic */
	UFUNCTION(BlueprintPure, Category = "Magic")
	float GetMagic();

	/** Get full Magic value */
	UFUNCTION(BlueprintPure, Category = "Magic")
	float GetMaxMagic();

	/** Get Magic for display */
	UFUNCTION(BlueprintPure, Category = "Magic")
	FText GetMagicText();

private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};


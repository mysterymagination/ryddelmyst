// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FawnCharacter.h"
#include "LookitYouPawn.h"
#include "Snowball.h"
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

private:
	bool FirstPersonCameraMode = true;
	bool IsRunning = false;
	uint8_t SelectedWeaponIdx = 0;
	FTimeline MyTimeline;
	float CurveFloatValue;
	float TimelineValue;
	FTimerHandle MagicTimerHandle;

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

	// Distance at which the player will carry objects out from their mesh on X axis.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float CarryDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float RedFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float FullMagic = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float Magic = FullMagic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	UCurveFloat* MagicCurve;

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

	/** Get Health */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth();

	/** Get Magic */
	UFUNCTION(BlueprintPure, Category = "Magic")
	float GetMagic();

	/** Update Health data and UI */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealth(float HealthChange);

	/** Update Magic data and UI */
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void UpdateMagic(float MagicChange);

	/** Damage Timer */
	UFUNCTION()
	void DamageTimer();

	/** Play damage flash VFX */
	UFUNCTION(BlueprintPure, Category = "Health")
	bool PlayFlash();

	UFUNCTION()
	void HandleDamage(float Damage, const FHitResult& HitInfo);

	/** Callback run when our magic recharge timeline ticks */
	UFUNCTION()
	void OnMagicRechargeTick();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};


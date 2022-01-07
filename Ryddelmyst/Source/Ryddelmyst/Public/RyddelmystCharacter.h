// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FawnCharacter.h"
#include "LookitYouPawn.h"
#include "Snowball.h"
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

private:
	bool FirstPersonCameraMode = true;

public:
	ARyddelmystCharacter();
	virtual void OnLostFollower(ILookitYou* lookitYou) override;

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseLookUpRate;

	/** Handle to our LookitYou impl, the camerafolk; expected to be set in Editor or BP to e.g. a LookitYouPawn  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	class ALookitYouPawn* LookitYouGo;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

protected:
	// Projectile class to spawn, naturally it must be some variation of the very best Pathfinder spell
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<ASnowball> ProjectileClass;

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
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};


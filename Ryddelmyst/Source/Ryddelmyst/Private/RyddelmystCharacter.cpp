// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ARyddelmystCharacter

ARyddelmystCharacter::ARyddelmystCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	for first person perspective
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 50.f + BaseEyeHeight)); // Position the camera slightly above eye level and at about the front of the mesh's face
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetActive(true);

	// Create a CameraComponent	for third person perspective
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FVector thirdPersonOffset = FVector(-200.00f, 0.0f, 50.f + BaseEyeHeight);
	FRotator cameraOffsetCCWYaw = FRotator(0.0, -45.0, 0.0);
	thirdPersonOffset = cameraOffsetCCWYaw.RotateVector(thirdPersonOffset);
	ThirdPersonCameraComponent->SetRelativeLocation(thirdPersonOffset); // Position the camera well above eye level and behind the mesh's head in quadrant 4 of a plane perpendicular to the character's height axis (Z)
	// todo: seems like the runtime camera view isn't tied to the camera mesh's facing?
	FRotator cameraLocalCCWYaw = FRotator(0.0, -45.0, 0.0);
	ThirdPersonCameraComponent->SetRelativeRotation(cameraLocalCCWYaw); // point the camera at our mesh by matching its own relative rotation to the angle we used to rotate its offset vector (plus some more to take our mesh's glory in a little more clearly)
	ThirdPersonCameraComponent->bUsePawnControlRotation = true;
	ThirdPersonCameraComponent->SetActive(false);
}

void ARyddelmystCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are riddled with RyddelmystCharacter!"));

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay; first person cam rel rotation is %s and world rotation is %s.  Third person cam rel rotation is %s and world rotation is %s.  Capsule rel rotation is %s and world rotation is %s."),
		*FirstPersonCameraComponent->GetRelativeRotation().ToString(),
		*FirstPersonCameraComponent->GetComponentRotation().ToString(),
		*ThirdPersonCameraComponent->GetRelativeRotation().ToString(),
		*ThirdPersonCameraComponent->GetComponentRotation().ToString(),
		*GetCapsuleComponent()->GetRelativeRotation().ToString(),
		*GetCapsuleComponent()->GetComponentRotation().ToString());

}

//////////////////////////////////////////////////////////////////////////
// Input

void ARyddelmystCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind camera toggle events
	PlayerInputComponent->BindAction("CameraToggle", IE_Released, this, &ARyddelmystCharacter::CameraToggle);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ARyddelmystCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARyddelmystCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARyddelmystCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARyddelmystCharacter::LookUpAtRate);
}

void ARyddelmystCharacter::CameraToggle()
{
	if (FirstPersonCameraMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraToggle; going to third person cam, whose rel rotation is %s and world rotation is %s.  Capsule rel rotation is %s and world rotation is %s."),
			*ThirdPersonCameraComponent->GetRelativeRotation().ToString(),
			*ThirdPersonCameraComponent->GetComponentRotation().ToString(),
			*GetCapsuleComponent()->GetRelativeRotation().ToString(),
			*GetCapsuleComponent()->GetComponentRotation().ToString());
		FirstPersonCameraComponent->SetActive(false);
		ThirdPersonCameraComponent->SetActive(true);
		FirstPersonCameraMode = false;
	} 
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraToggle; going to first person cam, whose rel rotation is %s and world rotation is %s.  Capsule rel rotation is %s and world rotation is %s."),
			*FirstPersonCameraComponent->GetRelativeRotation().ToString(),
			*FirstPersonCameraComponent->GetComponentRotation().ToString(),
			*GetCapsuleComponent()->GetRelativeRotation().ToString(),
			*GetCapsuleComponent()->GetComponentRotation().ToString());
		FirstPersonCameraComponent->SetActive(true);
		ThirdPersonCameraComponent->SetActive(false);
		FirstPersonCameraMode = true;
	}
}

void ARyddelmystCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ARyddelmystCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ARyddelmystCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARyddelmystCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


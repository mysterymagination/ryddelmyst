// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	ThirdPersonCameraComponent->SetRelativeLocation(FVector(-239.56f, 1.75f, 100.f + BaseEyeHeight)); // Position the camera slightly above eye level and at about the front of the mesh's face
	ThirdPersonCameraComponent->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	ThirdPersonCameraComponent->bUsePawnControlRotation = false;
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

	// Bind LookitYou control mode
	PlayerInputComponent->BindAction("Free Cam Mode", IE_Released, this, &ARyddelmystCharacter::SendControl);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ARyddelmystCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARyddelmystCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARyddelmystCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ARyddelmystCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARyddelmystCharacter::LookUpAtRate);
	PlayerInputComponent->BindAction("Cycle Weapon Up", IE_Released, this, &ARyddelmystCharacter::CycleWeaponUp);
	PlayerInputComponent->BindAction("Cycle Weapon Down", IE_Released, this, &ARyddelmystCharacter::CycleWeaponDown);

	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ARyddelmystCharacter::Fire);
}

void ARyddelmystCharacter::SendControl()
{
	if (!LookitYouGo)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		LookitYouGo = GetWorld()->SpawnActor<ALookitYouPawn>(ThirdPersonCameraComponent->GetComponentLocation(), ThirdPersonCameraComponent->GetComponentRotation(), SpawnParams);
	}
	if (LookitYouGo)
	{
		LookitYouGo->SetFollowCharacter(this);
		LookitYouGo->TakeControl();
	}
}

void ARyddelmystCharacter::CameraToggle()
{
	UE_LOG(LogTemp, Warning, TEXT("CameraToggle; 1PP cam rotation is %s"),  *FirstPersonCameraComponent->GetComponentRotation().ToString());
	if (FirstPersonCameraMode)
	{
		FirstPersonCameraMode = false;
		FirstPersonCameraComponent->SetActive(false);
		ThirdPersonCameraComponent->SetActive(true);
	}
	else
	{
		ThirdPersonCameraComponent->SetActive(false);
		FirstPersonCameraComponent->SetActive(true);
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

void ARyddelmystCharacter::LookUp(float Value)
{
	if (Value != 0.0f)
	{
		// Only send controller pitch to CharacterMovementComponent if we're in 1PP.  In 3PP, manually pitch the 1PP cam based on controller input since 1PP cam controls spawn location of projectiles; it is essentially our 'muzzle'.  CharacterMovementComponent seems to only consider forwarding pitch to 1PP cam if it is active i.e. we are in 1PP mode, which is why we're manually applying pitch to 1PP cam while we're in 3PP.
		if (FirstPersonCameraMode)
		{
			AddControllerPitchInput(Value);
		}
		else
		{
			FRotator currentRotation = FirstPersonCameraComponent->GetComponentRotation();
			// We don't want any Roll happening, and for some reason when I use AddWorldRotation I get Roll even if I specify 0.f for Yaw and Roll params.  I noticed that in FPP the Roll seems to stay almost 0 forever, so I guess a direct set to 0 here is fine enough.
			// Current Pitch minus Value allows us to have mouse movement towards you => target pitch down and mouse movement away from you => pitch up like we have in FPP.
			FirstPersonCameraComponent->SetWorldRotation(FRotator(currentRotation.Pitch-Value, currentRotation.Yaw, 0.f));
		}
	}
}

void ARyddelmystCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARyddelmystCharacter::CycleWeaponUp()
{
	UE_LOG(LogTemp, Warning, TEXT("CycleWeaponUp"));
	if (Spells.Num() > 0)
	{	
		if (SelectedWeaponIdx < Spells.Num() - 1)
		{
			SelectedWeaponIdx++;
		}
		else
		{
			SelectedWeaponIdx = 0;
		}
	}
}

void ARyddelmystCharacter::CycleWeaponDown()
{
	UE_LOG(LogTemp, Warning, TEXT("CycleWeaponDown"));
	if (Spells.Num() > 0)
	{
		if (SelectedWeaponIdx > 0)
		{
			SelectedWeaponIdx--;
		}
		else
		{
			SelectedWeaponIdx = Spells.Num() - 1;
		}
	}
}

void ARyddelmystCharacter::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire; attempting to fire snowball... using weapon idx %u"), SelectedWeaponIdx);
	// Attempt to fire a projectile.
	if (SelectedWeaponIdx < Spells.Num())
	{
		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = FirstPersonCameraComponent->GetComponentLocation() + FTransform(FirstPersonCameraComponent->GetComponentRotation()).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = FirstPersonCameraComponent->GetComponentRotation();
		// todo: so what are we doing here exactly?  We've got the world space rotation of the firstpersoncameracomponent and then we're goosing its pitch, but what are we rotating 10 degrees around?  The Y axis, sure, but is that axis considered to run through world origin (implying we take our offset from origin into account somewhere) or does it run through the MuzzleLocation point?  Since the definition of a point in world space is (I think) a vector from origin to an endpoint, maybe there's no distinction?  But what about the arc length -- doesn't vector magnitude i.e. how far the point is away from world origin modify the length of the arc curve that an N degree rotation causes?  EDIT: yeah it does; arc length is radius*theta where theta is the rotation angle in radians and the radius here is our vector magnitude. I guess the point of interest is how we use this MuzzleRotation, and the answer to that is wrapped up in World->SpawnActor() below...
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			ASnowball* Snowball = World->SpawnActor<ASnowball>(Spells[SelectedWeaponIdx], MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Snowball)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Snowball->FireInDirection(LaunchDirection);
			}
		}
	}
}

void ARyddelmystCharacter::OnLostFollower(ILookitYou* lookitYou)
{
	// move the 3PP camera to the last location and rotation of the lookityou
	ThirdPersonCameraComponent->SetWorldLocationAndRotation(lookitYou->GetLocation(), lookitYou->GetRotation());
	// nullify the lookityou member
	LookitYouGo = nullptr;
}
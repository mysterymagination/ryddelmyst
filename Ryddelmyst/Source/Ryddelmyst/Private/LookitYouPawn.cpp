// Fill out your copyright notice in the Description page of Project Settings.

#include "LookitYouPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ALookitYouPawn::ALookitYouPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create a camera and attach to our root component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("LookitCam"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(RootComponent);
	Camera->SetActive(true);
	Movement = CreateDefaultSubobject<ULookitYouMovementComponent>(TEXT("LookitYouMove"));
	Movement->SetUpdatedComponent(RootComponent);
}

// Called when the game starts or when spawned
void ALookitYouPawn::BeginPlay()
{
	Super::BeginPlay();
	if (FollowCharacter)
	{
		AttachToActor(FollowCharacter, FAttachmentTransformRules::KeepWorldTransform);
	}
}

// Called to bind functionality to input
void ALookitYouPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent; LookitYouPawn receiving input"));
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Free Cam Mode", IE_Released, this, &ALookitYouPawn::EndFlyAbout);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &ALookitYouPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALookitYouPawn::MoveRight);
	InputComponent->BindAxis("Levitator", this, &ALookitYouPawn::MoveUp);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("Orbit", this, &ALookitYouPawn::Orbit);
}

void ALookitYouPawn::MoveForward(float AxisValue)
{
	if (AxisValue != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), AxisValue);
	}
}

void ALookitYouPawn::MoveRight(float AxisValue)
{
	if (AxisValue != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), AxisValue);
	}
}

void ALookitYouPawn::MoveUp(float AxisValue)
{
	if (AxisValue != 0.0f)
	{
		AddMovementInput(GetActorUpVector(), AxisValue);
	}
}

void ALookitYouPawn::Orbit(float AxisValue)
{
	// LookitYouMovementComponent orbit impl tracks the orbit scale factor (-1, 0, or 1) and applies it in TickComponent; we need to receive 0 AxisValue to know when to turn orbitting off
	Movement->Orbit(AxisValue);
}

void ALookitYouPawn::EndFlyAbout()
{
	if (FollowCharacter)
	{
		// switch player possession back to ryddelmyst character
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		UE_LOG(LogTemp, Warning, TEXT("FlyAbout; while attempting to give up control, we find GetController returns %p"), controller);
		controller->UnPossess();
		controller->Possess(FollowCharacter);
		UE_LOG(LogTemp, Warning, TEXT("FlyAbout; attempting to repossess follow pawn %p"), FollowCharacter);
		// tell the FawnPawn that we're sending control back its way; the RyddelmystCharacter impl will respond by moving its 3PP cam to the last LookitYouPawn location and setting its LookitYou handle to nullptr 
		FollowCharacter->OnLostFollower(this);
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FlyAbout; tried to fly LookitYouPawn, but he doesn't have a FollowCharacter set to detach/attach back to."));
	}
}

void ALookitYouPawn::TakeControl()
{
	UE_LOG(LogTemp, Warning, TEXT("TakeControl; LookitYouPawn hopping into pilot's chair!"));
	// switch player possession to this LookitYouPawn
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	UE_LOG(LogTemp, Warning, TEXT("FlyAbout; while attempting to give up control, we find GetController returns %p"), controller);
	controller->UnPossess();
	controller->Possess(this);
}

void ALookitYouPawn::EnableCamera(bool enable){}

FVector ALookitYouPawn::GetLocation() 
{
	return GetActorLocation();
}

FRotator ALookitYouPawn::GetRotation()
{
	return GetActorRotation();
}

void ALookitYouPawn::SetFollowCharacter(AFawnCharacter* followCharacter)
{
	FollowCharacter = followCharacter;
}

AFawnCharacter* ALookitYouPawn::GetFollowCharacter()
{
	return FollowCharacter;
}



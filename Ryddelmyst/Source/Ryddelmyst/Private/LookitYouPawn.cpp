// Fill out your copyright notice in the Description page of Project Settings.


#include "LookitYouPawn.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "IounTorch.h"
#include "MicroMeteor.h"

// Sets default values
ALookitYouPawn::ALookitYouPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Use a spring arm to give the camera smooth, natural-feeling motion.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("LookitCameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->TargetArmLength = FCameraArmLength;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;
	// Create a camera and attach to our spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("LookitCam"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ALookitYouPawn::BeginPlay()
{
	Super::BeginPlay();
	SpringArm->TargetArmLength = FCameraArmLength;
}

// Called to bind functionality to input
void ALookitYouPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("FlyAbout", IE_Released, this, &ALookitYouPawn::FlyAbout);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &ALookitYouPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALookitYouPawn::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &ALookitYouPawn::MoveUp);
	InputComponent->BindAxis("Turn", this, &ALookitYouPawn::Turn);
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

void ALookitYouPawn::Turn(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	// rot around Z, so turning 'round from our actor's perspective
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

void ALookitYouPawn::FlyAbout()
{
	if (CameraActive)
	{
		if (FollowPawn)
		{
			if (FollowMode)
			{
				// detach this LookitYouPawn from the follow pawn
				DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				// switch player possession from the follow pawn to this LookitYouPawn
				GetController()->UnPossess();
				GetController()->Possess(this);
				FollowMode = false;
			}
			else
			{
				// switch player possession from the this LookitYouPawn back to ryddelmyst character
				GetController()->UnPossess();
				GetController()->Possess(FollowPawn);
				// tell this LookitYouPawn to attach to ryddelmyst character again
				AttachToActor(FollowPawn, FAttachmentTransformRules::KeepWorldTransform);
				FollowMode = true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FlyAbout; tried to fly LookitYouPawn, but he doesn't have a FollowPawn set to detach/attach back to."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FlyAbout; tried to fly LookitYouPawn, but his camera is not active.  Please toggle camera to LookitYouPawn first so he can see where he's going when he takes flight!"));
	}
}

void ALookitYouPawn::EnableCamera(bool enable)
{
	Camera->SetActive(enable);
	CameraActive = enable;
}



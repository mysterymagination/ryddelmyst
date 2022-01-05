// Fill out your copyright notice in the Description page of Project Settings.

#include "LookitYouPawn.h"
#include "RyddelmystCharacter.h"
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
	Camera->SetActive(false);
	Movement = CreateDefaultSubobject<ULookitYouMovementComponent>(TEXT("LookitYouMove"));
	Movement->SetUpdatedComponent(RootComponent);
}

// Called when the game starts or when spawned
void ALookitYouPawn::BeginPlay()
{
	Super::BeginPlay();
	if (FollowPawn)
	{
		AttachToActor(FollowPawn, FAttachmentTransformRules::KeepWorldTransform);
	}
}

// Called to bind functionality to input
void ALookitYouPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent; LookitYouPawn receiving input"));
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Free Cam Mode", IE_Released, this, &ALookitYouPawn::FlyAbout);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &ALookitYouPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALookitYouPawn::MoveRight);
	InputComponent->BindAxis("Levitator", this, &ALookitYouPawn::MoveUp);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ALookitYouPawn::MirrorControllerPitch);
	InputComponent->BindAxis("Orbit", this, &ALookitYouPawn::Orbit);
}

void ALookitYouPawn::MirrorControllerPitch(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
	
	if (FollowPawn)
	{
		/* this didn't work presumably because the FollowPawn is not currently possessed, so the framework drops controller inputs to it
		FollowPawn->AddControllerPitchInput(AxisValue);
		*/
		FollowPawn->GetFirstPersonCamera()->AddRelativeRotation(FQuat(0.f, AxisValue, 0.f, 0.f));
	}
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

void ALookitYouPawn::FlyAbout()
{
	UE_LOG(LogTemp, Warning, TEXT("FlyAbout; LookitYouPawn should be in free fly mode"));
	if (CameraActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlyAbout; parent actor says %p"), GetParentActor());
		UE_LOG(LogTemp, Warning, TEXT("FlyAbout; owner actor says %p"), this->GetOwner());
		if (FollowPawn)
		{
			if (FollowMode)
			{
				// detach this LookitYouPawn from the follow pawn
				DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				// switch player possession from the follow pawn to this LookitYouPawn
				APlayerController* controller = GetWorld()->GetFirstPlayerController();
				controller->UnPossess();
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; while attempting to take control, we find GetController returns %p"), controller);
				controller->Possess(this);
				FollowMode = false;
			}
			else
			{
				AttachToActor(FollowPawn, FAttachmentTransformRules::KeepWorldTransform);
				// switch player possession from the this LookitYouPawn back to ryddelmyst character
				APlayerController* controller = GetWorld()->GetFirstPlayerController();
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; while attempting to give up control, we find GetController returns %p"), controller);
				controller->UnPossess();
				controller->Possess(FollowPawn);
				// tell this LookitYouPawn to attach to ryddelmyst character again
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; attempting to reattach to follow pawn %p"), FollowPawn);
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; our parent actor is %p"), GetParentActor());
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; our owner actor %p"), GetOwner());
				// the unpossess/possess shuffle above seems to want to switch viewtarget implicitly, so switch it back
				GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this, 0.0F, EViewTargetBlendFunction::VTBlend_Linear);
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
	UE_LOG(LogTemp, Warning, TEXT("EnableCamera; LookitYouPawn camera %s"), enable ? TEXT("activating") : TEXT("deactivating"));
	Camera->SetActive(enable);
	CameraActive = enable;
}

void ALookitYouPawn::TakeControl()
{
	UE_LOG(LogTemp, Warning, TEXT("TakeControl; LookitYouPawn hopping into pilot's chair!"));
	FlyAbout();
}

void ALookitYouPawn::SetFollowPawn(ARyddelmystCharacter* followPawn)
{
	FollowPawn = followPawn;
}

ARyddelmystCharacter* ALookitYouPawn::GetFollowPawn()
{
	return FollowPawn;
}



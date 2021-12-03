// Fill out your copyright notice in the Description page of Project Settings.

#include "LookitYouPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetActive(false);
}

// Called when the game starts or when spawned
void ALookitYouPawn::BeginPlay()
{
	Super::BeginPlay();
	
	SpringArm->TargetArmLength = FCameraArmLength;
	if (FollowPawn)
	{
		AttachToActor(FollowPawn, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ALookitYouPawn::Tick(float DeltaSeconds)
{

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
	UE_LOG(LogTemp, Warning, TEXT("FlyAbout; LookitYouPawn should be in free fly mode"));
	if (CameraActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlyAbout; parent actor says %p"), GetParentActor());
		UE_LOG(LogTemp, Warning, TEXT("FlyAbout; owner actor says %p"), this->GetOwner());
		if (FollowPawn)
		{
			if (FollowMode)
			{
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; while attempting to take control, we find GetController returns %p"), GetController());
				// detach this LookitYouPawn from the follow pawn
				DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				// switch player possession from the follow pawn to this LookitYouPawn
				AController* controller = GetController();
				// unpossess seems to invalidate the result of GetController(), so we have to store the pointer
				controller->UnPossess();
				controller->Possess(this);
				FollowMode = false;
			}
			else
			{
				// switch player possession from the this LookitYouPawn back to ryddelmyst character
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; while attempting to give up control, we find GetController returns %p"), GetController());
				AController* controller = GetController();
				controller->UnPossess();
				controller->Possess(FollowPawn);
				// tell this LookitYouPawn to attach to ryddelmyst character again
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; attempting to reattach to follow pawn %p"), FollowPawn);
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; our parent actor is %p"), GetParentActor());
				UE_LOG(LogTemp, Warning, TEXT("FlyAbout; our owner actor %p"), GetOwner());
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
	UE_LOG(LogTemp, Warning, TEXT("EnableCamera; LookitYouPawn camera %s"), enable ? TEXT("activating") : TEXT("deactivating"));
	Camera->SetActive(enable);
	CameraActive = enable;
}

void ALookitYouPawn::TakeControl()
{
	UE_LOG(LogTemp, Warning, TEXT("TakeControl; LookitYouPawn hopping into pilot's chair!"));
	FlyAbout();
}

void ALookitYouPawn::SetFollowPawn(APawn* followPawn)
{
	FollowPawn = followPawn;
}

APawn* ALookitYouPawn::GetFollowPawn()
{
	return FollowPawn;
}



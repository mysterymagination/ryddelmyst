// Fill out your copyright notice in the Description page of Project Settings.


#include "MollyPawn.h"
#include "Camera/CameraComponent.h"

// Sets default values
AMollyPawn::AMollyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// Dummy root Component we can attach things to
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// Create camera and visible object to represent our Molly
	UCameraComponent* MollyCam = CreateDefaultSubobject<UCameraComponent>(TEXT("MollyCam"));
	VisibleMollyComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheVisibleMolly"));
	// Attach camera to our root component, and then offset and rotate the camera
	MollyCam->SetupAttachment(RootComponent);
	MollyCam->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
	MollyCam->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	// Attach visible object to our root component
	VisibleMollyComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMollyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMollyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	{
		float CurrentScale = VisibleMollyComponent->GetComponentScale().X;
		if (bGrowing)
		{
			// Grow to double size over the course of one second
			CurrentScale += DeltaTime;
		}
		else
		{
			// Shrink half as fast as we grow
			CurrentScale -= (DeltaTime * 0.5f);
		}
		// Make sure we never drop below our starting size, or increase past double size.
		CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 2.0f);
		VisibleMollyComponent->SetWorldScale3D(FVector(CurrentScale));
	}

	// Handle movement based on our "MoveX" and "MoveY" axes
	{
		if (!CurrentVelocity.IsZero())
		{
			FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
			SetActorLocation(NewLocation);
		}
	}

}

// Called to bind functionality to input
void AMollyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Respond when our "Grow" key is pressed or released.
	InputComponent->BindAction("Grow", IE_Pressed, this, &AMollyPawn::StartGrowing);
	InputComponent->BindAction("Grow", IE_Released, this, &AMollyPawn::StopGrowing);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &AMollyPawn::Move_XAxis);
	InputComponent->BindAxis("MoveRight", this, &AMollyPawn::Move_YAxis);
}

void AMollyPawn::Move_XAxis(float AxisValue)
{
	// Move at 100 units per second forward or backward
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void AMollyPawn::Move_YAxis(float AxisValue)
{
	// Move at 100 units per second right or left
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f;
}

void AMollyPawn::StartGrowing()
{
	bGrowing = true;
}

void AMollyPawn::StopGrowing()
{
	bGrowing = false;
}


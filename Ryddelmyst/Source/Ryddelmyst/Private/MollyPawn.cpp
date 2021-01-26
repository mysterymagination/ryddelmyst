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

}

// Called to bind functionality to input
void AMollyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


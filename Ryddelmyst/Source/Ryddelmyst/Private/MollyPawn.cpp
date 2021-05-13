// Fill out your copyright notice in the Description page of Project Settings.


#include "MollyPawn.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "IounTorch.h"
#include "MicroMeteor.h"

// Sets default values
AMollyPawn::AMollyPawn() :
bGrowing(false)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// Sphere shape will serve as our root component
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("MollyPawnPresence"));
	// Create and position a mesh component so we can see where our spherical Molly is
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheVisibleMolly"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		// TODO: why is it necessary to move the sphere mesh 40 units down on Z to line up with the center of the sphere component?
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		// Our sphere component has a radius of 40 units and the startercontent sphere mesh is 50, so scale it down by 20%
		SphereVisual->SetWorldScale3D(FVector(0.8f));
	}
	// Create a particle system that we can activate or deactivate
	MollyParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MollyMovementParticles"));
	MollyParticles->SetupAttachment(SphereVisual);
	MollyParticles->bAutoActivate = false;
	// Offset slighlty from bottom-center of mesh to improve visibility
	// TODO: how come we're moving up from our initial Z pos here and down in the adjustment of the mesh itself above?  What is considered origin in UE4?
	MollyParticles->SetRelativeLocation(FVector(-20.0f, 0.0f, 20.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	if (ParticleAsset.Succeeded())
	{
		MollyParticles->SetTemplate(ParticleAsset.Object);
	}
	 // Use a spring arm to give the camera smooth, natural-feeling motion.
	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;
	// Create a camera and attach to our spring arm
	UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("MollyCam"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	// Create an instance of our movement component, and tell it to update the root.
	MollyMovementComponent = CreateDefaultSubobject<UMollyPawnMovementComponent>(TEXT("MollyMovementComponent"));
	MollyMovementComponent->UpdatedComponent = RootComponent;
	// Give her an ioun torch
	auto* IounTorch = CreateDefaultSubobject<AIounTorch>(TEXT("MollyTorch"));
	IounTorch->SetupAttachment(SphereVisual);
	// todo: how come her bounds stay the same regardless of scaling?
	UE_LOG(LogTemp, Warning, TEXT("AMollyPawn::ctor; molly's measurements %f"), SphereVisual->CalcLocalBounds().SphereRadius);
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
		float CurrentScale = RootComponent->GetComponentScale().X;
		if (bGrowing)
		{
			// todo: migrate fMoveTime to MollyPawnMovementComponent and provide a getter to call here
			float moveTime = MollyMovementComponent->getMoveTime();
			if(moveTime > 0 && moveTime <= kSPECIAL_MOVE_THRESHOLD)
			{
				// Special growth action to 2x immediately
				CurrentScale = 2.0f;
			}
			else
			{
				// Grow to double size over the course of one second
				CurrentScale += DeltaTime;
			}
		}
		else
		{
			// Shrink half as fast as we grow
			CurrentScale -= (DeltaTime * 0.5f);
		}
		// Make sure we never drop below our starting size, or increase past double size.
		CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 2.0f);
		RootComponent->SetWorldScale3D(FVector(CurrentScale));
	}
	UE_LOG(LogTemp, Warning, TEXT("AMollyPawn::Tick; molly pos is %s"), *GetActorLocation().ToString());
}

UPawnMovementComponent* AMollyPawn::GetMovementComponent() const
{
	return MollyMovementComponent;
}

// Called to bind functionality to input
void AMollyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Respond when our "Grow" key is pressed or released.
	InputComponent->BindAction("Grow", IE_Pressed, this, &AMollyPawn::StartGrowing);
	InputComponent->BindAction("Grow", IE_Released, this, &AMollyPawn::StopGrowing);
	InputComponent->BindAction("Sparkle", IE_Pressed, this, &AMollyPawn::ParticleToggle);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &AMollyPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMollyPawn::MoveRight);
	InputComponent->BindAxis("Turn", this, &AMollyPawn::Turn);
}

void AMollyPawn::MoveForward(float AxisValue)
{
	if (MollyMovementComponent && (MollyMovementComponent->UpdatedComponent == RootComponent))
	{
		UE_LOG(LogTemp, Display, TEXT("MollyPawn::MoveForward; forward vector is %s and axisvalue is %f"), *GetActorForwardVector().ToString(), AxisValue);
		MollyMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
	}
}

void AMollyPawn::MoveRight(float AxisValue)
{
	if (MollyMovementComponent && (MollyMovementComponent->UpdatedComponent == RootComponent))
	{
		UE_LOG(LogTemp, Display, TEXT("MollyPawn::MoveRight; right vector is %s and axisvalue is %f"), *GetActorRightVector().ToString(), AxisValue);
		MollyMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
	}
}

void AMollyPawn::Turn(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	// rot around Z, so turning 'round from our actor's perspective
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

void AMollyPawn::StartGrowing()
{
	UE_LOG(LogTemp, Warning, TEXT("StartGrowing"));
	bGrowing = true;
}

void AMollyPawn::StopGrowing()
{
	UE_LOG(LogTemp, Warning, TEXT("StopGrowing"));
	bGrowing = false;
}

void AMollyPawn::ParticleToggle()
{
	UE_LOG(LogTemp, Warning, TEXT("Toggling Particles"));
	if(MollyParticles && MollyParticles->Template)
	{
		MollyParticles->ToggleActive();
	}
}


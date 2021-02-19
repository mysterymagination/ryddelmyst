// Fill out your copyright notice in the Description page of Project Settings.


#include "IounTorchComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values for this component's properties
UIounTorchComponent::UIounTorchComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// create root scenecomponent box that'll be the physical core of our IounTorch, and then attach it to this torch object
	auto* boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("RootBoxComponent"));
	boxComp->InitBoxExtent(FVector(20.0f, 20.0f, 15.0f));
	boxComp->SetCollisionProfileName(TEXT("IounTorchPresence"));
	boxComp->UpdateBodySetup();
	UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::ctor; torch box origin is %s and the extent is %s"), *boxComp->CalcBounds(FTransform()).Origin.ToString(), *boxComp->CalcBounds(FTransform()).BoxExtent.ToString());
	SceneRoot = boxComp;
	SceneRoot->SetupAttachment(this);
	// IounTorch mesh
	UStaticMeshComponent* PyramidVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheVisibleTorch"));
	PyramidVisual->SetupAttachment(SceneRoot);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PyramidVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid"));
	if (PyramidVisualAsset.Succeeded())
	{
		PyramidVisual->SetStaticMesh(PyramidVisualAsset.Object);
		// why does 15 on Z put me at the floor of the box and 0 buts me in the middle?  I thought the extent vector of 15 on Z meant 15 from one endpoint of an edge to the other on Z, such that half would be 7.5?  EDIT: apparently the def of extents here must be that it extends out that far from origin, which is the center, so it extends that far in positive and negative directions on each axis.
		//PyramidVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // centered in box, also the default if no relative location specified
		PyramidVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f)); // sitting on floor of box
		//PyramidVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 15.0f)); // sitting on ceiling of box
		PyramidVisual->SetWorldScale3D(FVector(0.3f));
	}
	UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::ctor; pyramid visual rough sphere bound radius is %f"), PyramidVisual->CalcLocalBounds().SphereRadius);
	// Create a particle system that will stay on all the time
	TorchParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IounTorchParticles"));
	TorchParticles->SetupAttachment(PyramidVisual);
	TorchParticles->bAutoActivate = false;
	// Offset slighlty from bottom-center of mesh to improve visibility
	TorchParticles->SetRelativeLocation(FVector(-20.0f, 0.0f, 10.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	if (ParticleAsset.Succeeded())
	{
		TorchParticles->SetTemplate(ParticleAsset.Object);
	}
}

// Called when the game starts
void UIounTorchComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UIounTorchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto* pOrbitted = this->GetAttachParent();
	// TODO: define our per frame pos according to attach parent, performing one circumnavigation of the actor per second
	if(pOrbitted)
	{
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; orbitted parent is %s"), *pOrbitted->GetFName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; torch parent pos is %s"), *pOrbitted->GetComponentLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; torch pos is %s"), *GetComponentLocation().ToString());
		
		// todo: floating torch - should really move this into a MovementComponent thingy
		FVector NewLocation = GetComponentLocation();
		FRotator NewRotation = GetComponentRotation();
		float RunningTime = GetWorld()->GetTimeSeconds();
		float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
		NewLocation.Z += DeltaHeight * 10;
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; runningtime says %f and deltaheight is %f"), RunningTime, DeltaHeight);
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; orbit offset says %s"), *orbitOffset.ToString());
		float DeltaRotation = DeltaTime * 20.0f; //Rotate by 20 degrees per second
		NewRotation.Yaw += DeltaRotation;
		UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::TickComponent(); rot yaw says %f"), NewRotation.Yaw);
		SetWorldLocationAndRotation(NewLocation, NewRotation);

		// orbit motion
		// todo: rotate around Z axis centered on the orbitted actor by deltatime*360 degrees each frame
		//float orbitRotation = DeltaTime * 20.0f;
		//FRotator offsetVecRot;
		//offsetVecRot.Yaw += orbitRotation;
		//orbitOffset = offsetVecRot.RotateVector(orbitOffset);
		orbitOffset = NewRotation.RotateVector(orbitOffset);
		UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::TickComponent(); orbitOffset says %s"), *orbitOffset.ToString());
		SetRelativeLocation(orbitOffset);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; no parent to orbit yet"));
	}
}


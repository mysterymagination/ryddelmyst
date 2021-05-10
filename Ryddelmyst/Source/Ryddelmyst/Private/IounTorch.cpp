// Fill out your copyright notice in the Description page of Project Settings.


#include "IounTorch.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MicroMeteor.h"


// Sets default values for this component's properties
AIounTorch::AIounTorch() :
OrbitOffset(75.0f, 75.0f, 120.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;
	
	// create root scenecomponent box that'll be the physical core of our IounTorch, and then attach it to this torch object
	auto* boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("RootBoxComponent"));
	boxComp->InitBoxExtent(FVector(20.0f, 20.0f, 15.0f));
	boxComp->SetCollisionProfileName(TEXT("IounTorchPresence"));
	boxComp->UpdateBodySetup();
	UE_LOG(LogTemp, Warning, TEXT("AIounTorch::ctor; torch box origin is %s and the extent is %s"), *boxComp->CalcBounds(FTransform()).Origin.ToString(), *boxComp->CalcBounds(FTransform()).BoxExtent.ToString());
	RootComponent = boxComp;
	// IounTorch mesh
	UStaticMeshComponent* PyramidVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheVisibleTorch"));
	PyramidVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PyramidVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid"));
	if (PyramidVisualAsset.Succeeded())
	{
		PyramidVisual->SetStaticMesh(PyramidVisualAsset.Object);
		// why does 15 on Z put me at the floor of the box and 0 buts me in the middle?  I thought the extent vector of 15 on Z meant 15 from one endpoint of an edge to the other on Z, such that half would be 7.5?  EDIT: apparently the def of extents here must be that it extends out that far from origin, which is the center, so it extends that far in positive and negative directions on each axis.
		//PyramidVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // centered in box, also the default if no relative location specified
		PyramidVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -15.0f)); // sitting on floor of box
		//PyramidVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 15.0f)); // sitting on ceiling of box
		PyramidVisual->SetWorldScale3D(FVector(0.5f));
	}
	UE_LOG(LogTemp, Warning, TEXT("AIounTorch::ctor; pyramid visual rough sphere bound radius is %f"), PyramidVisual->CalcLocalBounds().SphereRadius);
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
void AIounTorch::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AIounTorch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// todo: add a OrbitMovementComponent and query him for our orbit (I guess world) position this frame

	// define our per frame pos according to attach parent, performing circumnavigation at 20 degrees per second
	if(pOrbitted)
	{
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; orbitted parent is %s"), *pOrbitted->GetFName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; torch parent pos is %s"), *pOrbitted->GetComponentLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; torch pos is %s"), *GetActorLocation().ToString());
		
		// floating torch - should really move this into a MovementComponent thingy
		FVector NewLocation = GetActorLocation();
		FRotator NewRotation = GetActorRotation();
		float RunningTime = GetWorld()->GetTimeSeconds();
		float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
		NewLocation.Z += DeltaHeight * 150;
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; runningtime says %f and deltaheight is %f"), RunningTime, DeltaHeight);
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; orbit offset says %s"), *OrbitOffset.ToString());
		float DeltaRotation = DeltaTime * 100.0f; //Rotate by 100 degrees per second
		NewRotation.Yaw += DeltaRotation;
		UE_LOG(LogTemp, Warning, TEXT("AIounTorch::TickComponent(); rot yaw says %f after adding deltatime of %f times 20 (%f)"), NewRotation.Yaw, DeltaTime, DeltaTime*20.0f);  
		// TODO: how come world rotation around Z causes a body to spin in place?  I would expect it to rotate around Z crossing through world origin, and therefore assume a sort of orbitting motion of its own.  Perhaps the concept of world vs. local rotation is different than world vs. local position?  Maybe any given rotation essentially has the axes running through the current rotating body's origin?  But then how does our vector rotation work?  That guy, I think is basically given as a vector with a certain magnitude coming out of world origin who gets rotated to have a certain heading and is then picked up and dropped at the spherical Molly origin such that the torch orbits her and not world origin.
		SetActorLocationAndRotation(NewLocation, NewRotation);

		// orbit motion
		OrbitYaw = DeltaTime * 20.0f;
		FRotator OffsetVecRot(0.0f, 0.0f, 0.0f);
		UE_LOG(LogTemp, Warning, TEXT("AIounTorch::TickComponent; OffsetVecRot says %s"), *OffsetVecRot.ToString());
		OffsetVecRot.Yaw = OrbitYaw;
		OrbitOffset = OffsetVecRot.RotateVector(OrbitOffset);
		UE_LOG(LogTemp, Warning, TEXT("AIounTorch::TickComponent(); OrbitOffset says %s"), *OrbitOffset.ToString());
		SetRelativeLocation(OrbitOffset);

		// meteor proc
		fMeteorSpawnTimer += DeltaTime;
		if (fMeteorSpawnTimer >= fMeteorSpawnInterval && iMeteorCount < iMaxMeteors)
		{
			auto* pMeteor = NewObject<AMicroMeteor>(this);
			pMeteor->setId(++iMeteorCount);
			pMeteor->AttachToActor(this);
			fMeteorSpawnTimer = 0.0f;
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; no parent to orbit yet"));
	}
}

UPrimitiveComponent* AIounTorch::getPhysicality()
{
	return SceneRoot;
}

float AIounTorch::getOrbitYaw()
{
	return OrbitYaw;
}


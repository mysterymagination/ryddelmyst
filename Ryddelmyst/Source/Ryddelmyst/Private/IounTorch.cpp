// Fill out your copyright notice in the Description page of Project Settings.


#include "IounTorch.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MicroMeteor.h"


// Sets default values for this component's properties
AIounTorch::AIounTorch() :
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

	// meteor proc
	fMeteorSpawnTimer += DeltaTime;
	if (fMeteorSpawnTimer >= fMeteorSpawnInterval && iMeteorCount < iMaxMeteors)
	{
		auto* pMeteor = NewObject<AMicroMeteor>(this);
		pMeteor->setId(++iMeteorCount);
		// todo: issue #3 probably aren't going to actually want to attach the meteors to the torch; the whole idea behind the orbitmovementcomponent is to let the editor handle the association between orbited body and orbiting body, to make the whole system much more modular and useful than a hardcoded hierarchy of orbiting stuff.
		pMeteor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		fMeteorSpawnTimer = 0.0f;
	}
}


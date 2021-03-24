// Fill out your copyright notice in the Description page of Project Settings.


#include "MicroMeteorComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "IounTorchComponent.h"

// Sets default values for this component's properties
UMicroMeteorComponent::UMicroMeteorComponent() 
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	/*
	// Sphere shape will serve as our root component
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphereComponent"));
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionProfileName(TEXT("MicroMeteorPresence"));
	SphereComponent->SetupAttachment(this);
	// Create and position a mesh component so we can see where our spherical Molly is
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TheVisibleMolly"));
	SphereVisual->SetupAttachment(SphereComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		// Our sphere component has a radius of 10 units and the startercontent sphere mesh is 50, so scale it down by 80%
		SphereVisual->SetWorldScale3D(FVector(0.2f));
	}
	// Create a particle system that we can activate or deactivate
	MeteorParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MeteorFuryParticles"));
	MeteorParticles->SetupAttachment(SphereVisual);
	MeteorParticles->bAutoActivate = false;
	// visibility offset
	MeteorParticles->SetRelativeLocation(FVector(-20.0f, 0.0f, 20.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	if (ParticleAsset.Succeeded())
	{
		MeteorParticles->SetTemplate(ParticleAsset.Object);
	}

	// todo: trying simple world space positioning for debug
	UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::ctor; setting world pos to about our maze hallway floor"));
	this->SetWorldLocation(FVector(-20.0f, 450.0f, 250.0f));
	*/
}


// Called when the game starts
void UMicroMeteorComponent::BeginPlay()
{
	Super::BeginPlay();

	// Sphere shape will serve as our root component
	USphereComponent* SphereComponent = NewObject<USphereComponent>(this);
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionProfileName(TEXT("MicroMeteorPresence"));
	SphereComponent->SetupAttachment(this);
	SphereComponent->RegisterComponent();
	// Create and position a mesh component so we can see where our spherical Molly is
	UStaticMeshComponent* SphereVisual = NewObject<UStaticMeshComponent>(this);
	SphereVisual->SetupAttachment(SphereComponent);
	UStaticMesh* SphereVisualAsset = LoadObject<UStaticMesh>(GetOuter(), TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	SphereVisual->SetStaticMesh(SphereVisualAsset);
	SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
	// Our sphere component has a radius of 10 units and the startercontent sphere mesh is 50, so scale it down by 80%
	SphereVisual->SetWorldScale3D(FVector(0.2f));
	SphereVisual->RegisterComponent();
}


// Called every frame
void UMicroMeteorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto* pOrbitted = static_cast<UIounTorchComponent*>(this->GetAttachParent());
	if (pOrbitted)
	{
		// init orbit offset based on orbitted body size
		if (OrbitOffset.IsZero())
		{
			FVector orbittedExtent = pOrbitted->getPhysicality()->CalcBounds(FTransform()).BoxExtent;
			OrbitOffset = { 0.0f, 2.0f * orbittedExtent.Y, 1.75f * orbittedExtent.Z };
			UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; orbit offset says %s and orbitted extent says %s"), *OrbitOffset.ToString(), *orbittedExtent.ToString());
		}
		fLifeTimer += DeltaTime;
		UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; lifetimer is now %f"), fLifeTimer);
		if (fLifeTimer >= fMaxLifeTime)
		{
			if (!bLaunched)
			{
				UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; lifetimer is now %f and we're not launched, so launching"), fLifeTimer);
				MeteoricLaunch();
			}
			else
			{
				// take off straight on orthogonal, moving linearly at 100 units/second at our current angle
				//LaunchedOffset += FVector(DeltaTime * LaunchedOffset.X, DeltaTime * LaunchedOffset.Y, 0.0f);
				
				// todo: instead of progressing in world space, I think we want to stay relative to the orbitted body BUT with any rotation transforms removed via FRotator inverse, I guess?
				// get current pOrbitted relative location
				FVector parentRelativeLocation = pOrbitted->GetRelativeLocation();
				// accumulate based on torch yaw
				OrbitAccumulator.Yaw += pOrbitted->getOrbitYaw();
				// invert orbit accumulator rotator
				FRotator InverseTorchOrbit = OrbitAccumulator.GetInverse();
				// rotate the parentRelativeLocation vector by the inverted orbit accumulator rotator such that we have an offset vector from the torch's orbitted body to the position the torch was in when we launched... maybe the orbit accumulator needs to live here and we just publish the Yaw mod we make per frame up in the torch so that we know the rotation that has occurred since launch occurred (which is what we seek to remove) per meteor instance.  Anyway, this newly inverse rotated offset vector becomes our meteor's new LaunchedOffset.
				LaunchedOffset = InverseTorchOrbit.RotateVector(parentRelativeLocation);
				// progress the offset vector accumulator XY by DeltaTime times themselves and call SetRelativeLocation()
				LaunchedOffset += FVector(DeltaTime*LaunchedOffset.X, DeltaTime*LaunchedOffset.Y, 0.0f);
				SetRelativeLocation(LaunchedOffset);

				// todo: alternatively, take a snapshot of the torch and meteor world space locations when launch occurs and call the delta between those two points your launch vector.  Progress that vector fractionally on XY and boom done.

				fLaunchedLifeTimer += DeltaTime;
				UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; we've been launched for %f"), fLaunchedLifeTimer);
				if (fLaunchedLifeTimer >= fMaxLaunchedLifeTime)
				{
					UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; we've been launched for %f, which is quite long enough (gte max launch time of %f) -- self-destruct!"), fLaunchedLifeTimer, fMaxLaunchedLifeTime);
					// todo: explosion particle fx before destroy?
					TArray<USceneComponent*> childrenArray;
					GetChildrenComponents(true, childrenArray);
					for (auto* childComponent : childrenArray)
					{
						childComponent->DestroyComponent();
					}
					DestroyComponent();
				}
				else
				{
					// progress straight launched path
					UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; going off straight... LaunchedOffset says %s"), *LaunchedOffset.ToString());
					SetWorldLocation(LaunchedOffset);
				}
			}
		}
		else
		{
			// orbit motion @ 60 degrees/second
			float OrbitRotation = DeltaTime * 160.0f;
			FRotator OffsetVecRot(0.0f, 0.0f, 0.0f);
			OffsetVecRot.Yaw = OrbitRotation;
			UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; OffsetVecRot says %s"), *OffsetVecRot.ToString());
			OrbitOffset = OffsetVecRot.RotateVector(OrbitOffset);

			// progress orbit path
			UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; still orbitting... OrbitOffset says %s"), *OrbitOffset.ToString());
			SetRelativeLocation(OrbitOffset);
		}		
	}
}

void UMicroMeteorComponent::MeteoricLaunch()
{
	// todo: fire up with fire particle fx
	// acquire launch init relative pos and flip launched trigger
	LaunchedOffset = GetRelativeLocation();
	bLaunched = true;
}


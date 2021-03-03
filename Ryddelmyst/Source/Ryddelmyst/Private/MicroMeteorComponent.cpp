// Fill out your copyright notice in the Description page of Project Settings.


#include "MicroMeteorComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UMicroMeteorComponent::UMicroMeteorComponent() 
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
}


// Called when the game starts
void UMicroMeteorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMicroMeteorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto* pOrbitted = this->GetAttachParent();
	if (pOrbitted)
	{
		// init orbit offset based on orbitted body size
		if (OrbitOffset.IsZero())
		{
			// todo: getting 0s as offset vector comps for some reason
			FVector orbittedExtent = pOrbitted->CalcBounds(FTransform()).BoxExtent;
			OrbitOffset = { 0.0f, 1.25f * orbittedExtent.X, 0.75f * orbittedExtent.Z };
			UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; orbit offset says %s and orbitted extent says %s"), *OrbitOffset.ToString(), *orbittedExtent.ToString());
		}
		fLifeTimer += DeltaTime;
		if (fLifeTimer >= fMaxLifeTime)
		{
			if (!bLaunched)
			{
				MeteoricLaunch();
			}
			else
			{
				// take off straight on orthogonal, moving linearly at 100 units/second at our current angle
				OrbitOffset += FVector(DeltaTime * 100.0f);
				fLaunchedLifeTimer += DeltaTime;
				if (fLaunchedLifeTimer >= fMaxLaunchedLifeTime)
				{
					DestroyComponent();
				}
			}
		}
		else
		{
			// orbit motion @ 60 degrees/second
			float OrbitRotation = DeltaTime * 60.0f;
			FRotator OffsetVecRot(0.0f, 0.0f, 0.0f);
			UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::TickComponent; OffsetVecRot says %s"), *OffsetVecRot.ToString());
			OffsetVecRot.Yaw = OrbitRotation;
			OrbitOffset = OffsetVecRot.RotateVector(OrbitOffset);
			UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::TickComponent(); OrbitOffset says %s"), *OrbitOffset.ToString());
			SetRelativeLocation(OrbitOffset);
		}
	}
}

void UMicroMeteorComponent::MeteoricLaunch()
{
	// todo: fire up with fire particle fx
	bLaunched = true;
}


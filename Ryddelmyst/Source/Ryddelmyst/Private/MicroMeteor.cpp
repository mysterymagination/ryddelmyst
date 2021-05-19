// Fill out your copyright notice in the Description page of Project Settings.


#include "MicroMeteor.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include <string>

// Sets default values for this component's properties
AMicroMeteor::AMicroMeteor() 
{
	PrimaryActorTick.bCanEverTick = true;
	// adjust timing with a little instance-unique rando action
	fMaxLifeTime += FMath::RandRange(-2, 5);
	fMaxLaunchedLifeTime += FMath::RandRange(-2, 2);
	// Sphere shape will serve as our root component
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("MeteorPhysicsSphere"));
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionProfileName(TEXT("MicroMeteorPresence"));
	RootComponent = SphereComponent;
	// Create and position a mesh component so we can see where our spherical Molly is
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeteorVisualSphere"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		// TODO: why is it necessary to move the sphere mesh 40 units down on Z to line up with the center of the sphere component?
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		// Our sphere component has a radius of 10 units and the startercontent sphere mesh is 50, so scale it down by 80%
		SphereVisual->SetWorldScale3D(FVector(0.2f));
	}
	// Add orbity movement
	OrbitMotion = CreateDefaultSubobject<UOrbitMovementComponent>(TEXT("OrbitMovementComponent"));
	OrbitMotion->UpdatedComponent = RootComponent;
}


// Called when the game starts
void AMicroMeteor::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AMicroMeteor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	fLifeTimer += DeltaTime;
	if (fLifeTimer >= fMaxLifeTime)
	{
		if (!bLaunched)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MicroMeteor::Tick; lifetimer is now %f and we're not launched, so launching meteor with id %d"), *FDateTime::UtcNow().ToString(), fLifeTimer, mId);
			MeteoricLaunch();
		}
		else
		{
			// move along launch velocity
			UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::Tick; launched -- continuing launch world coords are %s and we're about to progress by launch vector %s times launch speed %f times dt %f, for meteor id %d"), *GetActorLocation().ToString(), *LaunchVector.ToString(), fLaunchSpeed, DeltaTime, mId);
			SetActorLocation(GetActorLocation() + LaunchVector * fLaunchSpeed * DeltaTime);
			UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::Tick; launched -- continuing launch world coords are %s and we've now progressed by launch vector %s times launch speed %f times dt %f, for meteor id %d"), *GetActorLocation().ToString(), *LaunchVector.ToString(), fLaunchSpeed, DeltaTime, mId);

			// update launched time tracker
			fLaunchedLifeTimer += DeltaTime;
			if (fLaunchedLifeTimer >= fMaxLaunchedLifeTime)
			{
				// todo: explosion particle fx before destroy?
				TArray<USceneComponent*> childrenArray;
				GetComponents(childrenArray, true);
				for (auto* childComponent : childrenArray)
				{
					childComponent->DestroyComponent();
				}
				Destroy();
				UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::Tick; destroying meteor id %d"), mId);
			}
		}
	}	
}

void AMicroMeteor::MeteoricLaunch()
{
	// todo: fire up with fire particle fx
	// todo: lookup the orbitted body location from orbitmovementcomponent
	FVector OrbittedBodyLocation(0.0f);
	// todo: shut down and detach the orbitmovementcomponent
	
	// acquire launch vector via init launch meteor world pos minus launch orbitted body pos and flip launched trigger
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; init launch world coords of meteor are %s and launch world coords of orbitted body are %s so our delta vec to be used for init launch offset vec is %s, for meteor id %d"), *GetActorLocation().ToString(), *OrbittedBodyLocation.ToString(), *(GetActorLocation() - OrbittedBodyLocation).ToString(), mId);
	
	// this diff vector gives us our launch direction, tho its magnitude is arbitrary
	FVector DirectionVector = GetActorLocation() - OrbittedBodyLocation;
	// calculate launch vector as our direction vector reduced to mags 1 so we can apply a simple 1 map unit * a constant N per second speed to finish off our velocity.  We don't want a Z comp in our velocity, so just drop it to 0. 
	// we reduce to mags 1 by following c^2 = a^2 + b^2 where we know c^2 = 1 and therefore c = 1.  With 'a' as run and 'b' as rise and ratio given as rise:run, a = sqrt((ratio^2 + 1)^-1) and b = sqrt(((ratio^-1)^2 + 1)^-1)
	float directionRatio = FMath::Abs(DirectionVector.Y / DirectionVector.X);
	float invDirectionRatio = FMath::Abs(DirectionVector.X / DirectionVector.Y);
	float Mag1X = FMath::Sqrt(1/(FMath::Square(directionRatio) + 1));
	float Mag1Y = FMath::Sqrt(1/(FMath::Square(invDirectionRatio) + 1));
	// renew signage
	Mag1X *= FMath::IsNegativeFloat(DirectionVector.X) ? -1 : 1;
	Mag1Y *= FMath::IsNegativeFloat(DirectionVector.Y) ? -1 : 1;
	if (FMath::IsNaN(Mag1X) || FMath::IsNaN(Mag1Y))
	{
		UE_LOG(LogTemp, Warning, TEXT("TickComponent; launch -- NaN detected for X or Y components, which are %f,%f.  Full details: dirrat is %f, invdirrat is %f, DirectionVector.X is %f, and DirectionVector.Y is %f, for meteor with id %d"), Mag1X, Mag1Y, directionRatio, invDirectionRatio, DirectionVector.X, DirectionVector.Y, mId);
	}
	LaunchVector = FVector(Mag1X, Mag1Y, 0.0f);
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; launch -- vector mag should be 1 for init launch vector %s, re: meteor id %d"), *LaunchVector.ToString(), mId);
	bLaunched = true;
}

void AMicroMeteor::setId(size_t id)
{
	mId = id;
}

size_t AMicroMeteor::getId()
{
	return mId;
}


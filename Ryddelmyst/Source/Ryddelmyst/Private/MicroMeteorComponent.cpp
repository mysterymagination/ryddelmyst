// Fill out your copyright notice in the Description page of Project Settings.


#include "MicroMeteorComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "IounTorchComponent.h"
#include <string>

// Sets default values for this component's properties
UMicroMeteorComponent::UMicroMeteorComponent() 
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMicroMeteorComponent::BeginPlay()
{
	Super::BeginPlay();

	// adjust timing with a little instance-unique rando action
	fMaxLifeTime += FMath::RandRange(-2, 5);
	fMaxLaunchedLifeTime += FMath::RandRange(-2, 2);

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

	// store handle to our orbitted body
	pOrbitted = static_cast<UIounTorchComponent*>(GetAttachParent());
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay; reparent -- pOrbitted says %p"), pOrbitted);
}


// Called every frame
void UMicroMeteorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// init orbit offset based on orbitted body size
	if (OrbitOffset.IsZero())
	{
		FVector orbittedExtent = pOrbitted->getPhysicality()->CalcBounds(FTransform()).BoxExtent;
		OrbitOffset = { 0.0f, 2.0f * orbittedExtent.Y, 1.75f * orbittedExtent.Z };
		UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; orbitoffset says %s and orbitted extent says %s"), *OrbitOffset.ToString(), *orbittedExtent.ToString());
	}
	fLifeTimer += DeltaTime;
	UE_LOG(LogTemp, Warning, TEXT("MicroMeteor::TickComponent; lifetimer is now %f"), fLifeTimer);
	if (fLifeTimer >= fMaxLifeTime)
	{
		if (!bLaunched)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s MicroMeteor::TickComponent; lifetimer is now %f and we're not launched, so launching meteor with id %d"), *FDateTime::UtcNow().ToString(), fLifeTimer, mId);
			MeteoricLaunch();
		}
		else
		{
			// move along launch velocity
			UE_LOG(LogTemp, Warning, TEXT("TickComponent; launched -- continuing launch world coords are %s and we're about to progress by launch vector %s times launch speed %f times dt %f, for meteor id %d"), *GetComponentLocation().ToString(), *LaunchVector.ToString(), fLaunchSpeed, DeltaTime, mId);
			SetWorldLocation(GetComponentLocation() + LaunchVector * fLaunchSpeed * DeltaTime);
			UE_LOG(LogTemp, Warning, TEXT("TickComponent; launched -- continuing launch world coords are %s and we've now progressed by launch vector %s times launch speed %f times dt %f, for meteor id %d"), *GetComponentLocation().ToString(), *LaunchVector.ToString(), fLaunchSpeed, DeltaTime, mId);

			// update launched time tracker
			fLaunchedLifeTimer += DeltaTime;
			if (fLaunchedLifeTimer >= fMaxLaunchedLifeTime)
			{
				// todo: explosion particle fx before destroy?
				TArray<USceneComponent*> childrenArray;
				GetChildrenComponents(true, childrenArray);
				for (auto* childComponent : childrenArray)
				{
					childComponent->DestroyComponent();
				}
				DestroyComponent();
				UE_LOG(LogTemp, Warning, TEXT("TickComponent; destroying meteor id %d"), mId);
			}
		}
	}
	else
	{
		// orbit motion @ 60 degrees/second
		float OrbitRotation = DeltaTime * 160.0f;
		FRotator OffsetVecRot(0.0f, 0.0f, 0.0f);
		OffsetVecRot.Yaw = OrbitRotation;
		OrbitOffset = OffsetVecRot.RotateVector(OrbitOffset);

		// progress orbit path
		SetRelativeLocation(OrbitOffset);
	}		
}

void UMicroMeteorComponent::MeteoricLaunch()
{
	// todo: fire up with fire particle fx
	// detach from orbitted body
	DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	// acquire launch vector via init launch meteor world pos minus launch orbitted body pos and flip launched trigger
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; init launch world coords of meteor are %s and launch world coords of orbitted body are %s so our delta vec to be used for init launch offset vec is %s, for meteor id %d"), *GetComponentLocation().ToString(), *pOrbitted->GetComponentLocation().ToString(), *(GetComponentLocation() - pOrbitted->GetComponentLocation()).ToString(), mId);
	
	/* 
	// so this simple approach of correct direction and whatever mags works a treat, but makes speed aspect of velocity harder to control.
	LaunchVector = GetComponentLocation() - pOrbitted->GetComponentLocation();
	LaunchVector.Z = 0.0f;
	*/

	/*
	// this modified approach attempts to bring our component with the larger abs value down to 1 (or -1) and the one with a smaller abs value to some fractional value, such that we have a vector magnitude *close* to 1 (or -1)
	FVector DirectionVector = GetComponentLocation() - pOrbitted->GetComponentLocation();
	float MaxComponent = FMath::Max(FMath::Abs(DirectionVector.X), FMath::Abs(DirectionVector.Y));
	LaunchVector = FVector(DirectionVector.X/MaxComponent, DirectionVector.Y/MaxComponent, 0.0f);
	*/

	 // todo: this approach should give me vector mag 1 with directionality preserved, but instead I get crazy stuff like teleporting or entirely stationery meteors?
	// this diff vector gives us our launch direction, tho its magnitude is arbitrary
	FVector DirectionVector = GetComponentLocation() - pOrbitted->GetComponentLocation();
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
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; launch -- vector mag should be 1 for %s, re: meteor id %d"), *LaunchVector.ToString(), mId);
	
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; init launch offset says %s"), *LaunchVector.ToString());
	bLaunched = true;
}

void UMicroMeteorComponent::setId(size_t id)
{
	mId = id;
}

size_t UMicroMeteorComponent::getId()
{
	return mId;
}


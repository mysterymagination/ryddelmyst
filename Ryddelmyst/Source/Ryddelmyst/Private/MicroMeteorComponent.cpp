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
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; world coords of meteor are %s and world coords of orbitted body are %s"), *GetComponentLocation().ToString(), *pOrbitted->GetComponentLocation().ToString());
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
			UE_LOG(LogTemp, Warning, TEXT("%s MicroMeteor::TickComponent; lifetimer is now %f and we're not launched, so launching"), *FDateTime::UtcNow().ToString(), fLifeTimer);
			MeteoricLaunch();
		}
		else
		{
			SetWorldLocation(GetComponentLocation() + LaunchVector);

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
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; init launch world coords of meteor are %s and launch world coords of orbitted body are %s so our delta vec to be used for init launch offset vec is %s"), *GetComponentLocation().ToString(), *pOrbitted->GetComponentLocation().ToString(), *(GetComponentLocation() - pOrbitted->GetComponentLocation()).ToString());
	// todo: currently this gives us a meteoric speed equal to the maggie of this diff vector, which is not very convenient -- I'd like to be able to tweak it in the editor to go @ N map units per second and then only the direction will be given by this vector.
	LaunchVector = GetComponentLocation() - pOrbitted->GetComponentLocation();
	// we don't want to progress on Z, so leave launch vector Z at 0
	// assuming we're going to be adding the launch vector to the current
	// world coords each frame.
	LaunchVector.Z = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("TickComponent; init launch offset says %s"), *LaunchVector.ToString());
	bLaunched = true;
}


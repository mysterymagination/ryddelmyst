// Fill out your copyright notice in the Description page of Project Settings.


#include "MicroMeteorComponent.h"

// Sets default values for this component's properties
UMicroMeteorComponent::UMicroMeteorComponent() 
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// todo: setup physics and visuals
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
			FVector orbittedExtent = pOrbitted->CalcBounds(FTransform()).BoxExtent;
			OrbitOffset = { 0.0f, 1.25f * orbittedExtent.X, 0.75f * orbittedExtent.Z };
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


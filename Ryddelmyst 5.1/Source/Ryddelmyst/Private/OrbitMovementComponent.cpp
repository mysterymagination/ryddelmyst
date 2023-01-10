// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbitMovementComponent.h"

// Sets default values for this component's properties
UOrbitMovementComponent::UOrbitMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOrbitMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	// attach this orbitmovementcomponent as a transform child of the orbited body so that our transforms applied here will be relative to it
	if (UpdatedComponent && OrbitedBody)
	{  
		UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::BeginPlay; attaching to orbited body %s at default attach component %p"), *OrbitedBody->GetName(), OrbitedBody->GetDefaultAttachComponent());
		UpdatedComponent->AttachToComponent(OrbitedBody->GetDefaultAttachComponent(), FAttachmentTransformRules::KeepWorldTransform);
		UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::BeginPlay; attached the updatedcomponent to %p"), UpdatedComponent->GetAttachParent());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OrbitMovementComponent::BeginPlay; could not attach to an orbited body because we don't know what our orbitting body is yet (%p) and/or we don't know what our orbited body is yet (%p)"), UpdatedComponent, OrbitedBody);
	}
}


// Called every frame
void UOrbitMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Make sure that everything is still valid, and that we are allowed to move.
	if (!UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	FVector DesiredMovementThisFrame(0.0f);
	FRotator SpinRotation = UpdatedComponent->GetComponentRotation();
	if (IsFloating)
	{
		FVector FloatingLocation = FVector(0.0f, 0.0f, 0.0f);
		float RunningTime = GetWorld()->GetTimeSeconds();
		float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
		FloatingLocation.Z += DeltaHeight * 150 * FloatingSpeed;
		UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::TickComponent; floating -- runningtime says %f, deltaheight is %f, and floating location is %s"), RunningTime, DeltaHeight, *FloatingLocation.ToString());
		DesiredMovementThisFrame += FloatingLocation;
	}
	if (IsSpinning)
	{
		
		// TODO: how come world rotation around Z causes a body to spin in place?  I would expect it to rotate around Z crossing through world origin, and therefore assume a sort of orbitting motion of its own.  Perhaps the concept of world vs. local rotation is different than world vs. local position?  Maybe any given rotation essentially has the axes running through the current rotating body's origin?  But then how does our vector rotation work?  That guy, I think is basically given as a vector with a certain magnitude coming out of world origin who gets rotated to have a certain heading and is then picked up and dropped at the spherical Molly origin such that the torch orbits her and not world origin.
		float DeltaRotation = DeltaTime * 100.0f * SpinningSpeed; //Rotate by 100*N degrees per second
		SpinRotation.Yaw += DeltaRotation;
		UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::TickComponent(); rot yaw says %f after adding deltatime of %f times 100 times SpinningSpeed (%f)"), SpinRotation.Yaw, DeltaTime, DeltaRotation);
	}
	
	// orbit motion -- define our per frame pos according to attach parent, performing circumnavigation at 20 degrees per second
	float OrbitYaw = DeltaTime * 20.0f;
	FRotator OffsetVecRot(0.0f, 0.0f, 0.0f);
	OffsetVecRot.Yaw = OrbitYaw;
	UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::TickComponent; OffsetVecRot says %s"), *OffsetVecRot.ToString());
	OrbitOffset = OffsetVecRot.RotateVector(OrbitOffset);
	UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::TickComponent(); OrbitOffset says %s"), *OrbitOffset.ToString());

	// apply any other translations
	OrbitOffset += DesiredMovementThisFrame;

	// apply the orbit vector
	UpdatedComponent->SetRelativeLocation(OrbitOffset);
	UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::TickComponent(); just updated orbiting body known as %s to be at location %s relative to the orbitted body known as %s at world coords %s"), *UpdatedComponent->GetName(), *UpdatedComponent->GetRelativeLocation().ToString(), *OrbitedBody->GetName(), *OrbitedBody->GetActorLocation().ToString());

	UE_LOG(LogTemp, Warning, TEXT("OrbitMovementComponent::TickComponent(); desired movement this frame is %s and desired rotation this frame is %s"), *DesiredMovementThisFrame.ToString(), *SpinRotation.ToString());
	
	// TODO: how would I apply my translations via safemoveupdatedcomponent, essentially doing them incrementally instead of calculating the final pos and applying all at once with a relative setter?
	 
	// apply non-orbit motion to the orbiting body
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector(0.0f), SpinRotation, true, Hit);

	// If we bumped into something, try to slide along it
	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}

void UOrbitMovementComponent::OnComponentDestroyed(bool bDestroyHierarchy)
{
	Super::OnComponentDestroyed(bDestroyHierarchy);
	if (UpdatedComponent)
	{
		UpdatedComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}

///UPrimitiveComponent* UOrbitMovementComponent::getOrbitedBody()
AActor* UOrbitMovementComponent::getOrbitedBody()
{
	return OrbitedBody;
}

///void UOrbitMovementComponent::setOrbitedBody(UPrimitiveComponent* body)
void UOrbitMovementComponent::setOrbitedBody(AActor* body)
{
	OrbitedBody = body;
}


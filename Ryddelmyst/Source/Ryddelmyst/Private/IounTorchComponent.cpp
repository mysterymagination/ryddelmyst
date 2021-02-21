// Fill out your copyright notice in the Description page of Project Settings.


#include "IounTorchComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values for this component's properties
UIounTorchComponent::UIounTorchComponent() :
OrbitOffset(75.0f, 75.0f, 120.0f)
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
		PyramidVisual->SetWorldScale3D(FVector(0.5f));
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
		
		// floating torch - should really move this into a MovementComponent thingy
		FVector NewLocation = GetComponentLocation();
		FRotator NewRotation = GetComponentRotation();
		float RunningTime = GetWorld()->GetTimeSeconds();
		float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
		NewLocation.Z += DeltaHeight * 150;
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; runningtime says %f and deltaheight is %f"), RunningTime, DeltaHeight);
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; orbit offset says %s"), *OrbitOffset.ToString());
		float DeltaRotation = DeltaTime * 100.0f; //Rotate by 20 degrees per second
		NewRotation.Yaw += DeltaRotation;
		UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::TickComponent(); rot yaw says %f after adding deltatime of %f times 20 (%f)"), NewRotation.Yaw, DeltaTime, DeltaTime*20.0f);
		// TODO: wtf?  when I have this line uncommented, I get a weird accelerating and then decelerating propeller motion out of the torch around the spherical Molly... EDIT: when I comment it, things work great right up until I rotate the actor we're attached to at which point we get our propeller business again but under more controlled circumstances.  Essentially the issue is yaw rotation values approaching 180 or -180 which cause the torch to teleport halfway round the orbitted body every frame.  As we approach 0 this rotation appears to 'slow' because the torch is no longer moving so far per frame.  
		// TODO: how can I keep my floating and rotating torch while also maintaining smooth orbitting motion (and rotate the actor without bothering any of these)?  I'm guessing the main point of interest is in creating a different FRotator for the orbit vector rotation, which isn't based on the rotation of the torch or molly.
		// TODO: how come world rotation around Z causes a body to spin in place?  I would expect it to rotate around Z crossing through world origin, and therefore assume a sort of orbitting motion of its own.  Perhaps the concept of world vs. local rotation is different than world vs. local position?  Maybe any given rotation essentially has the axes running through the current rotating body's origin?  But then how does our vector roation work?  That guy, I think is basically given as a vector with a certain magnitude coming out of world origin who gets rotated to have a certain heading and is then picked up and dropped at the spherical Molly origin such that the torch orbits her and not world origin.
		SetWorldLocationAndRotation(NewLocation, NewRotation);

		// orbit motion
		float OrbitRotation = DeltaTime * 20.0f;
		// todo: our rotation around molly seems odd -- we're basically taking only the overall magnitude of the components in our offset vector into account.  The different components don't seem to actually affect the ultimate position of the torch as expected.  I'd expect 75.0f, 75.0f, 120.0f to put us to the upper forward right of the actor and then we just yaw around her from that point.  Instead, we seem to always be at the same height (basically level with the bottom of the actor) and the only thing that changes is how far away we are from the actor while we yaw around.
		FRotator OffsetVecRot = OrbitOffset.Rotation();
		UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::TickComponent; OffsetVecRot says %s"), *OffsetVecRot.ToString());
		OffsetVecRot.Yaw = OrbitRotation;
		OrbitOffset = OffsetVecRot.RotateVector(OrbitOffset);
		UE_LOG(LogTemp, Warning, TEXT("UIounTorchComponent::TickComponent(); OrbitOffset says %s"), *OrbitOffset.ToString());
		SetRelativeLocation(OrbitOffset);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("IounTorch::TickComponent; no parent to orbit yet"));
	}
}


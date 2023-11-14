// Fill out your copyright notice in the Description page of Project Settings.

#include "Snowball.h"
#include "RyddelmystCharacter.h"
#include "MathUtils.h"
#include "Kismet/GameplayStatics.h"
#include "IceDamageType.h"
#include "IDefender.h"
#include "IAnatomy.h"
#include "FrozenStatusEffect.h"

// Sets default values
ASnowball::ASnowball()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if(!HitBoxer)
	{
		HitBoxer = CreateDefaultSubobject<UHitBoxerComponent>(TEXT("Snowy HitBoxer"));
	}
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
	if (!SpellSphereComponent)
	{
		// Use a sphere as a simple collision representation.
		SpellSphereComponent = CreateDefaultSubobject<USpellSphereComponent>(TEXT("SpellSphereComponent"));
		// Set the sphere's collision radius.
		
		// todo: trying to lower the impact force of the snowball; maybe mass derives from the radius of the physics sphere?
		SpellSphereComponent->InitSphereRadius(15.0f);
		SpellSphereComponent->SetMassOverrideInKg(NAME_None, 0.5f, true);

		// On hit event handling
		SpellSphereComponent->SetSimulatePhysics(true);
		SpellSphereComponent->SetNotifyRigidBodyCollision(true);
		SpellSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SpellSphereComponent->SetCollisionProfileName("HitBox");
		UE_LOG(LogTemp, Warning, TEXT("snowball ctor; adding OnComponentHit delegate pointing to HitBoxer::OnHit"));
		FScriptDelegate onHitDelegate;
		onHitDelegate.BindUFunction(HitBoxer, FName("OnHit"));
		SpellSphereComponent->OnComponentHit.Add(onHitDelegate);

		// Set the root component to be the collision component.
		RootComponent = SpellSphereComponent;
	}
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(SpellSphereComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}
	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SnowballMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/FirstPerson/Meshes/FirstPersonProjectileMesh.FirstPersonProjectileMesh'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
		ProjectileMeshComponent->SetNotifyRigidBodyCollision(true);
		ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ProjectileMeshComponent->SetCollisionProfileName("Projectile");
		ProjectileMeshComponent->OnComponentHit.AddDynamic(this, &ASnowball::HandleMeshCollision);
	}
	
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	// Create a particle system, whose asset details will be filled in by subclasses
	SnowballParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SnowballParticles"));
	SnowballParticles->SetupAttachment(RootComponent);
	SnowballParticles->bAutoActivate = true;

	// default launch behavior
	LaunchFn = [&](AActor* LaunchingActor, const FVector& LaunchDirection)
	{
		ProjectileMovementComponent->Velocity = LaunchDirection * ProjectileMovementComponent->InitialSpeed;
	};
}

// Called when the game starts or when spawned
void ASnowball::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASnowball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("Tick; projectile velocity is %s"), *ProjectileMovementComponent->Velocity.ToString());
}

void ASnowball::BreakPhysics()
{
	SpellSphereComponent->SetSimulatePhysics(false);
	GetWorldTimerManager().SetTimer(BrokenPhysicsTimerHandle, this, &ASnowball::FixPhysics, BrokenPhysicsPeriod, false);
}

void ASnowball::FixPhysics()
{
	SpellSphereComponent->SetSimulatePhysics(true);
}

// Function that initializes the projectile's velocity in the shoot direction.
void ASnowball::Cast(ARyddelmystCharacter* LaunchingCharacter, const FVector& LaunchDirection)
{
	SpellSphereComponent->Caster = LaunchingCharacter;
	UE_LOG(LogTemp, Warning, TEXT("Cast; launchingchar is %s and launchdir is %s"), *LaunchingCharacter->GetName(), *LaunchDirection.ToString());
	LaunchFn(LaunchingCharacter, LaunchDirection);
}

void ASnowball::HandleMeshCollision(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
	Destroy();
}



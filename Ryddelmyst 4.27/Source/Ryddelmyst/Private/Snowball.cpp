// Fill out your copyright notice in the Description page of Project Settings.

#include "Snowball.h"
#include "RyddelmystCharacter.h"
#include "MathUtils.h"
#include "Kismet/GameplayStatics.h"
#include "IceDamageType.h"
#include "IDefender.h"
#include "FrozenStatusEffect.h"

// Sets default values
ASnowball::ASnowball()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
	if (!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision radius.
		
		// todo: trying to lower the impact force of the snowball; maybe mass derives from the radius of the physics sphere?
		CollisionComponent->InitSphereRadius(15.0f);
		CollisionComponent->SetMassOverrideInKg(NAME_None, 0.5f, true);

		// On hit event handling
		CollisionComponent->SetSimulatePhysics(true);//(false);
		CollisionComponent->SetNotifyRigidBodyCollision(true);//(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//(ECollisionEnabled::NoCollision);
		CollisionComponent->SetCollisionProfileName("BlockAllDynamic");//("NoCollision");
		FScriptDelegate onHitDelegate;
		onHitDelegate.BindUFunction(this, FName("OnHit"));
		CollisionComponent->OnComponentHit.Add(onHitDelegate);

		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
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
	}
	
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	// Create a particle system, whose asset details will be filled in by subclasses
	SnowballParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SnowballParticles"));
	SnowballParticles->SetupAttachment(RootComponent);
	SnowballParticles->bAutoActivate = true;
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
	CollisionComponent->SetSimulatePhysics(false);
	GetWorldTimerManager().SetTimer(BrokenPhysicsTimerHandle, this, &ASnowball::FixPhysics, BrokenPhysicsPeriod, false);
}

void ASnowball::FixPhysics()
{
	CollisionComponent->SetSimulatePhysics(true);
}

// Function that initializes the projectile's velocity in the shoot direction.
void ASnowball::Cast(ARyddelmystCharacter* LaunchingCharacter, const FVector& LaunchDirection)
{
	UE_LOG(LogTemp, Warning, TEXT("Cast; launchingchar is %s and launchdir is %s"), *LaunchingCharacter->GetName(), *LaunchDirection.ToString());
	try
	{
		LaunchFn(LaunchingCharacter, LaunchDirection);
	}
	catch(const std::bad_function_call& e)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast; customized launch fn is not set, so using default launch.  Details: %s"), *FString(e.what()));
		// default launch behavior
		ProjectileMovementComponent->Velocity = LaunchDirection * ProjectileMovementComponent->InitialSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Cast; launching in direction vector %s scaled by speed %f for resultant velocity %s"), 
			*LaunchDirection.ToString(), ProjectileMovementComponent->InitialSpeed, *ProjectileMovementComponent->Velocity.ToString());
	}

	ProcessCost(LaunchingCharacter);
}

void ASnowball::ProcessCost(ARyddelmystCharacter* CasterCharacter)
{
	// todo: add a Cost struct that's blueprintable with cost values corresponding to various character stats, normally MP but could be HP etc.  Ideally we'd have a mapping of some sort that would see e.g. that Snowball has an MP cost of 20 and therefore triggers an event OnMpCost(20) that RyddelmystCharacter handles by calling her UpdateMagic(-20).  That way we could have variable casting costs and abstract away the specific details of how interested parties handle the cost event. 
	// todo: the above Cost could even include custom behavior e.g. the caster flies back N meters or their HP is halved for N seconds; that would have to work outside the blueprint ecosystem tho I think since blueprints don't really support lambda functions.
}

// todo (later): make a UBulletSphereComponent class that subclasses USphereComponent and implements IAttacker (and maybe IDefender for cases of e.g. target is making a swipe attack to destroy a bullet which might otherwise linger?)
//  and have that be our Snowball collision component instead of stock USphereComponent.  That way we could move closer to potentially having a common hit handler function that works through
//  common interfaces.
// todo (later): move the status effects application code to IAttacker and IDefender, with the latter processing immunities etc.
void ASnowball::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnHit; HitComp says %s, OtherActor says %s, OtherComp says %s, normal impulse says %s, hitres says %s"), 
		*HitComp->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), *NormalImpulse.ToString(), *Hit.ToString());
	UE_LOG(LogTemp, Warning, TEXT("OnHit; EffectsOnTarget has size %d"), EffectsOnTarget.size());
	// auxiliary effect lambdas
	for (auto Effect : EffectsOnTarget)
	{
		Effect(OtherActor, Hit);
	}
	// todo: move the actual damage processing to the damaged actor/component rather than keeping it here; that way we can have different handling based on head shots, disabling leg shots etc. 
	//UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, UGameplayStatics::GetPlayerController(GetWorld(), 0), this, DamageType);
	
	// todo: do the IAttacker x IDefender dance here?  Currently ASnowball IS the IAttacker so I guess just ignore the striker arg here.  That's fine since ASnowball has some other snowball 
	//  specific things it needs to do, and this is its own specialized hit handler function.
	if(OtherComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
	{
		// Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
		UE_LOG(LogTemp, Warning, TEXT("OnHit; using attack name %s"), *GetName());
		Damage = IAttacker::Execute_CalculateDamageTx(this, GetName(), this);
		IDefender::Execute_CalculateDamageRx(OtherComp, Damage, IAttacker::Execute_GetDamageTypes(this, GetName()));
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *OtherComp->GetName()); 
	}

	Destroy();
	// todo: leave behind flattened snowball messh?
}

float ASnowball::CalculateDamageTx_Implementation(const FString& AttackName, AActor* BattleStatsBearer)
{
	float BaseDamage = Power * IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->Magic;
	UE_LOG(LogTemp, Warning, TEXT("CalculateDamage; Power (%f) * Magic (%f) = BaseDamage (%f)"), Power, IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->Magic, BaseDamage);
	BaseDamage += MathUtils::RollNdM(IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->Lvl, 6);
	return DamageScaleFactor * BaseDamage;
}


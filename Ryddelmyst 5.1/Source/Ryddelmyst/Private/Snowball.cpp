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
		HitBoxer = CreateDefaultSubobject<UHitBoxer>(TEXT("Snowy HitBoxer"));
	}
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
	if (!SpellSphereComponent)
	{
		// Use a sphere as a simple collision representation.
		SpellSphereComponent = CreateDefaultSubobject<USpellSphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision radius.
		
		// todo: trying to lower the impact force of the snowball; maybe mass derives from the radius of the physics sphere?
		SpellSphereComponent->InitSphereRadius(15.0f);
		SpellSphereComponent->SetMassOverrideInKg(NAME_None, 0.5f, true);

		// On hit event handling
		SpellSphereComponent->SetSimulatePhysics(true);
		SpellSphereComponent->SetNotifyRigidBodyCollision(true);
		SpellSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SpellSphereComponent->SetCollisionProfileName("BlockAllDynamic");
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
	Caster = LaunchingCharacter;
	UE_LOG(LogTemp, Warning, TEXT("Cast; launchingchar is %s and launchdir is %s"), *Caster->GetName(), *LaunchDirection.ToString());
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

	ProcessCost(Caster);
}

// todo: move the status effects application code to SnowballAttack
/*
void ASnowball::OnHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSnowballHit; HitComp says %s, OtherActor says %s, OtherComp says %s, normal impulse says %s, hitres says %s"), 
		*HitComp->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), *NormalImpulse.ToString(), *Hit.ToString());
	UE_LOG(LogTemp, Warning, TEXT("OnSnowballHit; EffectsOnTarget has size %d"), EffectsOnTarget.size());
	// auxiliary effect lambdas
	for (auto Effect : EffectsOnTarget)
	{
		Effect(OtherActor, Hit);
	}
	
	// todo: do the IAttacker x IDefender dance here?  Currently ASnowball IS the IAttacker so I guess just ignore the striker arg here.  
	//  That's fine since ASnowball has some other snowball specific things it needs to do, and this is its own specialized hit handler function;
	//  this logic should however be bundled in an Attack that the actual snowball sphere UPrimitiveComponent subclass owns via IAttacker.
	if(OtherComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
	{
		// Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
		UE_LOG(LogTemp, Warning, TEXT("OnSnowballHit; using attack name %s"), *GetName());
		Damage = CalculateSnowballDamageTx(Caster);
		UArmor* Armor = IDefender::Execute_GetArmor(OtherComp);
		float DamageRx = 0.f;
		if(OtherComp->GetClass()->ImplementsInterface(UAnatomy::StaticClass()))
		{
			UAnatomyUnit* AnatomyUnit = IAnatomy::Execute_GetAnatomyUnit(OtherComp);
			DamageRx = Armor->CalculateDamageRx(OtherActor, AnatomyUnit, Damage, DamageTypesToWeightsMap);
			AnatomyUnit->Debilitate(OtherActor);
		}
		else
		{
			DamageRx = Armor->CalculateDamageRx(OtherActor, nullptr, Damage, DamageTypesToWeightsMap);
		}
		TArray<TSubclassOf<UDamageType>> Types;
		DamageTypesToWeightsMap.GenerateKeyArray(Types);
		UGameplayStatics::ApplyPointDamage(OtherActor, DamageRx, NormalImpulse, Hit, UGameplayStatics::GetPlayerController(GetWorld(), 0), this, Types[0]);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("OnSnowballHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *OtherComp->GetName()); 
	}

	Destroy();
	// todo: leave behind flattened snowball messh?
}
*/

float ASnowball::CalculateSnowballDamageTx(AActor* BattleStatsBearer)
{
	float BaseDamage = Power * IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Magic"];
	UE_LOG(LogTemp, Warning, TEXT("CalculateDamage; Power (%f) * Magic (%f) = BaseDamage (%f)"), Power, IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Magic"], BaseDamage);
	BaseDamage += MathUtils::RollNdM(IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Level"], 6);
	return DamageScaleFactor * BaseDamage;
}


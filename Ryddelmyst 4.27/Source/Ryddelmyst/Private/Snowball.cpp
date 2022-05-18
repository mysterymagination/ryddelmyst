// Fill out your copyright notice in the Description page of Project Settings.

#include "Snowball.h"
#include "RyddelmystCharacter.h"
#include "MathUtils.h"
#include "Kismet/GameplayStatics.h"
#include "IceDamageType.h"
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
		CollisionComponent->SetMassOverrideInKg(NAME_None, 0.1f, true);

		// On hit event handling
		CollisionComponent->SetSimulatePhysics(true);
		CollisionComponent->SetNotifyRigidBodyCollision(true);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionComponent->SetCollisionProfileName("BlockAllDynamic");
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
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("'/Game/Ryddelmyst_Assets/Materials/SnowballMaterial.SnowballMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	// Create a snowy particle system
	SnowballParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SnowballParticles"));
	SnowballParticles->SetupAttachment(RootComponent);
	SnowballParticles->bAutoActivate = true;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Ryddelmyst_Assets/Particles/P_Snowflakes.P_Snowflakes"));
	if (ParticleAsset.Succeeded())
	{
		SnowballParticles->SetTemplate(ParticleAsset.Object);
	}

	DamageType = UIceDamageType::StaticClass();
	UE_LOG(LogTemp, Warning, TEXT("Ctor; creating default frozen status effect for snowball"));
	StatusEffect = CreateDefaultSubobject<UFrozenStatusEffect>(TEXT("SnowballFreezeEffect"));
	StatusEffect->SetId("FreezeStatusEffect");
	auto FreezeEffect = ::Cast<UFrozenStatusEffect>(StatusEffect);
	if(FreezeEffect)
	{
		FreezeEffect->SetDuration(FreezeDuration);
	}
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
}

// Function that initializes the projectile's velocity in the shoot direction.
void ASnowball::Cast(ARyddelmystCharacter* LaunchingCharacter, const FVector& LaunchDirection)
{
	Caster = LaunchingCharacter;
	try
	{
		LaunchFn(LaunchingCharacter, LaunchDirection);
	}
	catch(const std::bad_function_call& e)
	{
		UE_LOG(LogTemp, Log, TEXT("Cast; customized launch fn is not set, so using default launch.  Details: %s"), *FString(e.what()));
		// default launch behavior
		ProjectileMovementComponent->Velocity = LaunchDirection * ProjectileMovementComponent->InitialSpeed;
	}

	ProcessCost(LaunchingCharacter);
}

void ASnowball::ProcessCost(ARyddelmystCharacter* CasterCharacter)
{
	// todo: add a Cost struct that's blueprintable with cost values corresponding to various character stats, normally MP but could be HP etc.  Ideally we'd have a mapping of some sort that would see e.g. that Snowball has an MP cost of 20 and therefore triggers an event OnMpCost(20) that RyddelmystCharacter handles by calling her UpdateMagic(-20).  That way we could have variable casting costs and abstract away the specific details of how interested parties handle the cost event. 
	// todo: the above Cost could even include custom behavior e.g. the caster flies back N meters or their HP is halved for N seconds; that would have to work outside the blueprint ecosystem tho I think since blueprints don't really support lambda functions.
}

void ASnowball::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnHit; EffectsOnTarget has size %d"), EffectsOnTarget.size());
	
	// primary StatusEffect
	if(StatusEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnHit; applying primary StatusEffect with id %s"), *StatusEffect->GetId());
		StatusEffect->OnEffectApplied(OtherActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnHit; primary StatusEffect is not set"));
	}

	// auxiliary effect lambdas
	for (auto Effect : EffectsOnTarget)
	{
		Effect(OtherActor, Hit);
	}
	float dmg = 0.f;
	if(Caster)
	{
		dmg = CalculateDamage(Caster);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OnHit; caster unset, so we cannot calc damage"));
	}
	UE_LOG(LogTemp, Warning, TEXT("OnHit; damage is %f"),dmg);
	UGameplayStatics::ApplyPointDamage(OtherActor, dmg, NormalImpulse, Hit, UGameplayStatics::GetPlayerController(GetWorld(), 0), this, DamageType);
	Destroy();
	// todo: leave behind flattened snowball messh?
}

float ASnowball::CalculateDamage(ARyddelmystCharacter* Character)
{
	float Damage = Power * Character->GetStats().MagPwr;
	Damage += MathUtils::RollNdM(Character->GetStats().Lvl, 6);
	return DamageScaleFactor * Damage;
}


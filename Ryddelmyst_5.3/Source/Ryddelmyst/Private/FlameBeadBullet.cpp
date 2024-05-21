// Fill out your copyright notice in the Description page of Project Settings.


#include "FlameBeadBullet.h"
#include "FlameBeadAttack.h"

// Sets default values
AFlameBeadBullet::AFlameBeadBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UObject* Attacker = nullptr;

	if (!SpellSphereComponent)
	{
		// Use a sphere as a simple collision representation.
		SpellSphereComponent = CreateDefaultSubobject<USpellSphereComponent>(TEXT("SpellSphereComponent"));
		// Set the sphere's collision radius.
		SpellSphereComponent->InitSphereRadius(1.0f);

		// On hit event handling and physkiss
		SpellSphereComponent->SetSimulatePhysics(false);
		SpellSphereComponent->SetNotifyRigidBodyCollision(false);
		SpellSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SpellSphereComponent->SetEnableGravity(false);
	}

	FScriptDelegate onHitDelegate;
	onHitDelegate.BindUFunction(this, FName("OnHit"));
	SpellSphereComponent->OnComponentHit.Add(onHitDelegate);

	// Set the root component to be the collision component.
	BulletMovement->InitialSpeed = 300.0f;
	BulletMovement->MaxSpeed = 300.0f;
	BulletMovement->SetUpdatedComponent(SpellSphereComponent);
	RootComponent = SpellSphereComponent;
	Attacker = SpellSphereComponent;

	if (!BulletMesh)
	{
		BulletMesh = CreateDefaultSubobject<USpellStaticMeshComponent>(TEXT("FlameBeadMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
		if (Mesh.Succeeded())
		{
			BulletMesh->SetStaticMesh(Mesh.Object);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FlameBead ctor; failed to load bullet mesh asset"));
		}

		BulletMesh->SetSimulatePhysics(false);
		BulletMesh->SetMobility(EComponentMobility::Movable);
		BulletMesh->SetEnableGravity(false);
		// collision config
		BulletMesh->SetNotifyRigidBodyCollision(false);
		BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ensure the mesh follows the root component transform hierarchy; for some reason it won't render without this either
		BulletMesh->SetupAttachment(RootComponent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FlameBead ctor; bullet mesh asset unexpectedly already set"));
	}

	/* more bulletmesh approach
	FScriptDelegate onHitDelegate;
	onHitDelegate.BindUFunction(HitBoxer, FName("OnHit"));
	BulletMesh->OnComponentHit.Add(onHitDelegate);
	RootComponent = BulletMesh;
	Attacker = BulletMesh;
	*/

	// material setup
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialInstance(TEXT("'/Game/Ryddelmyst_Assets/Materials/MI_GlowyLava.MI_GlowyLava'"));
	if (MaterialInstance.Succeeded())
	{
		BulletMaterial = MaterialInstance.Object;
		BulletMesh->SetMaterial(0, BulletMaterial);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FlameBead ctor; failed to load default glowy lava material asset"));
	}

	// magic weapon setup
	Tags.Add(FName(UAttack::TAG_FLAG_IGNORE_IFRAMES));
	IAttacker::Execute_GetWeapon(Attacker)->AttackMap =
	{
		{UFlameBeadAttack::ATTACK_NAME, CreateDefaultSubobject<UFlameBeadAttack>(FName(UFlameBeadAttack::ATTACK_NAME))}
	};
	IAttacker::Execute_GetWeapon(Attacker)->CurrentAttackName = UFlameBeadAttack::ATTACK_NAME;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterBolt.h"

// Sets default values
ABlasterBolt::ABlasterBolt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UObject* Attacker = nullptr;
	if (!BulletMesh)
	{
		BulletMesh = CreateDefaultSubobject<USpellStaticMeshComponent>(TEXT("BlasterBoltMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/Ryddelmyst_Assets/Meshes/shapes/capsule.capsule"));
		if (Mesh.Succeeded())
		{
			BulletMesh->SetStaticMesh(Mesh.Object);
		}
	}
	if (!UseMeshForCollision && !BulletShape)
	{
		// Use a capsule as a simple collision representation.
		SpellCapsuleComponent = CreateDefaultSubobject<USpellCapsuleComponent>(TEXT("SpellCapsuleComponent"));
		BulletShape = SpellCapsuleComponent;
		// Set the capsule's collision radius.
		SpellCapsuleComponent->InitCapsuleSize(5.f, 25.f);
		// physics config
		BulletShape->SetMassOverrideInKg(NAME_None, Mass, true);
		BulletShape->SetSimulatePhysics(true);
		// collision config
		BulletShape->SetNotifyRigidBodyCollision(true);
		BulletShape->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BulletShape->SetCollisionProfileName("Projectile");
		FScriptDelegate onHitDelegate;
		onHitDelegate.BindUFunction(HitBoxer, FName("OnHit"));
		BulletShape->OnComponentHit.Add(onHitDelegate);
		// set the root component to be the collision component.
		RootComponent = BulletShape;
		BulletMesh->SetupAttachment(RootComponent);
		Attacker = BulletShape;
	}
	else if (UseMeshForCollision)
	{
		BulletMesh->SetMassOverrideInKg(NAME_None, Mass, true);
		BulletMesh->SetSimulatePhysics(true);
		// collision config
		BulletMesh->SetNotifyRigidBodyCollision(true);
		BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BulletMesh->SetCollisionProfileName("Projectile");
		FScriptDelegate onHitDelegate;
		onHitDelegate.BindUFunction(HitBoxer, FName("OnHit"));
		BulletMesh->OnComponentHit.Add(onHitDelegate);
		RootComponent = BulletMesh;
		Attacker = BulletMesh;
	}

	// todo: material setup

	// todo: magic weapon setup
	IAttacker::Execute_GetWeapon(Attacker)->AttackMap =
	{
		//{UFireSnowballAttack::ATTACK_NAME, CreateDefaultSubobject<UFireSnowballAttack>(FName(*UFireSnowballAttack::ATTACK_NAME))}
	};
	IAttacker::Execute_GetWeapon(Attacker)->CurrentAttackName = TEXT("");//UFireSnowballAttack::ATTACK_NAME;
}

// Called when the game starts or when spawned
void ABlasterBolt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlasterBolt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


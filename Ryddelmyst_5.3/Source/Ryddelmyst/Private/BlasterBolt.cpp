// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterBolt.h"

// Sets default values
ABlasterBolt::ABlasterBolt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!BulletShape)
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
	}
	if (!BulletMesh)
	{
		BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlasterBoltMeshComponent"));
		// todo: need a capsule shaped mesh
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/FirstPerson/Meshes/FirstPersonProjectileMesh.FirstPersonProjectileMesh'"));
		if (Mesh.Succeeded())
		{
			BulletMesh->SetStaticMesh(Mesh.Object);
		}
	}
	BulletMesh->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	BulletMesh->SetupAttachment(RootComponent);
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


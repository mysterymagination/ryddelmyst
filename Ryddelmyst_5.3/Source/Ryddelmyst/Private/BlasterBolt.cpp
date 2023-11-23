// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterBolt.h"
#include "BlasterBoltAttack.h"

// Sets default values
ABlasterBolt::ABlasterBolt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UObject* Attacker = nullptr;
	if (!BulletMesh)
	{
		BulletMesh = CreateDefaultSubobject<USpellStaticMeshComponent>(TEXT("BlasterBoltMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Ryddelmyst_Assets/Meshes/shapes/capsule.capsule'"));
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

	// material setup
	if (!BulletMaterial)
	{
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialInstance(TEXT("'/Game/Ryddelmyst_Assets/Materials/MI_GlowyPurpleLava.MI_GlowyPurpleLava'"));
		if (MaterialInstance.Succeeded())
		{
			BulletMaterial = MaterialInstance.Object;//UMaterialInstanceDynamic::Create(Material.Object, BulletMesh);
			BulletMesh->SetMaterial(0, BulletMaterial);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BlasterBolt ctor; failed to load default glowy purple lava material asset"));
		}
	}

	// magic weapon setup
	Tags.Add(FName(UAttack::TAG_FLAG_IGNORE_IFRAMES));
	IAttacker::Execute_GetWeapon(Attacker)->AttackMap =
	{
		{UBlasterBoltAttack::ATTACK_NAME, CreateDefaultSubobject<UBlasterBoltAttack>(FName(UBlasterBoltAttack::ATTACK_NAME))}
	};
	IAttacker::Execute_GetWeapon(Attacker)->CurrentAttackName = UBlasterBoltAttack::ATTACK_NAME;
}

// Called when the game starts or when spawned
void ABlasterBolt::BeginPlay()
{
	Super::BeginPlay();
	if (BulletShape)
	{
		BulletShape->SetMassOverrideInKg(NAME_None, Mass, true);
	}
	BulletMesh->SetMassOverrideInKg(NAME_None, Mass, true);

	/*
	// material setup
	if (!BulletMaterial)
	{
		UMaterial* Material = LoadObject<UMaterial>
			(
				nullptr,
				TEXT("'/Game/Ryddelmyst_Assets/Materials/M_GlowyPurpleLava.M_GlowyPurpleLava'"),
				nullptr,
				LOAD_None,
				nullptr
			);
		BulletMaterial = UMaterialInstanceDynamic::Create(Material, BulletMesh);
		BulletMesh->SetMaterial(0, BulletMaterial);
		
	}
	*/
}

// Called every frame
void ABlasterBolt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


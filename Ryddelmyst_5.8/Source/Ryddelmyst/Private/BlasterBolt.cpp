// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterBolt.h"
#include "BlasterBoltAttack.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABlasterBolt::ABlasterBolt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UObject* Attacker = nullptr;
	BulletMesh = CreateDefaultSubobject<USpellStaticMeshComponent>(TEXT("BlasterBoltMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Ryddelmyst_Assets/Meshes/shapes/capsule.capsule'"));
	if (Mesh.Succeeded())
	{
		BulletMesh->SetStaticMesh(Mesh.Object);
	}

	BulletMesh->SetSimulatePhysics(true);
	// collision config
	BulletMesh->SetNotifyRigidBodyCollision(true);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BulletMesh->SetCollisionProfileName("Projectile");
	BulletMesh->SetEnableGravity(false);
	FScriptDelegate onHitDelegate;
	onHitDelegate.BindUFunction(HitBoxer, FName("OnHit"));
	BulletMesh->OnComponentHit.Add(onHitDelegate);
	RootComponent = BulletMesh;
	Attacker = BulletMesh;

	// material setup
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialInstance(TEXT("'/Game/Ryddelmyst_Assets/Materials/MI_GlowyPurpleLava.MI_GlowyPurpleLava'"));
	if (MaterialInstance.Succeeded())
	{
		BulletMaterial = MaterialInstance.Object;
		BulletMesh->SetMaterial(0, BulletMaterial);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BlasterBolt ctor; failed to load default glowy purple lava material asset"));
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
	BulletMesh->SetMassOverrideInKg(NAME_None, Mass, true);
}

// Called every frame
void ABlasterBolt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

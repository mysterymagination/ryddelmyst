// Fill out your copyright notice in the Description page of Project Settings.


#include "HurledBoulder.h"
#include "BoulderAttack.h"

AHurledBoulder::AHurledBoulder()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UObject* Attacker = nullptr;
	BulletMesh = CreateDefaultSubobject<USpellStaticMeshComponent>(TEXT("HurledBoulderMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Ryddelmyst_Assets/Meshes/Rocks/SM_Boulder.SM_Boulder'"));
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
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialInstance(TEXT("'/Game/Ryddelmyst_Assets/Meshes/Rocks/MI_Rock1.MI_Rock1'"));
	if (MaterialInstance.Succeeded())
	{
		BulletMaterial = MaterialInstance.Object;
		BulletMesh->SetMaterial(0, BulletMaterial);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HurledBoulder ctor; failed to load material asset"));
	}

	// magic weapon setup
	Tags.Add(FName(UAttack::TAG_FLAG_IGNORE_IFRAMES));
	IAttacker::Execute_GetWeapon(Attacker)->AttackMap =
	{
		{UBoulderAttack::ATTACK_NAME, CreateDefaultSubobject<UBoulderAttack>(FName(UBoulderAttack::ATTACK_NAME))}
	};
	IAttacker::Execute_GetWeapon(Attacker)->CurrentAttackName = UBoulderAttack::ATTACK_NAME;
}

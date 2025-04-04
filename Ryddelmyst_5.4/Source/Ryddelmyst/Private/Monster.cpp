// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "IAttacker.h"
#include "Weapon.h"
#include "Attack.h"
#include "RyddelmystGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "ParticleUtils.h"

AMonster::AMonster()
{
   PrimaryActorTick.bCanEverTick = true;
   HitBoxer = CreateDefaultSubobject<UHitBoxerComponent>(TEXT("Monstrous HitBoxer"));
   static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticleAsset(TEXT("'/Game/Ryddelmyst_Assets/Particles/P_MonsterKillFX.P_MonsterKillFX'"));
   if (ParticleAsset.Succeeded())
   {
	   DeathParticleSystem = ParticleAsset.Object;
   }
}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();
	if(MonsterStatsType)
	{
		MonsterStats = NewObject<UBattleStats>(this, MonsterStatsType);
		// init other vars based on stats
		IBattleStatsBearer::Execute_HandleStatModification(this, "Speed");
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay; monster stats set to %s, and maxwalkspeed shows %f"), *MonsterStats->ToString(), GetCharacterMovement()->MaxWalkSpeed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay; Monster stats undefined"));
	}

	FScriptDelegate DamageDelegate;
	DamageDelegate.BindUFunction(this, FName("HandleDamage"));
	OnTakeAnyDamage.Add(DamageDelegate);
	Cast<URyddelmystGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->CurrentMonsterCount++;
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMonster::ToggleRun()
{
	if (IsRunning)
	{
		// toggle to walk speed
		GetCharacterMovement()->MaxWalkSpeed = MonsterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Run; after switching to walk mode, MaxWalkSpeed of %s is %f from speed stat %f times BaseWalkSpeed %f"), *GetName(), GetCharacterMovement()->MaxWalkSpeed, MonsterStats->StatsData.StatsMap["Speed"], BaseWalkSpeed);
	}
	else
	{
		// toggle to run speed
		GetCharacterMovement()->MaxWalkSpeed = MonsterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed * RunSpeedFactor;
		UE_LOG(LogTemp, Warning, TEXT("Run; after switching to run mode, MaxWalkSpeed of %s is %f from speed stat %f times BaseWalkSpeed %f times run speed factor %f"), *GetName(), GetCharacterMovement()->MaxWalkSpeed, MonsterStats->StatsData.StatsMap["Speed"], BaseWalkSpeed, RunSpeedFactor);
	}
	IsRunning = !IsRunning;
}

void AMonster::Run()
{
	GetCharacterMovement()->MaxWalkSpeed = MonsterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed * RunSpeedFactor;
	UE_LOG(LogTemp, Warning, TEXT("Run; after switching to run mode, MaxWalkSpeed of %s is %f from speed stat %f times BaseWalkSpeed %f times run speed factor %f"), *GetName(), GetCharacterMovement()->MaxWalkSpeed, MonsterStats->StatsData.StatsMap["Speed"], BaseWalkSpeed, RunSpeedFactor);
	IsRunning = true;
}

void AMonster::Walk()
{
	GetCharacterMovement()->MaxWalkSpeed = MonsterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed;
	UE_LOG(LogTemp, Warning, TEXT("Walk; after switching to walk mode, MaxWalkSpeed of %s is %f from speed stat %f times BaseWalkSpeed %f"), *GetName(), GetCharacterMovement()->MaxWalkSpeed, MonsterStats->StatsData.StatsMap["Speed"], BaseWalkSpeed);
	IsRunning = false;
}

bool AMonster::GetRunningStatus()
{
	return IsRunning;
}

void AMonster::HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// todo: load up a nice juicy explosion sound to indicate a good hit. A doink or something for hitting a part of a creature that can't be damaged might be nice,
        //  but that ain't easy to do here since most of the instances of that we have going on are accidental and probably derive from something something confunsion between
        //  root collision shape and the physics asset of a sk. mesh which is somewhat obscured while editing the Actor BP. Anyway, if we really need that we could put a tag in
        //  some armor plates or wrap certain undamageable places in unharmor which gives some nice doink feedback to the player.
	UE_LOG(LogTemp, Warning, TEXT("HandleDamage; %s says ouch for %f because of damaging actor %s"), *DamagedActor->GetName(), Damage, DamageCauser ? *DamageCauser->GetName() : TEXT("null"));
	MonsterStats->StatsData.StatsMap["HP"] -= Damage;
	MonsterStats->StatsData.StatsMap["HP"] = FMath::Clamp(MonsterStats->StatsData.StatsMap["HP"], 0.0f, MonsterStats->StatsData.StatsMap["MaxHP"]);
	if (MonsterStats->StatsData.StatsMap["HP"] == 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleDamage; %s HP with max %f has been depleted, so it will HandleDeath()"), *DamagedActor->GetName(), MonsterStats->StatsData.StatsMap["MaxHP"]);
		HandleDeath(DamageCauser);
	}
	else 
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			LoadObject<USoundBase>(nullptr, TEXT("/Game/Ryddelmyst_Assets/Audio/SFX/bfxr_sounds/Explosion.Explosion"), nullptr, LOAD_None, nullptr),
			GetActorLocation(),
			GetActorRotation(),
			Cast<URyddelmystGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->SFXVolumeScale,
			1.f,
			0.f,
			nullptr,
			nullptr,
			nullptr
		);
	}
}

void AMonster::HandleDeath_Implementation(AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleDeath; %s is destroyed by %s!"), *GetName(), DamageCauser ? *DamageCauser->GetName() : TEXT("null"));
	ParticleUtils::SpawnParticlesAtLocation(GetWorld(), GetActorLocation(), DeathParticleSystem);
	URyddelmystGameInstance* GameInstance = Cast<URyddelmystGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		LoadObject<USoundBase>(nullptr, TEXT("/Game/Ryddelmyst_Assets/Audio/SFX/bfxr_sounds/Explosion2.Explosion2"), nullptr, LOAD_None, nullptr),
		GetActorLocation(),
		GetActorRotation(),
		GameInstance->SFXVolumeScale,
		1.f,
		0.f,
		nullptr,
		nullptr,
		nullptr
	);
	Destroy();
	Cast<URyddelmystGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->CurrentMonsterCount--;
}

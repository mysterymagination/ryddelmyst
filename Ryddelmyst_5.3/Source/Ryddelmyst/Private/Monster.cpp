// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "IAttacker.h"
#include "Weapon.h"
#include "Attack.h"
#include "ParticleUtils.h"

AMonster::AMonster()
{
    /* todo: overriding this in BP may not be working?  I'm not seeing BP_ReaperTreantAI doing anything
    AIControllerClass = AMonsterAI::StaticClass();
    */
   PrimaryActorTick.bCanEverTick = true;
   HitBoxer = CreateDefaultSubobject<UHitBoxerComponent>(TEXT("Monstrous HitBoxer")); 
   static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticleAsset(TEXT("/Niagara/DefaultAssets/Templates/Systems/SimpleExplosion.SimpleExplosion"));
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
	UE_LOG(LogTemp, Warning, TEXT("HandleDamage; %s says ouch for %f"), *DamagedActor->GetName(), Damage);
	MonsterStats->StatsData.StatsMap["HP"] -= Damage;
	MonsterStats->StatsData.StatsMap["HP"] = FMath::Clamp(MonsterStats->StatsData.StatsMap["HP"], 0.0f, MonsterStats->StatsData.StatsMap["MaxHP"]);
	if (MonsterStats->StatsData.StatsMap["HP"] == 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleDamage; %s HP with max %f has been depleted, so it will HandleDeath()"), *DamagedActor->GetName(), MonsterStats->StatsData.StatsMap["MaxHP"]);
		HandleDeath();
	}
}

void AMonster::HandleDeath_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleDamage; %s is destroyed!"), *GetName());
	ParticleUtils::SpawnParticlesAtLocation(GetWorld(), GetActorLocation(), DeathParticleSystem);
	Destroy();
}

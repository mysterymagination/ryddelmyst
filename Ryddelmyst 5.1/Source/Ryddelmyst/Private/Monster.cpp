// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "IAttacker.h"
#include "Weapon.h"
#include "Attack.h"

AMonster::AMonster()
{
    /* todo: overriding this in BP may not be working?  I'm not seeing BP_ReaperTreantAI doing anything
    AIControllerClass = AMonsterAI::StaticClass();
    */
   PrimaryActorTick.bCanEverTick = true;
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

void AMonster::OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
	if (StrikingComp->GetClass()->ImplementsInterface(UAttacker::StaticClass()))
	{
		UWeapon* Weapon = IAttacker::Execute_GetWeapon(StrikingComp);
		UE_LOG(LogTemp, Warning, TEXT("monster OnHit; Striking comp %s is wielding weapon %s"), *StrikingComp->GetName(), *Weapon->GetName());
		UAttack** Attack_Check = Weapon->AttackMap.Find(Weapon->CurrentAttackName);
		if(Attack_Check)
		{
			UE_LOG(LogTemp, Warning, TEXT("monster OnHit; Striking comp %s weapon %s is running attack %s on %s's shapely %s"), 
				*StrikingComp->GetName(), *Weapon->GetName(), *(*Attack_Check)->GetName(), *StrickenActor->GetName(), *StrickenComp->GetName());
			(*Attack_Check)->OnHit_Implementation(StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("monster OnHit; Striking comp %s is not an IAttacker so can't proceed with attack processing"), *StrikingComp->GetName());
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSurface.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
ADamageSurface::ADamageSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADamageSurface::BeginPlay()
{
	Super::BeginPlay();
	if (CollisionShapeComponent)
	{
		// todo: will the static mesh fire overlap events based on the actual shape of the mesh or will it use generated simple collision shape?  If the latter, I guess we'll need to define simple collision for the lava pool manually to be only the lava-y bits.
		FScriptDelegate OverlapBeginDelegate;
		OverlapBeginDelegate.BindUFunction(this, FName("OnOverlapBegin"));
		CollisionShapeComponent->OnComponentBeginOverlap.Add(OverlapBeginDelegate);
		FScriptDelegate OverlapEndDelegate;
		OverlapBeginDelegate.BindUFunction(this, FName("OnOverlapEnd"));
		CollisionShapeComponent->OnComponentBeginOverlap.Add(OverlapEndDelegate);
	}
}

// Called every frame
void ADamageSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADamageSurface::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// todo: gonna wanna add a timer to apply damage over time after all, since there isn't obviously an OnOverlapping or similar per-frame overlap event.  TBD if we get beging and/or end overlap events per-frame?
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, GetActorLocation(), SweepResult, nullptr, this, DamageType);
}

void ADamageSurface::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ADamageSurface::ApplyDamage()
{

}


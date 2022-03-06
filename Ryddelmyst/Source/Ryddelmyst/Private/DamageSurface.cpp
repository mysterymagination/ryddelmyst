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
		FScriptDelegate OverlapBeginDelegate;
		OverlapBeginDelegate.BindUFunction(this, FName("OnOverlapBegin"));
		CollisionShapeComponent->OnComponentBeginOverlap.Add(OverlapBeginDelegate);
		FScriptDelegate OverlapEndDelegate;
		OverlapEndDelegate.BindUFunction(this, FName("OnOverlapEnd"));
		CollisionShapeComponent->OnComponentEndOverlap.Add(OverlapEndDelegate);
	}
}

// Called every frame
void ADamageSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADamageSurface::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin; overlap begin with %s"), *OtherActor->GetName());
	ContactActor = OtherActor;
	HitResult = SweepResult;
	GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &ADamageSurface::ApplyDamage, 0.5f, true, 0.0f);
}

void ADamageSurface::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin; overlap end with %s"), *OtherActor->GetName());
	ContactActor = nullptr;
	GetWorldTimerManager().ClearTimer(DamageTimerHandle);
}

void ADamageSurface::ApplyDamage()
{
	UGameplayStatics::ApplyPointDamage(ContactActor, DamageAmount, GetActorLocation(), HitResult, nullptr, this, DamageType);
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineGuideComponent.h"
#include <cmath>
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

void USplineGuideComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!Spline)
	{
		// todo: populate spline parametrically. Maybe make a few enum categories for pattern like sine wave and then scalers or something so the wave amplitude and period change.
		// todo: as a first step, let's go with sine wave on Y and Z=x^2 
		// The first thing we need is to determine a point granularity; when we draw y=x^2 on a graphing calculator, it increments X by 1 but is bounded by the display resolution being very pixelated and therefore allowing for a visible distance on X before Y gets so big it's outta sight. That's not a 'luxury' we have in UE so instead we need to limit the granularity of points e.g. 1 point per 100 cm or something so that the curve won't immediately rise up into the sky and disappear for y=x^2 and similar exponential curves. To do that, we'll want to establish the number of points in our spline and the length of the spline. The width of the spline could be subject to some function or other, but isn't really of interest to y=x^2. The height of course is a function of the length on the tin. 
		// This design for y=x^2 is just a start; the framework should be easily expandable to three dimensional functions with the main point of interest being that we've established a progression metric dimension, in this case local X, to serve as our function input.
		// The Spline will run along the relative XZ plane, such that it will be in front or back and up or down from the Actor its attached to. 
		int SplinePointSpacer = SplineLength / SplinePointCount; // the distance from one spline point to the next on local X
		float SplineWaveAngle = 360.f / (float)SplinePointCount;
		for (int PointIdx = 0; PointIdx < SplinePointCount; PointIdx++)
		{
			float X = PointIdx * SplinePointSpacer;
			float Y = SplineWaveRadius * sin(SplineWaveAngle * PointIdx);
			float Z = pow(X, 2);
			Spline->AddPoint(FSplinePoint(PointIdx, FVector(X, Y, Z)));
		}
	}
	// attach the splinecomponent to the owner actor's transform hierarchy via rootcomponent
	Spline->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	// todo: add more splines, maybe three or five total in a one of couple spread patterns out from the host Actor. Ideal thing might be to create a cone with bullet spray range for height out from the host Actor and pick 3-5 vectors randomly inside that cone. 
	if (BulletTemplate)
	{
		// setup timer to spawn bullets from World.
		GetOwner()->GetWorldTimerManager().SetTimer(BulletSpawnTimerHandle, this, &USplineGuideComponent::SpawnBullet, SpawnRate, false, 0.f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SplineGuideComponent::BeginPlay; failed to install bullet spawn timer because bullet template is null. Please set a ASpellBullet subclass as bullet template in the editor."));
	}
}

void USplineGuideComponent::SpawnBullet()
{
	// spawn bullet in the world
	// FTransform SpawnTransform;
	FTransform SpawnTransform;
	FVector ZeroPoint = Spline->GetSplinePointAt(0, ESplineCoordinateSpace::World).Position;
	FVector OnePoint = Spline->GetSplinePointAt(1, ESplineCoordinateSpace::World).Position;
	SpawnTransform.SetLocation(ZeroPoint);
	SpawnTransform.SetRotation(FQuat(UKismetMathLibrary::FindLookAtRotation(OnePoint, ZeroPoint)));
	SpawnTransform.SetScale3D(FVector(1.f));
	FActorSpawnParameters SpawnParams;
	ASpellBullet* Bullet = GetOwner()->GetWorld()->SpawnActorDeferred<ASpellBullet>
	(
		BulletTemplate,
		SpawnTransform,
		GetOwner(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	// install MagicWeapon->WielderData for Clash API projectile collision damage handling. We're assuming the owner Actor of the SplineGuideComponent is also a BattleStatsBearer here.
	IAttacker::Execute_GetWeapon(Bullet->GetAttacker())->WielderData = IBattleStatsBearer::Execute_GetStats(GetOwner())->StatsData;
	UGameplayStatics::FinishSpawningActor
	(
		Bullet,
		SpawnTransform,
		ESpawnActorScaleMethod::MultiplyWithRoot
	);
	Bullets.Add(Bullet);
	// if we've reached bullet limit, cancel this timer
	if (--BulletLimit <= 0)
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(BulletSpawnTimerHandle);
	}
}

void USplineGuideComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// tack the towingpoint onto the bullet, rotated to match rotation of forward vector and use that point to find the nearest point to it on the spline. Then we rotate our bullet to lookat the nearest point on the spline, which is our actual destination, and set the projectile movement component's velocity to a speed scaled unit vector in the direction of our destination point from our source point.
	FVector TowPoint(100.f, 0.f, 0.f);
	for (auto Bullet : Bullets)
	{
		if (IsValid(Bullet))
		{
			// check if the bullet has reached the terminal node of the spline
			FVector TerminalPointLocation = Spline->GetWorldLocationAtSplinePoint(SplinePointCount - 1);
			if ((TerminalPointLocation - Bullet->GetActorLocation()).Length() <= TowPoint.X/10.f)
			{
				// bullet has approximately reached end of spline, despawn it
				Bullet->Destroy();
			}
			else
			{
				FVector BulletRelativeTowPoint = Bullet->GetActorRotation().RotateVector(TowPoint) + Bullet->GetActorLocation();
				FVector Destination = Spline->FindLocationClosestToWorldLocation(BulletRelativeTowPoint, ESplineCoordinateSpace::World);
				FRotator DestRotation = UKismetMathLibrary::FindLookAtRotation(Bullet->GetActorLocation(), Destination);
				Bullet->SetActorRotation(DestRotation);
				FVector DirectionToTravel = DestRotation.Vector();
				Bullet->BulletMovement->Velocity = DirectionToTravel * Bullet->BulletMovement->InitialSpeed;
			}
		}
	}

	// if all bullets are destroyed, broadcast an event saying the spline guide component is no longer needed
	int DestroyedBulletCount = 0;
	for (auto Bullet : Bullets)
	{
		if (!IsValid(Bullet))
		{
			DestroyedBulletCount++;
		}
	}
	if (DestroyedBulletCount >= Bullets.Num())
	{
		// todo: broadcast spline guide completion event
	}
}
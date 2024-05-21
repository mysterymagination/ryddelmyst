// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineGuideComponent.h"
#include <cmath>
#include "Kismet/KismetMathLibrary.h"
#include "RyddelmystEventManager.h"
#include "RyddelmystGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "FlameBeadBullet.h"

USplineGuideComponent::USplineGuideComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USplineGuideComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USplineGuideComponent::BuildSpline()
{
	if (Spline)
	{
		Spline->ClearSplinePoints();
		Spline->bDrawDebug = DebugDrawSpline;
		// todo: add more splines, maybe three or five total in a one of couple spread patterns out from the host Actor. Ideal thing might be to create a cone with bullet spray range for height out from the host Actor and pick 3-5 vectors randomly inside that cone.
		// todo: populate spline parametrically. Maybe make a few enum categories for pattern like sine wave and then scalers or something so the wave amplitude and period change. Or just take lambdas in for each point and let the client define the equations.
		// todo: as a first step, let's go with sine wave on Y and Z=x^2
		// The first thing we need is to determine a point granularity; when we draw y=x^2 on a graphing calculator, it increments X by 1 but is bounded by the display resolution being very pixelated and therefore allowing for a visible distance on X before Y gets so big it's outta sight. That's not a 'luxury' we have in UE so instead we need to limit the granularity of points e.g. 1 point per 100 cm or something so that the curve won't immediately rise up into the sky and disappear for y=x^2 and similar exponential curves. To do that, we'll want to establish the number of points in our spline and the length of the spline. The width of the spline could be subject to some function or other, but isn't really of interest to y=x^2. The height of course is a function of the length on the tin.
		// This design for y=x^2 is just a start; the framework shoul  d be easily expandable to three dimensional functions with the main point of interest being that we've established a progression metric dimension, in this case local X, to serve as our function input.
		// The Spline will run along the relative XZ plane, such that it will be in front or back and up or down from the Actor its attached to.
		int SplinePointSpacer = SplineLength / SplinePointCount; // the distance from one spline point to the next on local X
		float SplineWaveAngle = 360.f / (float)SplinePointCount;
		if (BaseSplinePoints.Num() == 0)
		{
			for (int PointIdx = 0; PointIdx < SplinePointCount; PointIdx++)
			{
				float X = 25 + PointIdx * SplinePointSpacer;
				float Y = SplineWaveRadius * sin(SplineWaveAngle * PointIdx);
				float Z = pow(PointIdx * 1.5, 2);
				BaseSplinePoints.Add(FVector(X, Y, Z));
				UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::BuildSpline; adding spline point at index %d with position %f, %f, %f"), PointIdx, X, Y, Z);
			}
		}

		// add our points to the spline
		Spline->SetSplineLocalPoints(BaseSplinePoints);

		UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::BuildSpline; spline itself is at %s"), *Spline->GetComponentLocation().ToString());
		for (int PointIdx = 0; PointIdx < SplinePointCount; PointIdx++)
		{
			UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::BuildSpline; spline point at index %d has world position %s and local position %s, and get world location at spline point reports %s. Owning actor %s's origin is at %s."),
				PointIdx,
				*Spline->GetSplinePointAt(PointIdx, ESplineCoordinateSpace::World).Position.ToString(),
				*Spline->GetSplinePointAt(PointIdx, ESplineCoordinateSpace::Local).Position.ToString(),
				*Spline->GetWorldLocationAtSplinePoint(PointIdx).ToString(),
				*GetOwner()->GetName(),
				*GetOwner()->GetActorLocation().ToString()
			);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SplineGuideComponent::BuildSpline; failed to populate spline because linked splinecomponent is null. Please have the owning Actor set a SplineComponent as spline."));
	}
}

void USplineGuideComponent::StartSplineBullets()
{
	if (Spline)
	{
		if (BulletTemplate)
		{
			// setup timer to spawn bullets from World.
			UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::StartSplineBullets; starting bullet spawn timer with rate %f"), SpawnRate);
			GetOwner()->GetWorldTimerManager().SetTimer(BulletSpawnTimerHandle, this, &USplineGuideComponent::SpawnBullet, SpawnRate, true, 0.f);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SplineGuideComponent::StartSplineBullets; failed to install bullet spawn timer because bullet template is null. Please set a ASpellBullet subclass as bullet template in the editor."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SplineGuideComponent::StartSplineBullets; failed to start bullet spawning because spline is null. Please have the owning Actor set a SplineComponent as spline."));
	}
}

void USplineGuideComponent::SpawnBullet()
{
	// spawn bullet in the world
	// FTransform SpawnTransform;
	FTransform SpawnTransform;
	FVector ZeroPoint = Spline->GetWorldLocationAtSplinePoint(0); //Spline->GetSplinePointAt(0, ESplineCoordinateSpace::World).Position;
	FVector OnePoint = Spline->GetWorldLocationAtSplinePoint(1); //Spline->GetSplinePointAt(1, ESplineCoordinateSpace::World).Position;
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
	UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::SpawnBullet; spawning bullet %s at location %s; %d bullets remaining."), *Bullet->GetName(), *SpawnTransform.GetLocation().ToString(), BulletsRemaining);
	// install MagicWeapon->WielderData for Clash API projectile collision damage handling. We're assuming the owner Actor of the SplineGuideComponent is also a BattleStatsBearer here.
	IAttacker::Execute_GetWeapon(Bullet->GetAttacker())->WielderData = IBattleStatsBearer::Execute_GetStats(GetOwner())->StatsData;
	UGameplayStatics::FinishSpawningActor
	(
		Bullet,
		SpawnTransform,
		ESpawnActorScaleMethod::MultiplyWithRoot
	);
	// attach Bullet to owning Actor's transform hierarchy
	Bullet->AttachToComponent(GetOwner()->GetDefaultAttachComponent(), FAttachmentTransformRules::KeepWorldTransform);
	Bullets.Add(Bullet);
	// if we've reached bullet limit, cancel this timer
	if (--BulletsRemaining <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::SpawnBullet; stopping bullet spawn timer because we've hit the limit of %i bullets."), BulletLimit);
		GetOwner()->GetWorldTimerManager().ClearTimer(BulletSpawnTimerHandle);
	}
}

void USplineGuideComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	/*
	for (int PointIdx = 0; PointIdx < Spline->GetNumberOfSplinePoints(); ++PointIdx)
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::TickComponent; spline point at index %d has world position %s and local position %s, and world location at spline point API says %s. Owning actor %s's origin is at %s."),
			PointIdx,
			*Spline->GetSplinePointAt(PointIdx, ESplineCoordinateSpace::World).Position.ToString(),
			*Spline->GetSplinePointAt(PointIdx, ESplineCoordinateSpace::Local).Position.ToString(),
			*Spline->GetWorldLocationAtSplinePoint(PointIdx).ToString(),
			*GetOwner()->GetName(),
			*GetOwner()->GetActorLocation().ToString()
		);
	}
	*/

	// tack the towingpoint onto the bullet, rotated to match rotation of forward vector and use that point to find the nearest point to it on the spline. Then we rotate our bullet to lookat the nearest point on the spline, which is our actual destination, and set the projectile movement component's velocity to a speed scaled unit vector in the direction of our destination point from our source point.
	// todo: something something deltatime to smooth movement? Would need to adjust the terminal point location threshold below in that case, probably.
	for (auto Bullet : Bullets)
	{
		if (IsValid(Bullet))
		{
			// check if the bullet has reached the terminal node of the spline
			FVector TerminalPointLocation = Spline->GetWorldLocationAtSplinePoint(SplinePointCount - 1);
			UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::TickComponent; spline terminal point location is %s and bullet distance from it is %f"),
				*TerminalPointLocation.ToString(),
				(TerminalPointLocation - Bullet->GetActorLocation()).Length()
			);
			// todo: is overshoot a possibility e.g. the bullet moves too far in a frame to detect being close enough to the terminal point to trigger despawn?
			//  If so, how do we detect that? Simply doing terminal point - bullet location and checking for negative is not enough because the bullet's trajectory is arbitrary.
			//  I guess you could try to take trajectory and relative positions into account, or maybe compare samples across a couple frames to see if the distance from
			//  terminal point is ever growing (which would indicate an overshoot).
			if ((TerminalPointLocation - Bullet->GetActorLocation()).Length() <= TowingPoint.X/10.f)
			{
				// bullet has approximately reached end of spline, despawn it
				UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::TickComponent; bullet %s has reached the end of the spline. Despawning."), *Bullet->GetName());
				Bullet->Destroy();
			}
			else
			{
				FVector BulletRelativeTowPoint = GetOwner()->GetActorRotation().RotateVector(TowingPoint) + Bullet->GetActorLocation();
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
	if (DestroyedBulletCount >= BulletLimit)
	{
		// shut down spline guide component
		UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::TickComponent; all bullets have been destroyed. Destroying splineguide."));
		DestroyComponent(false);
	}

	/* simple spline path debugging block
	FVector TowPoint(10.f, 0.f, 0.f);
	for (auto Bullet : Bullets)
	{
		FVector BulletRelativeTowPoint = GetOwner()->GetActorRotation().RotateVector(TowPoint) + Bullet->GetActorLocation();
		FVector Destination = Spline->FindLocationClosestToWorldLocation(BulletRelativeTowPoint, ESplineCoordinateSpace::World);
		FRotator DestRotation = UKismetMathLibrary::FindLookAtRotation(Bullet->GetActorLocation(), Destination);
		Bullet->SetActorRotation(DestRotation);
		FVector DirectionToTravel = DestRotation.Vector();
		Bullet->BulletMovement->Velocity = DirectionToTravel * Bullet->BulletMovement->InitialSpeed;

		UE_LOG(LogTemp, Warning, TEXT("SplineGuideComponent::TickComponent; setting bullet %s (whose location is %s) destination to %s. Bullet velocity is %s and speed is %f."),
			*Bullet->GetName(),
			*Bullet->GetActorLocation().ToString(),
			*Destination.ToString(),
			*Bullet->BulletMovement->Velocity.ToString(),
			Bullet->BulletMovement->InitialSpeed
		);
	}
	*/
}

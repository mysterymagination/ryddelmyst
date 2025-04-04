// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/TimerHandle.h"
#include "SpellBullet.h"
#include "SplineGuideComponent.generated.h"

/**
 * Moves an array of Actor bullets' root SceneComponents along a spline path; the path may be prefabricated and provided or generated at runtime with parameters.
 * We'll have a single spline with this component directing bullets' movement along it; we'll set up a transform attachment between the spline and the spawned bullet actors and then apply AddMovementInput() with a given speed and direction derived by using a point placed in front of the bullet and sampling for the nearest spline point to it with FindLocationClosestToWorldLocation() so the bullet is continously making its way along the spline.
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API USplineGuideComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USplineGuideComponent();
	/**
	 * Pointer to the USplineComponent that this component guides actors along.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	USplineComponent* Spline;
	/**
	 * UClass pointer to an ASpellBullet subclass, defining the type of bullet we'll spawn in.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	TSubclassOf<ASpellBullet> BulletTemplate;
	/**
	 *  A point offset from each bullet's forward vector which we use to sample the closest point on the spline, and then have bullet lookat and move towards that point on the spline.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	FVector TowingPoint = FVector(10.f,0.f,0.f);
	/**
	 *	The total FSplinePoints making up our USplineComponent; together with SplineLength, this determines the granularity of the spline and how rapidly the interval function for each dimension should be applied e.g. how much space exists between X values when we run y=x^2 to determine a new Y.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int SplinePointCount = 10;
	/**
	 * The total length in centimeters along our progressions axis (hardcoded to relative X for now) of the entire spline; together with SplineLength, this determines the granularity of the spline and how rapidly the interval function for each dimension should be applied e.g. how much space exists between X values when we run y=x^2 to determine a new Y.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int SplineLength = 1000;
	/**
	 * The radius in centimeters of the circle our trig function (hardcoded to sin() wave for now) rotates points around i.e. how far away from the progression axis our points move on the wave axis (hardcoded to Y for now).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int SplineWaveRadius = 50;
	/**
	 * Whether or not to draw the spline in editor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool DebugDrawSpline = false;
	/**
	 * Whether or not to destroy the given Spline param when the spline guide is being destroyed
	 * i.e. the Spline was created just for the purpose of this spline guide's use.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	bool DestroySpline = false;
	/**
	 * The rate in seconds at which bullets spawn along the spline.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	float SpawnRate = 2.5f;
	/**
	 * The number of bullets we spawn along the spline.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	int BulletLimit = 10;
	/**
	 * Array of local space (relative to the attach actor) points defining the spline shape.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
	TArray<FVector> BaseSplinePoints;

private:
	UPROPERTY()
	FTimerHandle BulletSpawnTimerHandle;
	/**
	 * Array tracking our spawned ASpellBullets.
	 */
	UPROPERTY()
	TArray<ASpellBullet*> Bullets;
	/**
	 * The remaining number of bullets we should spawn along the spline.
	 */
	UPROPERTY()
	int BulletsRemaining = BulletLimit;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetBulletType(TSubclassOf<ASpellBullet> BulletType) { BulletTemplate = BulletType; };
	/**
	 * Populate the linked SplineComponent Spline with generated points.
	 */
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void BuildSpline();
	/**
	 * Kick off the bullet spawn timer.
	 */
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void StartSplineBullets();

private:
	/**
	 * Spawns a bullet anchored at the 0th spline point.
	 */
	void SpawnBullet();
};

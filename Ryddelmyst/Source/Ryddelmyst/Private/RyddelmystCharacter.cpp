// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Interact.h"
#include "Describable.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "RyddelmystHUD.h"
#include "GameFramework/PlayerController.h"
#include "ItemActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ARyddelmystCharacter

ARyddelmystCharacter::ARyddelmystCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	for first person perspective
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 50.f + BaseEyeHeight)); // Position the camera slightly above eye level and at about the front of the mesh's face
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetActive(true);

	// Create a CameraComponent	for third person perspective
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	ThirdPersonCameraComponent->SetRelativeLocation(FVector(-239.56f, 1.75f, 100.f + BaseEyeHeight)); // Position the camera slightly above eye level and at about the front of the mesh's face
	ThirdPersonCameraComponent->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	ThirdPersonCameraComponent->bUsePawnControlRotation = false;
	ThirdPersonCameraComponent->SetActive(false);
}

void ARyddelmystCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are riddled with RyddelmystCharacter!"));

	// linear magic recharge; recharges in 20 point blocks every 5 seconds
	FTimerDelegate TimerDelegate;
	float MagicRechargeAmount = 20.f;
	TimerDelegate.BindUFunction(this, FName("UpdateMagic"), MagicRechargeAmount);
	GetWorldTimerManager().SetTimer(MagicTimerHandle, TimerDelegate, 5.0f, true, 0.f);
	
	FScriptDelegate DamageDelegate;
	DamageDelegate.BindUFunction(this, FName("HandleDamage"));
	OnTakeAnyDamage.Add(DamageDelegate);

	HUD = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD<ARyddelmystHUD>();

	FScriptDelegate OverlapBeginDelegate;
	OverlapBeginDelegate.BindUFunction(this, FName("OnOverlapBegin"));
	GetCapsuleComponent()->OnComponentBeginOverlap.Add(OverlapBeginDelegate);
	FScriptDelegate OverlapEndDelegate;
	OverlapEndDelegate.BindUFunction(this, FName("OnOverlapEnd"));
	GetCapsuleComponent()->OnComponentEndOverlap.Add(OverlapEndDelegate);
}

void ARyddelmystCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARyddelmystCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind camera toggle events
	PlayerInputComponent->BindAction("CameraToggle", IE_Released, this, &ARyddelmystCharacter::CameraToggle);

	// Bind LookitYou control mode
	PlayerInputComponent->BindAction("Free Cam Mode", IE_Released, this, &ARyddelmystCharacter::SendControl);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ARyddelmystCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARyddelmystCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARyddelmystCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ARyddelmystCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARyddelmystCharacter::LookUpAtRate);
	PlayerInputComponent->BindAction("Cycle Weapon Up", IE_Released, this, &ARyddelmystCharacter::CycleWeaponUp);
	PlayerInputComponent->BindAction("Cycle Weapon Down", IE_Released, this, &ARyddelmystCharacter::CycleWeaponDown);

	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ARyddelmystCharacter::Fire);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ARyddelmystCharacter::Run);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ARyddelmystCharacter::Interact);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ARyddelmystCharacter::HandleCrouch);

	PlayerInputComponent->BindAxis("Select Item", this, &ARyddelmystCharacter::CycleItem);
	PlayerInputComponent->BindAction("Use Item", IE_Released, this, &ARyddelmystCharacter::UseItem);
}

void ARyddelmystCharacter::Interact()
{
	// if we're already engaged with an interactable, end that engagement e.g. by dropping a GRABBABLE Actor.
	if (GrabbedActor)
	{
		/* bounding box is better since we don't necessarily want to rely on having a skeletalmesh whose origin happens to be her feet
		// look up SkeletalMeshComponent of the Character and use her Z so that objects will be placed at her feet.
		float DroppedZValue = GetActorLocation().Z;
		UActorComponent* CharacterComponent = GetComponentByClass(USkeletalMeshComponent::StaticClass());
		USkeletalMeshComponent* CharacterSkeleton = Cast<USkeletalMeshComponent>(CharacterComponent);
		if (CharacterSkeleton)
		{
			DroppedZValue = CharacterSkeleton->GetComponentLocation().Z;
		}
		*/

		// since GetActorBounds fills in the BoundBoxExtents output var with values relative to the center of the Actor, e.g. half the full size in a given dimension, we can just use the Z component directly for our half height modifier to DroppedZValue below
		FVector GrabbedBoundingBoxExtents;
		FVector GrabbedOrigin;
		GrabbedActor->GetActorBounds(false, GrabbedOrigin, GrabbedBoundingBoxExtents, false);
		// todo: the placement on Z gets messed up by crouching iff I use the bounding box extent Z rather than the skeletal mesh origin Z (which happens to be our feet).  I'm guessing this is because one or the other but not both of GetActorLocation().Z and the GetActorBounds bounding box extent Z is being adjusted for crouch, causing our final calculated Z to be slighlty below the ground. EDIT: looks like your bb extent z doesn't fully change for some reason; it went down very slightly but was about the same.  I'm guessing it gets modified by the crouch, but something in my collision bounds is not modified by the crouch, so we get a weird slight modification.
		FVector CharacterBoundingBoxExtents;
		FVector CharacterOrigin;
		GetActorBounds(true, CharacterOrigin, CharacterBoundingBoxExtents, false);
		UE_LOG(LogTemp, Warning, TEXT("character origin is %s and grabbed obj origin is %s"), *CharacterOrigin.ToString(), *GrabbedOrigin.ToString());
		UE_LOG(LogTemp, Warning, TEXT("character Z is %f and character bb extent z is %f, and grabbed obj Z is %f and grabbed obj bb extent Z is %f"), GetActorLocation().Z, CharacterBoundingBoxExtents.Z, GrabbedActor->GetActorLocation().Z, GrabbedBoundingBoxExtents.Z);
		
		/*
		// world space strat, for practice and reference
		FVector DroppedLocationOffset = GetActorRotation().RotateVector(FVector(MaxInteractDistance * 2.f, 0.f, GrabbedBoundingBoxExtents.Z));
		float CharacterFeetZ = GetActorLocation().Z - CharacterBoundingBoxExtents.Z;
		// we need to add back our crouched half height as the crouch op does not seem to affect the bounding box extents
		if (GetCharacterMovement()->IsCrouching())
		{
			CharacterFeetZ += GetCharacterMovement()->CrouchedHalfHeight;
		}
		GrabbedActor->SetActorLocation(FVector(DroppedLocationOffset.X + GetActorLocation().X, DroppedLocationOffset.Y + GetActorLocation().Y, DroppedLocationOffset.Z + CharacterFeetZ));
		*/

		// local space strat, cleaner
		FVector DroppedLocationOffset(CarryDistance, 0.f, GrabbedBoundingBoxExtents.Z);
		float CharacterFeetZ = -CharacterBoundingBoxExtents.Z;
		UE_LOG(LogTemp, Warning, TEXT("Interact; dropped location Z raw is %f and feetz is %f so final Z location relative to actor is %f"), DroppedLocationOffset.Z, CharacterFeetZ, DroppedLocationOffset.Z + CharacterFeetZ);
		// we need to add back our crouched half height as the crouch op does not seem to affect the bounding box extents
		if (GetCharacterMovement()->IsCrouching())
		{
			CharacterFeetZ += GetCharacterMovement()->CrouchedHalfHeight;
		}
		GrabbedActor->SetActorRelativeLocation(FVector(DroppedLocationOffset.X, DroppedLocationOffset.Y, DroppedLocationOffset.Z + CharacterFeetZ));

		GrabbedActor->SetActorEnableCollision(true);
		GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedActor = nullptr;
	}
	else
	{
		// todo: debug draw this trace so we can figure out why the ray is apparently not running directly through the reticle at the center of the viewport.
		// determine where our ray trace should begin and end
		const FVector start_trace = FirstPersonCameraComponent->GetComponentLocation();
		const FVector direction = FirstPersonCameraComponent->GetComponentRotation().Vector();
		const FVector end_trace = start_trace + (direction * MaxInteractDistance);
		UE_LOG(LogTemp, Warning, TEXT("Interact; ray start says %s, direction says %s, and ray end says %s"), *start_trace.ToString(), *direction.ToString(), *end_trace.ToString());
		DrawDebugLine(
			GetWorld(), 
			start_trace, 
			end_trace,
			FColor(255,0,255,255),
			false,
			50000.f,
			1,
			12.f
		);
		const FName TraceTag("InteractRay");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		FCollisionQueryParams TraceParams(FName(TEXT("InteractTrace")), true, this);
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.bTraceComplex = true;
		TraceParams.TraceTag = TraceTag;

		// cast our ray out and check for a hit object implementing IInteract
		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, start_trace, end_trace, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

		// process any hit actor looking for interactability
		AActor* Actor = Hit.GetActor();
		if (Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Interact; found something in range called %s"), *Actor->GetName());
			if (Actor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Interact; actor is interactable!"));
				TArray<InteractCapability> capArray = IInteract::Execute_OnInteract(Actor);
				UE_LOG(LogTemp, Warning, TEXT("Interact; cap array of interactable has %d members"), capArray.Num());
				int idx = 0;
				for (auto cap : capArray)
				{
					UE_LOG(LogTemp, Warning, TEXT("Interact; cap array index is %d, and ordinal says %u"), idx, (uint8)cap);
					idx++;
					UEnum* MyEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("InteractCapability"));
					UE_LOG(LogTemp, Warning, TEXT("Interact; myenum ptr is %p"), MyEnum);
					if (MyEnum)
					{
						FString DisplayString = MyEnum->GetNameStringByValue((uint8)cap);
						UE_LOG(LogTemp, Warning, TEXT("Interact; cap array of interactable says %s"), *DisplayString);
					}

					if (cap == InteractCapability::GRABBABLE)
					{
						GrabbedActor = Actor;
						// todo: physics on during grab causes the object to not follow us for some reason despite attachment, even with gravity off
						// todo: check if physics is enabled and iff so, disable.  Also make a note somewhere that we need to renable physics for this grabbed actor at whatever components we find it enabled.
						GrabbedActor->DisableComponentsSimulatePhysics();
						// todo: if we teleport the object into its carry location relative to the player and that location is inside another collision object, the player and grabbed object get rocketed away.  Funny, but not useful.  
						GrabbedActor->SetActorEnableCollision(false);
						GrabbedActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
						GrabbedActor->SetActorRelativeLocation(FVector(CarryDistance, 0.f, 0.f));
						/* Forward Vector version; it's just a unit vector on X accounting for all your rotations e.g. vector [1,0,0] rotated by all your character's rotations.  
						GrabbedActor->SetActorLocation(GetActorLocation() + (GetActorForwardVector() * CarryDistance));
						*/
						UE_LOG(LogTemp, Warning, TEXT("Interact; player forward vector is %s.  placing grabbed actor at %s relative to player.  Its world coords are %s and world coords of player are %s"), *GetActorForwardVector().ToString(), *GrabbedActor->GetRootComponent()->GetRelativeLocation().ToString(), *GrabbedActor->GetActorLocation().ToString(), *GetActorLocation().ToString());
					}
					else if (cap == InteractCapability::DESCRIBABLE)
					{
						if (Actor->GetClass()->ImplementsInterface(UDescribable::StaticClass()))
						{
							FString DescString = IDescribable::Execute_GenerateDescription(Actor);
							UE_LOG(LogTemp, Warning, TEXT("Interact; description of %s is %s"), *Actor->GetName(), *DescString);
						}
					}

					// todo: extend player collision bounds to encompass the grabbable object; I guess toss a cubeoid around it?  Alternative would be to lean on the existing collision of the object and somehow get a message sent to the player iff the player is holding it that it has collided with something.
				}
			}
		}
	}
}

void ARyddelmystCharacter::SendControl()
{
	if (!LookitYouGo)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		LookitYouGo = GetWorld()->SpawnActor<ALookitYouPawn>(ThirdPersonCameraComponent->GetComponentLocation(), ThirdPersonCameraComponent->GetComponentRotation(), SpawnParams);
	}
	if (LookitYouGo)
	{
		LookitYouGo->SetFollowCharacter(this);
		LookitYouGo->TakeControl();
	}
}

void ARyddelmystCharacter::CameraToggle()
{
	UE_LOG(LogTemp, Warning, TEXT("CameraToggle; 1PP cam rotation is %s"),  *FirstPersonCameraComponent->GetComponentRotation().ToString());
	if (FirstPersonCameraMode)
	{
		FirstPersonCameraMode = false;
		FirstPersonCameraComponent->SetActive(false);
		ThirdPersonCameraComponent->SetActive(true);
	}
	else
	{
		ThirdPersonCameraComponent->SetActive(false);
		FirstPersonCameraComponent->SetActive(true);
		FirstPersonCameraMode = true;
	}
}

void ARyddelmystCharacter::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("Run; speed is %f and we are %s"), GetCharacterMovement()->Velocity.Size(), IsRunning ? TEXT("running") : TEXT("walking"));
	if (IsRunning)
	{
		// toggle to walk speed
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else
	{
		// toggle to run speed
		GetCharacterMovement()->MaxWalkSpeed = 1800.f;
	}
	IsRunning = !IsRunning;
}

void ARyddelmystCharacter::HandleCrouch()
{
	// todo: reposition any grabbed object based on crouch/uncrouch eye height etc.
	if (bIsCrouched)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleCrouch; uncrouching"));
		Super::UnCrouch(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleCrouch; crouching"));
		Super::Crouch(false);
	}
}


void ARyddelmystCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value * (IsRunning ? 30 : 1));
	}
}

void ARyddelmystCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value * (IsRunning ? 30 : 1));
	}
}

void ARyddelmystCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARyddelmystCharacter::LookUp(float Value)
{
	if (Value != 0.0f)
	{
		// Only send controller pitch to CharacterMovementComponent if we're in 1PP.  In 3PP, manually pitch the 1PP cam based on controller input since 1PP cam controls spawn location of projectiles; it is essentially our 'muzzle'.  CharacterMovementComponent seems to only consider forwarding pitch to 1PP cam if it is active i.e. we are in 1PP mode, which is why we're manually applying pitch to 1PP cam while we're in 3PP.
		if (FirstPersonCameraMode)
		{
			AddControllerPitchInput(Value);
		}
		else
		{
			FRotator currentRotation = FirstPersonCameraComponent->GetComponentRotation();
			// We don't want any Roll happening, and for some reason when I use AddWorldRotation I get Roll even if I specify 0.f for Yaw and Roll params.  I noticed that in FPP the Roll seems to stay almost 0 forever, so I guess a direct set to 0 here is fine enough.
			// Current Pitch minus Value allows us to have mouse movement towards you => target pitch down and mouse movement away from you => pitch up like we have in FPP.
			FirstPersonCameraComponent->SetWorldRotation(FRotator(currentRotation.Pitch-Value, currentRotation.Yaw, 0.f));
		}
	}
}

void ARyddelmystCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARyddelmystCharacter::CycleWeaponUp()
{
	UE_LOG(LogTemp, Warning, TEXT("CycleWeaponUp"));
	if (Spells.Num() > 0)
	{	
		if (SelectedWeaponIdx < Spells.Num() - 1)
		{
			SelectedWeaponIdx++;
		}
		else
		{
			SelectedWeaponIdx = 0;
		}
	}
}

void ARyddelmystCharacter::CycleWeaponDown()
{
	UE_LOG(LogTemp, Warning, TEXT("CycleWeaponDown"));
	if (Spells.Num() > 0)
	{
		if (SelectedWeaponIdx > 0)
		{
			SelectedWeaponIdx--;
		}
		else
		{
			SelectedWeaponIdx = Spells.Num() - 1;
		}
	}
}

void ARyddelmystCharacter::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire; attempting to fire snowball... using weapon idx %u"), SelectedWeaponIdx);
	// Attempt to fire a projectile.
	if (SelectedWeaponIdx < Spells.Num())
	{
		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = FirstPersonCameraComponent->GetComponentLocation() + FTransform(FirstPersonCameraComponent->GetComponentRotation()).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = FirstPersonCameraComponent->GetComponentRotation();
		// todo: so what are we doing here exactly?  We've got the world space rotation of the firstpersoncameracomponent and then we're goosing its pitch, but what are we rotating 10 degrees around?  The Y axis, sure, but is that axis considered to run through world origin (implying we take our offset from origin into account somewhere) or does it run through the MuzzleLocation point?  Since the definition of a point in world space is (I think) a vector from origin to an endpoint, maybe there's no distinction?  But what about the arc length -- doesn't vector magnitude i.e. how far the point is away from world origin modify the length of the arc curve that an N degree rotation causes?  EDIT: yeah it does; arc length is radius*theta where theta is the rotation angle in radians and the radius here is our vector magnitude. I guess the point of interest is how we use this MuzzleRotation, and the answer to that is wrapped up in World->SpawnActor() below...
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			TSubclassOf<ASnowball> SnowballType = Spells[SelectedWeaponIdx];
			if (Magic >= SnowballType.GetDefaultObject()->GetMagicCost())
			{
				UE_LOG(LogTemp, Warning, TEXT("Fire; magic is %f and cost is %f so firing"), Magic, SnowballType.GetDefaultObject()->GetMagicCost());
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();
				// Spawn the projectile at the muzzle.
				ASnowball* Snowball = World->SpawnActor<ASnowball>(SnowballType, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Snowball)
				{
					// Set the projectile's initial trajectory.
					FVector LaunchDirection = MuzzleRotation.Vector();
					Snowball->FireInDirection(LaunchDirection);
					UpdateMagic(-Snowball->GetMagicCost());
				}
			}
		}
	}
}

void ARyddelmystCharacter::OnLostFollower(ILookitYou* lookitYou)
{
	// move the 3PP camera to the last location and rotation of the lookityou
	ThirdPersonCameraComponent->SetWorldLocationAndRotation(lookitYou->GetLocation(), lookitYou->GetRotation());
	// nullify the lookityou member
	LookitYouGo = nullptr;
}

float ARyddelmystCharacter::GetHealth() 
{
	return Health;
}

float ARyddelmystCharacter::GetMaxHealth()
{
	return FullHealth;
}

float ARyddelmystCharacter::GetMagic()
{
	return Magic;
}
float ARyddelmystCharacter::GetMaxMagic()
{
	return FullMagic;
}

FText ARyddelmystCharacter::GetHealthText()
{
	int32 HP = FMath::RoundHalfFromZero(Health);
	FString HPS = FString::FromInt(HP);
	FString FullHPS = FString::FromInt(FullHealth);
	FString HealthHUD = HPS + FString(TEXT("/")) + FullHPS;
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

FText ARyddelmystCharacter::GetMagicText()
{
	int32 MP = FMath::RoundHalfFromZero(Magic);
	FString MPS = FString::FromInt(MP);
	FString FullMPS = FString::FromInt(FullMagic);
	FString MagicHUD = MPS + FString(TEXT("/")) + FullMPS;
	FText MPText = FText::FromString(MagicHUD);
	return MPText;
}

void ARyddelmystCharacter::UpdateHealth(float HealthChange)
{
	Health += HealthChange;
	Health = FMath::Clamp(Health, 0.0f, FullHealth);
}

void ARyddelmystCharacter::UpdateMagic(float MagicChange)
{
	Magic += MagicChange;
	UE_LOG(LogTemp, Warning, TEXT("UpdateMagic; magicchange is %f so magic is %f prior to clamp"), MagicChange, Magic);
	Magic = FMath::Clamp(Magic, 0.0f, FullMagic);
	UE_LOG(LogTemp, Warning, TEXT("UpdateMagic; magic is %f post clamp"), Magic);
}

void ARyddelmystCharacter::HandleDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, class UDamageType* DamageType, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleDamage; ouch for %f"), Damage);
	UpdateHealth(-Damage);
	SetCanBeDamaged(false);
	DamageInvincibilityTimer();
	// todo: send the character flying in some direction derived from HitInfo?
}

void ARyddelmystCharacter::SetDamageState()
{
	SetCanBeDamaged(true);
}

void ARyddelmystCharacter::DamageInvincibilityTimer()
{
	GetWorldTimerManager().SetTimer(InvincibilityTimerHandle, this, &ARyddelmystCharacter::SetDamageState, 1.f, false);
}

void ARyddelmystCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin; character overlapping %s"), *OtherActor->GetName());
	// if overlap item is an Item then add to onscreen inv as well as data inventory
	auto ItemActor = Cast<AItemActor>(OtherActor);
	if (ItemActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin; overlapped actor is an Item!"));
		if (Inventory.Num() < MaxInventory)
		{
			HUD->AddItemIcon(IItem::Execute_GetDisplayIcon(ItemActor));
			IItem::Execute_OnPickup(ItemActor, this);
			Inventory.Add(ItemActor->GetItem());
		}
		else
		{
			// todo: raise dialog informing the player that their inventory is full
		}
	}
}

void ARyddelmystCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd; character no longer overlapping %s"), *OtherActor->GetName());
}

void ARyddelmystCharacter::UseItem()
{
	if (SelectedItemIdx >= 0 && SelectedItemIdx < Inventory.Num())
	{
		IItem::Execute_OnUse(Inventory[SelectedItemIdx], this);
	}
}

void ARyddelmystCharacter::CycleItem(float Value)
{
	if (Value != 0.f)
	{
		if (Value > 0.f)
		{
			// cycle up, wrapping to 0 if at top bound
			if (SelectedItemIdx == Inventory.Num() - 1)
			{
				SelectedItemIdx = 0;
			}
			else
			{
				SelectedItemIdx++;
			}
		}
		else
		{
			// cycle down, wrapping to top bound if at 0
			if (SelectedItemIdx == 0)
			{
				SelectedItemIdx = Inventory.Num() - 1;
			}
			else
			{
				SelectedItemIdx--;
			}
		}
		HUD->SelectItem(SelectedItemIdx);
	}
}
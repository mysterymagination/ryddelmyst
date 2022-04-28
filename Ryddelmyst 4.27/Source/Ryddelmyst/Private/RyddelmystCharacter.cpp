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
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "OpenClose.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ARyddelmystCharacter

const FString ARyddelmystCharacter::EquipSlotsData[] = { TEXT("Head"), TEXT("Neck"), TEXT("Hands"), TEXT("Feet") };

const std::string ARyddelmystCharacter::ID_SPELL_ELECTRICSNOWBALL = "ElectricSnowball";
const std::string ARyddelmystCharacter::ID_SPELL_FIRESNOWBALL = "FireSnowball";
const std::string ARyddelmystCharacter::ID_SPELL_SNOWBALL = "Snowball";
const std::string ARyddelmystCharacter::ID_SPELL_ASPECT_CONJURATION = "Conjuration";
const std::string ARyddelmystCharacter::ID_SPELL_ASPECT_EVOCATION = "Evocation";
const std::string ARyddelmystCharacter::ID_SPELL_ASPECT_ENCHANTMENT = "Enchantment";
const std::string ARyddelmystCharacter::ID_SPELL_ASPECT_TRANSMUTATION = "Transmutation";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_CREATION = "Creation";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_ATTR = "Attributes";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_DAMAGE = "Damage";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_DURATION = "Duration";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_EFFECT = "Effect";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_SPAWN = "Spawn";

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

	// setup equipment mapping with our default static equip slots
	for (FString key : EquipSlotsData)
	{
		Equipment.Add(key);
	}
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
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay; ryddel hud is %p"), HUD);

	FScriptDelegate OverlapBeginDelegate;
	OverlapBeginDelegate.BindUFunction(this, FName("OnOverlapBegin"));
	GetCapsuleComponent()->OnComponentBeginOverlap.Add(OverlapBeginDelegate);
	FScriptDelegate OverlapEndDelegate;
	OverlapEndDelegate.BindUFunction(this, FName("OnOverlapEnd"));
	GetCapsuleComponent()->OnComponentEndOverlap.Add(OverlapEndDelegate);

	// process any starting equipment effects
	for (auto& Elem : Equipment)
	{
		if (Elem.Value)
		{
			IItem::Execute_OnEquip(Elem.Value, this);
		}
	}
}

void ARyddelmystCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// todo: generate overlap events with grabbed actor so that any overlap triggered events will fire, such as damage surface... this may not be practical since UE4's collision system really seems to want to handle itself
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARyddelmystCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("FixMe", IE_Pressed, this, &ARyddelmystCharacter::FixMe);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ARyddelmystCharacter::PauseGame);

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

void ARyddelmystCharacter::FixMe()
{
	UE_LOG(LogTemp, Warning, TEXT("FixMe; player pos before is %s"), *GetActorLocation().ToString());
	// oops, position in world i.e. relative to world origin is NOT considered part of the transform hierarchy, so for a root component not attached to any transform parent SetActorRelativeLocation and SetActorLocation are equivalent.
	//SetActorRelativeLocation(FVector(100.f, 100.f, 100.f));
	//SetActorLocation(FVector(100.f, 100.f, 100.f));
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * 100.f);
	UE_LOG(LogTemp, Warning, TEXT("FixMe; player pos after is %s"), *GetActorLocation().ToString());
}

void ARyddelmystCharacter::PauseGame()
{
	UE_LOG(LogTemp, Warning, TEXT("PauseGame"));
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetPause(true);
	HUD->ShowPauseMenu();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController);
}

void ARyddelmystCharacter::Interact()
{
	// hide dialogue if showing
	HUD->HideDialogue();

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
		FHitResult Hit = FireInteractRay();

		// process any hit actor looking for interactability
		AActor* Actor = Hit.GetActor();
		if (Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Interact; found something in range called %s"), *Actor->GetName());
			if (Actor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Interact; actor is interactable!"));
				TArray<InteractCapability> capArray = IInteract::Execute_OnInteract(Actor, this);
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
							// todo: the GenerateDescription API should probably return an already localized FText since UE4 doesn't seem to allow for localization of anything but an actual string literal 
							HUD->ShowDialogue(FText::FromString(DescString));
							UE_LOG(LogTemp, Warning, TEXT("Interact; description of %s is %s"), *Actor->GetName(), *DescString);
						}
					}
					else if (cap == InteractCapability::POCKETABLE)
					{
						AItemActor* ItemActor = Cast<AItemActor>(Actor);
						if (ItemActor)
						{
							UE_LOG(LogTemp, Warning, TEXT("Interact; pocketing item from %s"), *ItemActor->GetName());
							AddInventoryItemFromActor(ItemActor);
						}
					}
					else if (cap == InteractCapability::OPENABLE)
					{
						UE_LOG(LogTemp, Warning, TEXT("Interact; openable object %s found"), *Actor->GetName());
						IOpenClose::Execute_Open(Actor, this);
					}

					// todo: extend player collision bounds to encompass the grabbable object; I guess toss a cubeoid around it?  Alternative would be to lean on the existing collision of the object and somehow get a message sent to the player iff the player is holding it that it has collided with something.
				}
			}
		}
	}
}

FHitResult ARyddelmystCharacter::FireInteractRay()
{
	// determine where our ray trace should begin and end
	const FVector start_trace = FirstPersonCameraComponent->GetComponentLocation();
	const FVector direction = FirstPersonCameraComponent->GetComponentRotation().Vector();
	const FVector end_trace = start_trace + (direction * MaxInteractDistance);
	UE_LOG(LogTemp, Warning, TEXT("Interact; ray start says %s, direction says %s, and ray end says %s"), *start_trace.ToString(), *direction.ToString(), *end_trace.ToString());
	DrawDebugLine(
		GetWorld(),
		start_trace,
		end_trace,
		FColor(255, 0, 255, 255),
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
	return Hit;
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
				// todo: try to run the metamagiclaunch fn and do the default launch in the bad_function_call exception.  However, we also need a way to limit apply the metamagiclaunch to only relevant spells e.g. electric snowball.  The easiest way to do that would be three different metamagiclaunch functions, like we're doing for general modifiers, but then we've lost all control of scaling.  Ideal thing would be, instead of a complicated arch change, simply shoving additional modifications into the existing per-element functions; that could be achieved easily enough with a function vector or something, but then we have a problem in that the firing behavior currently lives here in the character rather than the spell.  If we move that into ASnowball as ASnowball::Launch(), we could also add a metamagiclaunch function there that the ASnowball::Launch() will call similarly to the above proposed failover to default model.

				UE_LOG(LogTemp, Warning, TEXT("Fire; magic is %f and cost is %f so firing"), Magic, SnowballType.GetDefaultObject()->GetMagicCost());
				
				// todo: we want Evocation, Transmutation, and Conjuration phases for metamagic application HOWEVER the Evocation (construction/init) and Conjuration (actual spawning) phases are somewhat coupled since we need to create and provision during Evocation the same number of snowballs that we eventually spawn during Conjuration. 

				// Spawn the projectile at the muzzle.
				FTransform SpawnTransform;
				SpawnTransform.SetLocation(MuzzleLocation);
				SpawnTransform.SetRotation(FQuat(MuzzleRotation));
				SpawnTransform.SetScale3D(FVector(1.f));
				ASnowball* Snowball = World->SpawnActorDeferred<ASnowball>(SnowballType, SpawnTransform, this, GetInstigator());
				
				if (Snowball)
				{
					Snowball->SetCaster(this);
					try
					{
						/*
						if (SnowballType == AFireSnowball::StaticClass())
						{
							MetamagicFireFn(Cast<AFireSnowball>(Snowball));
						}
						else if (SnowballType == AElectricSnowball::StaticClass())
						{
							MetamagicElectricFn(Cast<AElectricSnowball>(Snowball));
						}
						else
						{
							MetamagicIceFn(Snowball);
						}
						*/
					}
					catch (const std::bad_function_call& e)
					{
						UE_LOG(LogTemp, Log, TEXT("Fire; ignoring attempt to invoke unset metamagic fn.  Details: %s"), *FString(e.what()));
					}
					Snowball->FinishSpawning(SpawnTransform);

					// Set the projectile's initial trajectory.
					FVector LaunchDirection = MuzzleRotation.Vector();
					Snowball->Cast(this, LaunchDirection);
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
		AddInventoryItemFromActor(ItemActor);
	}
}

bool ARyddelmystCharacter::AddInventoryItemFromActor(AItemActor* ItemActor)
{
	TSubclassOf<UObject> ItemClass = ItemActor->GetItemType();
	if (ItemClass->ImplementsInterface(UItem::StaticClass()))
	{
		UObject* ItemObj = NewObject<UObject>(this, ItemClass);
		if (AddInventoryItem(ItemObj))
		{
			ItemActor->Destroy();
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AddInventoryItemFromActor; failed to add item %s"), *ItemObj->GetName());
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AddInventoryItemFromActor; itemactor's item class %s does not implement the item interface"), *ItemClass->GetName());
		return false;
	}
}

bool ARyddelmystCharacter::AddInventoryItem(UObject* ItemObj)
{
	if (Inventory.Num() < MaxInventory)
	{
		if (ItemObj->GetClass()->ImplementsInterface(UItem::StaticClass()))
		{
			HUD->AddItemIcon(IItem::Execute_GetDisplayIcon(ItemObj));
			IItem::Execute_OnPickup(ItemObj, this);
			Inventory.Add(ItemObj);
			if (Inventory.Num() == 1)
			{
				// adding first inv item, so auto select it
				SelectedItemIdx = 0;
				HUD->SelectItem(SelectedItemIdx);
			}
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem; item obj %s does not implement the item interface"), *ItemObj->GetName());
			return false;
		}
	}
	else
	{
		HUD->ShowDialogue(NSLOCTEXT("NSFeedback", "KeyInvFull", "Your inventory is full!"));
		UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem; inventory is full"));
		return false;
	}
}

void ARyddelmystCharacter::AddEquippedItem(UObject* ItemObj)
{
	if (ItemObj->GetClass()->ImplementsInterface(UItem::StaticClass()))
	{
		if (IItem::Execute_IsEquippable(ItemObj))
		{
			HUD->AddEquipIcon(IItem::Execute_GetDisplayIcon(ItemObj));
			IItem::Execute_OnEquip(ItemObj, this);
			Equipment.Add(IItem::Execute_GetEquipSlot(ItemObj), ItemObj);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AddEquippedItem; item obj %s cannot be equipped"), *ItemObj->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AddEquippedItem; item obj %s does not implement the item interface"), *ItemObj->GetName());
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
		UE_LOG(LogTemp, Warning, TEXT("UseItem; used item is %s and lives at %p"), *Inventory[SelectedItemIdx]->GetName(), Inventory[SelectedItemIdx]);
		bool UseSuccess = IItem::Execute_OnUse(Inventory[SelectedItemIdx], this);
		
		// todo: support non-consumable/multi-use items?
		
		// only process item consumption if the use succeeded, e.g. a key was used on the correct lock
		if (UseSuccess)
		{
			// remove the used item from inv
			Inventory.RemoveAt(SelectedItemIdx);
			// remove the used item's icon
			HUD->RemoveItemIcon(SelectedItemIdx);
			// only need to modify the SelectedItemIdx if we've used the only item in the inv or if we used the last item (i.e. item at highest index) in the inv array
			if (Inventory.Num() == 0)
			{
				// empty inv after removing used item, clear selection 
				HUD->ClearItemSelection();
			}
			else if (SelectedItemIdx >= Inventory.Num())
			{
				// drop the selected item idx to max since we're now above the max index
				SelectedItemIdx = Inventory.Num() - 1;
				HUD->SelectItem(SelectedItemIdx);
			}
		}
	}
}

void ARyddelmystCharacter::CycleItem(float Value)
{
	if (Value != 0.f)
	{
		if (!IsInventorySleeping && Inventory.Num() > 0)
		{
			if (Value > 0.f)
			{
				UE_LOG(LogTemp, Warning, TEXT("CycleItem; up"));
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
				UE_LOG(LogTemp, Warning, TEXT("CycleItem; down"));
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
			GetWorldTimerManager().SetTimer(InventoryTimerHandle, this, &ARyddelmystCharacter::WakeInventory, 0.25f, false);
			IsInventorySleeping = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CycleItem; inventory is sleeping or empty, can't cycle now"));
		}
	}
	else
	{
		// user is not holding down cycle keys, so wake inv immediately.   This way there is no possibility of perceived delay after the user manually presses the cycle key multiple times in quick succession.
		GetWorldTimerManager().ClearTimer(InventoryTimerHandle);
		IsInventorySleeping = false;
	}
}

void ARyddelmystCharacter::WakeInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("WakeInventory"));
	IsInventorySleeping = false;
}
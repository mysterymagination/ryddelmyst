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
#include "DrawDebugHelpers.h"
#include "RyddelmystHUD.h"
#include "GameFramework/PlayerController.h"
#include "ItemActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "OpenClose.h"
#include "SnowballAttack.h"
#include "RyddelmystGameInstance.h"
#include "Components/LightComponent.h"
#include <stdexcept>

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ARyddelmystCharacter

const FString ARyddelmystCharacter::EquipSlotsData[] = { TEXT("Head"), TEXT("Neck"), TEXT("Hands"), TEXT("Feet") };

const std::string ARyddelmystCharacter::ID_SPELL_ELECTRICSNOWBALL = "ElectricSnowball";
const std::string ARyddelmystCharacter::ID_SPELL_FIRESNOWBALL = "FireSnowball";
const std::string ARyddelmystCharacter::ID_SPELL_SNOWBALL = "Snowball";
const std::string ARyddelmystCharacter::ID_SPELL_PHASE_CONJURATION = "Conjuration";
const std::string ARyddelmystCharacter::ID_SPELL_PHASE_EVOCATION = "Evocation";
const std::string ARyddelmystCharacter::ID_SPELL_PHASE_ENCHANTMENT = "Enchantment";
const std::string ARyddelmystCharacter::ID_SPELL_PHASE_TRANSMUTATION = "Transmutation";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_CREATION = "Creation";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_ATTR = "Attributes";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_DAMAGE = "Damage";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_DURATION = "Duration";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_EFFECT = "Effect";
const std::string ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_SPAWN = "Spawn";

ARyddelmystCharacter::ARyddelmystCharacter()
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	for first person perspective
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 50.f + BaseEyeHeight)); // Position the camera slightly above eye level and at about the front of the mesh's face
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetActive(true);

	// Create the arm on which the 3PP camera will sit
	ThirdPersonCameraArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraArm"));
	ThirdPersonCameraArmComponent->SetupAttachment(GetCapsuleComponent());
	ThirdPersonCameraArmComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));
	ThirdPersonCameraArmComponent->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	ThirdPersonCameraArmComponent->bUsePawnControlRotation = true;
	ThirdPersonCameraArmComponent->TargetArmLength = CamArmLengthMax;

	// Create a CameraComponent	for third person perspective
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(ThirdPersonCameraArmComponent, USpringArmComponent::SocketName);
	ThirdPersonCameraComponent->SetActive(false);

	HitBoxer = CreateDefaultSubobject<UHitBoxerComponent>(TEXT("Ryddelmysty HitBoxer"));

	// setup equipment mapping with our default static equip slots
	for (FString key : EquipSlotsData)
	{
		Equipment.Add(key);
	}
    
	PortraitMap = 
	{
		{"happy", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Happy.maya_portraits_Sprite_Happy")).Object},
		{"sad", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Sad.maya_portraits_Sprite_Sad")).Object},
		{"weary", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Weary.maya_portraits_Sprite_Weary")).Object},
		{"eldritch", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Eldritch.maya_portraits_Sprite_Eldritch")).Object},
		{"flirty", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Flirty.maya_portraits_Sprite_Flirty")).Object},
		{"angry", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Angry.maya_portraits_Sprite_Angry")).Object},
		{"embarrassed", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Embarrassed.maya_portraits_Sprite_Embarrassed")).Object},
		{"confused", ConstructorHelpers::FObjectFinder<UPaperSprite>
 (TEXT("/Game/Ryddelmyst_Assets/Textures/maya_portraits_Sprite_Confused.maya_portraits_Sprite_Confused")).Object}
	};

	UE_LOG(LogTemp, Warning, TEXT("ryddelcharacter ctor; portraitmap at happy says %p"), PortraitMap["happy"]);
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

	if(CharacterStatsType)
	{
		CharacterStats = NewObject<UBattleStats>(this, CharacterStatsType);
		// init other vars based on stats
		IBattleStatsBearer::Execute_HandleStatModification(this, "Speed");
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay; character stats set to %s, and maxwalkspeed shows %f"), *CharacterStats->ToString(), GetCharacterMovement()->MaxWalkSpeed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay; character stats undefined"));
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

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ARyddelmystCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARyddelmystCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARyddelmystCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARyddelmystCharacter::LookUpAtRate);
	PlayerInputComponent->BindAction("Scroll Up", IE_Released, this, &ARyddelmystCharacter::ScrollUp);
	PlayerInputComponent->BindAction("Scroll Down", IE_Released, this, &ARyddelmystCharacter::ScrollDown);

	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ARyddelmystCharacter::Fire);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ARyddelmystCharacter::Run);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ARyddelmystCharacter::Interact);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ARyddelmystCharacter::HandleCrouch);

	PlayerInputComponent->BindAxis("Select Item", this, &ARyddelmystCharacter::CycleItem);
	PlayerInputComponent->BindAction("Use Item", IE_Released, this, &ARyddelmystCharacter::UseItem);
	PlayerInputComponent->BindAction("ToggleTorch", IE_Released, this, &ARyddelmystCharacter::ToggleTorch);
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

void ARyddelmystCharacter::ToggleTorch()
{
	auto LightComponent = FindComponentByTag<ULightComponent>(FName(TEXT("Player Light Source")));
	if (LightComponent)
	{
		LightComponent->ToggleVisibility();
	}
}

UBodyCapsuleComponent* ARyddelmystCharacter::GetBody_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("GetBody; body source not overriden in BP as expected!"));
	return nullptr;
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
	// hide dialogue if showing, and return early so we don't potentially trigger a new dialogue
	if(HUD->HideDialogue())
	{
		return;
	}

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
			CharacterFeetZ += GetCharacterMovement()->GetCrouchedHalfHeight();
		}
		GrabbedActor->SetActorRelativeLocation(FVector(DroppedLocationOffset.X, DroppedLocationOffset.Y, DroppedLocationOffset.Z + CharacterFeetZ));

		GrabbedActor->SetActorEnableCollision(true);
		GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		// alert the treant that his offspring is now safe again
		if (GrabbedActor->ActorHasTag(FName(TEXT("WoodEgg"))))
		{
			Cast<URyddelmystGameInstance>(GetWorld()->GetGameInstance())->GetEventManager()->WoodEggDangerEvent.Broadcast(false);
		}
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
						// alert the treant that his offspring is being disturbed!
						if (GrabbedActor->ActorHasTag(FName(TEXT("WoodEgg"))))
						{
							Cast<URyddelmystGameInstance>(GetWorld()->GetGameInstance())->GetEventManager()->WoodEggDangerEvent.Broadcast(true);
						}
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
							FDescriptor Desc = IDescribable::Execute_GenerateDescription(Actor);
							UPaperSprite* ReactionPortrait = nullptr;
							switch(Desc.Reaction)
							{
								// Maya defaults to a happy outlook!  That's the way to be.
								case InteractReactions::HAPPY:
								case InteractReactions::NEUTRAL:
									ReactionPortrait = PortraitMap["happy"];
									break;
								case InteractReactions::SAD:
									ReactionPortrait = PortraitMap["sad"];
									break;
								case InteractReactions::FLIRTY:
									ReactionPortrait = PortraitMap["flirty"];
									break;
								case InteractReactions::CONFUSED:
									ReactionPortrait = PortraitMap["confused"];
									break;
								case InteractReactions::ANGRY:
									ReactionPortrait = PortraitMap["angry"];
									break;
								case InteractReactions::EMBARRASSED:
									ReactionPortrait = PortraitMap["embarrassed"];
									break;
								case InteractReactions::WEARY:
									ReactionPortrait = PortraitMap["weary"];
									break;
								case InteractReactions::ELDRITCH:
									ReactionPortrait = PortraitMap["eldritch"];
									break;
								default:
									ReactionPortrait = PortraitMap["happy"];
									break;

							}
							HUD->ShowDialogue(ReactionPortrait, Desc.LocalizedDescription);
							const UEnum* ReactionEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("InteractReactions"));
							UE_LOG(LogTemp, Warning, TEXT("Interact; describing %s as %s and reaction %s.  Portrait map at confused says %p"), *Actor->GetName(), *Desc.LocalizedDescription.ToString(), *ReactionEnum->GetNameStringByIndex(static_cast<int32>(Desc.Reaction)), PortraitMap["confused"]);
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
						if(!IOpenClose::Execute_IsOpen(Actor))
						{
							UE_LOG(LogTemp, Warning, TEXT("Interact; opening opencloseable object %s"), *Actor->GetName());
							IOpenClose::Execute_Open(Actor, this);
						}
						else 
						{
							UE_LOG(LogTemp, Warning, TEXT("Interact; closing opencloseable object %s"), *Actor->GetName());
							IOpenClose::Execute_Close(Actor, this);
						}
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
	const FVector InitLocation = FirstPersonCameraComponent->GetComponentLocation();
	const FVector direction = FirstPersonCameraComponent->GetComponentRotation().Vector();
	const FVector start_trace = InitLocation + direction * GetCapsuleComponent()->GetScaledCapsuleRadius();
	UE_LOG(LogTemp, Warning, TEXT("Interact; FPP camera forward vec is %s and rotation.vector is %s and capsule radius is %f"), *FirstPersonCameraComponent->GetForwardVector().ToString(), *direction.ToString(), GetCapsuleComponent()->GetScaledCapsuleRadius());
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
	FCollisionQueryParams TraceParams(FName(TEXT("InteractTrace")), true, nullptr);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;
	TraceParams.TraceTag = TraceTag;

	// cast our ray out and check for a hit object implementing IInteract
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, start_trace, end_trace, ECollisionChannel::ECC_WorldDynamic, TraceParams);
	return Hit;
}

void ARyddelmystCharacter::CameraToggle()
{
	if (FirstPersonCameraMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraToggle; 3PP cam is at %s, maya is at %s, and camera arm is at %s relative to maya"), 
			*ThirdPersonCameraComponent->GetComponentLocation().ToString(),
			*GetActorLocation().ToString(),
			*ThirdPersonCameraArmComponent->GetRelativeLocation().ToString());
		FirstPersonCameraMode = false;
		FirstPersonCameraComponent->SetActive(false);
		ThirdPersonCameraComponent->SetActive(true);
		// in 3PP we want the cam to be able to orbit the character, which means we don't want rotation communicated to the pawn;
		// that's why we have special handling for moveforward/right that turns the character wherever the cam is pointing
		// and effectively allows free looking at self when still and sync'd rotation when in motion. 
		bUseControllerRotationYaw = false;
	}
	else
	{
		ThirdPersonCameraComponent->SetActive(false);
		FirstPersonCameraComponent->SetActive(true);
		FirstPersonCameraMode = true;
		// in 1PP we want the rotation to flow to both pawn and cam together because we are seeing through the character's eyes.
		// Otherwise, she can turn around and see out the back of her own head!
		bUseControllerRotationYaw = true;
	}
}

void ARyddelmystCharacter::Run()
{
	if (IsRunning)
	{
		// toggle to walk speed
		GetCharacterMovement()->MaxWalkSpeed = CharacterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Run; after switching to walk mode, MaxWalkSpeed is %f from speed stat %f times BaseWalkSpeed %f"), GetCharacterMovement()->MaxWalkSpeed, CharacterStats->StatsData.StatsMap["Speed"], BaseWalkSpeed);
	}
	else
	{
		// toggle to run speed
		GetCharacterMovement()->MaxWalkSpeed = CharacterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed * RunSpeedFactor;
		UE_LOG(LogTemp, Warning, TEXT("Run; after switching to run mode, MaxWalkSpeed is %f from speed stat %f times BaseWalkSpeed %f times run speed factor %f"), GetCharacterMovement()->MaxWalkSpeed, CharacterStats->StatsData.StatsMap["Speed"], BaseWalkSpeed, RunSpeedFactor);
	}
	IsRunning = !IsRunning;
}

void ARyddelmystCharacter::HandleCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleCrouch; iscrouched says %d"), Super::bIsCrouched);
	if (Super::bIsCrouched)
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
	if (Controller != nullptr && Value != 0.0f)
	{
		// find out current controller yaw
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector and apply controller's yaw, then move the character along the resultant direction vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * GetCharacterMovement()->MaxWalkSpeed);

		// turn the character in the direction they're now moving
		SetActorRotation(YawRotation);
	}
}

void ARyddelmystCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		// find out current controller yaw
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector and apply controller's yaw, then move the character along the resultant direction vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value * GetCharacterMovement()->MaxWalkSpeed);

		// turn the character in the direction they're now moving
		SetActorRotation(YawRotation);
	}
}

void ARyddelmystCharacter::Zoom3PPCam(float Factor)
{
	float Length = ThirdPersonCameraArmComponent->TargetArmLength;
	if (Length >= CamArmLengthMin && Factor < 0.f ||
		Length <= CamArmLengthMax && Factor > 0.f)
	{
		ThirdPersonCameraArmComponent->TargetArmLength += ZoomRate*Factor;
		UE_LOG(LogTemp, Warning, TEXT("Zoom3PPCam; new arm length is %f"), ThirdPersonCameraArmComponent->TargetArmLength);
	}
}

void ARyddelmystCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARyddelmystCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARyddelmystCharacter::ScrollUp()
{
	if (HUD->IsDialogueActive())
	{
		HUD->ScrollDialogueUp();
	}
	else if (!FirstPersonCameraMode)
	{
		Zoom3PPCam(-1.f);
	}
	else 
	{
		CycleWeaponUp();
	}
}

void ARyddelmystCharacter::ScrollDown()
{
	if (HUD->IsDialogueActive())
	{
		HUD->ScrollDialogueDown();
	}
	else if (!FirstPersonCameraMode)
	{
		Zoom3PPCam(1.f);
	}
	else
	{
		CycleWeaponDown();
	}
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

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = FirstPersonCameraComponent->GetComponentLocation() + FTransform(FirstPersonCameraComponent->GetComponentRotation()).TransformVector(MuzzleOffset);
		FRotator AdjustedMuzzleRotation = MuzzleRotation + FirstPersonCameraComponent->GetComponentRotation();
		UE_LOG(LogTemp, Warning, TEXT("Fire; adjusted MuzzleRotation becomes %s from original muzzle rotation %s plus first person cam rotation of %s"),
			*AdjustedMuzzleRotation.ToString(), *MuzzleRotation.ToString(), *FirstPersonCameraComponent->GetComponentRotation().ToString());
		/*
		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = FirstPersonCameraComponent->GetComponentRotation();
		// todo: so what are we doing here exactly?  We've got the world space rotation of the firstpersoncameracomponent and then we're goosing its pitch, but what are we rotating 10 degrees around?  The Y axis, sure, but is that axis considered to run through world origin (implying we take our offset from origin into account somewhere) or does it run through the MuzzleLocation point?  Since the definition of a point in world space is (I think) a vector from origin to an endpoint, maybe there's no distinction?  But what about the arc length -- doesn't vector magnitude i.e. how far the point is away from world origin modify the length of the arc curve that an N degree rotation causes?  EDIT: yeah it does; arc length is radius*theta where theta is the rotation angle in radians and the radius here is our vector magnitude. I guess the point of interest is how we use this MuzzleRotation, and the answer to that is wrapped up in World->SpawnActor() below... EDIT2: I think this is why we usually do rotation ops first in transform matrix math; you rotate with just the points making up the mesh effectively at origin and then translate them to whatever world offset location they should live at.
		MuzzleRotation.Pitch += 10.0f;
		*/

		UWorld* World = GetWorld();
		if (World)
		{
			TSubclassOf<ASnowball> SnowballType = Spells[SelectedWeaponIdx];
			// todo: support having metamagic change costs; the electricsnowball shouldn't cost more than the others until it has its awesome metamagic spreadshot installed
			UAttack* CDOSnowballAttack = IAttacker::Execute_GetWeapon(SnowballType.GetDefaultObject()->GetSpellSphereComponent())->GetCurrentAttack();
			if (CDOSnowballAttack->CheckCosts(this))
			{
				UE_LOG(LogTemp, Warning, TEXT("Fire; magic is %f and cost is %f so firing"), CharacterStats->StatsData.StatsMap["MP"], CDOSnowballAttack->Costs["MP"]);
				
				// todo: order of operations may be a problem with this metamagic model -- say I have a one evocation fx that multiplies damage by 2 and another that adds 2; you'll have different result values depending on which fx is applied first, since PEMDAS isn't factored into this model at all.  Commutativity isn't so much of an issue since that's re: operands rather than operations, and the operands are encapsulated within the function fx.  One possible fix would be to use an ordered map of some sort (or sort the pairs you would iterate over prior to iteration or something) based on a PEMDAS derived priority?  

				// Conjuration phase: lookup conjuration effects for the current spell and apply them, storing the returned bullet array.  If there are none, run the default creation behavior.
				std::vector<ASnowball*> Bullets;
				const auto& SpellMap = MetamagicMap[std::string(TCHAR_TO_UTF8(*SnowballType->GetName()))];
				for(const auto& Source : SpellMap)
				{
					// check the current Source for Conjuration effects and run them
					const auto SourceConjurationItr = Source.second.find(ARyddelmystCharacter::ID_SPELL_PHASE_CONJURATION);
					if(SourceConjurationItr != Source.second.end())
					{
						// todo: obviously this architecture only really supports one Conjuration[Creation] function, so how should the case of multiple creation functions from multiple sources interoperate?  Maybe the best method would be to check for incompatibilities at the OnEquip stage and disable the incompatible part of the newly added source or refuse to equip it entirely? 
						const auto& ConjurationMap = SourceConjurationItr->second;
						const auto ConjurationCreationItr = ConjurationMap.find(ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_CREATION);
						if (ConjurationCreationItr != ConjurationMap.end())
						{
							// since Creation is currently the only Conjuration phase effect (and any additional effects would likely need a different function sig), there's no need to iterate
								const auto& CreationFnVariant = ConjurationCreationItr->second;
							try
							{
								Bullets = std::get<std::function<std::vector<ASnowball*>/*created instances*/(ARyddelmystCharacter* /*creating character*/)>>(CreationFnVariant)(this); 
							}
							catch (std::bad_variant_access const& ex)
							{
								UE_LOG(LogTemp, Warning, TEXT("Fire; no Conjuration[Creation] function with expected signature from %s.  Details are: %s"), *FString(Source.first.c_str()), *FString(ex.what()));
							}
						}
					}
				}
				// If we didn't have any Conjuration[Creation] functions to run, we'll need to use the default
				if(Bullets.size() == 0)
				{
					FTransform SpawnTransform;
					SpawnTransform.SetIdentity();
					Bullets.emplace_back(World->SpawnActorDeferred<ASnowball>(SnowballType, SpawnTransform, this, GetInstigator()));
				}

				// install player battle stats as soon as the bullets are conjured
				for (auto Bullet : Bullets)
				{
					IAttacker::Execute_GetWeapon(Bullet->GetSpellSphereComponent())->WielderData = CharacterStats->StatsData;
				}

				// Evocation phase: lookup evocation effects for the current spell and apply them to each instance in the bullet array created above.
				for(const auto& Source : SpellMap)
				{
					// check the current Source for Evocation effects and run them
						auto SourceEvocationItr = Source.second.find(ARyddelmystCharacter::ID_SPELL_PHASE_EVOCATION);
						if(SourceEvocationItr != Source.second.end())
						{
							const auto& EvocationMap = SourceEvocationItr->second;
						for(const auto& Evocation : EvocationMap)
						{
							try 
							{
								const auto& EvocationFn = std::get<std::function<void(ASnowball*)>>(Evocation.second);
								for(auto Bullet : Bullets)
								{
									EvocationFn(Bullet);
								}
							}
							catch (std::bad_variant_access const& ex)
							{
								UE_LOG(LogTemp, Warning, TEXT("Fire; no evocation function with expected signature from %s.  Details are: %s"), *FString(Source.first.c_str()), *FString(ex.what()));
							}
						}
					}
				}

				// Enchantment phase: lookup enchantment effects for the current spell and store them for later OnHit application in each bullet instance in the bullet array created above.
				for(const auto& Source : SpellMap)
				{
					// check the current Source for Enchantment effects and install them for invocation in OnHit later
					auto SourceEnchantmentItr = Source.second.find(ARyddelmystCharacter::ID_SPELL_PHASE_ENCHANTMENT);
					if(SourceEnchantmentItr != Source.second.end())
					{
						const auto& EnchantmentMap = SourceEnchantmentItr->second;
						for(const auto& Enchantment : EnchantmentMap)
						{
							try 
							{
								const auto& EnchantmentFn = std::get<std::function<void(AActor* /*EnchantedActor*/, const FHitResult& /*HitResult data*/)>>(Enchantment.second);
								for(auto Bullet : Bullets)
								{
									UE_LOG(LogTemp, Warning, TEXT("Fire; adding enchantment from %s"), *FString(Source.first.c_str()));
									USnowballAttack* SnowballAttack = Cast<USnowballAttack>(IAttacker::Execute_GetWeapon(Bullet->GetSpellSphereComponent())->GetCurrentAttack());
									SnowballAttack->GetEffectsVector().emplace_back(EnchantmentFn);
								}
							}
							catch (std::bad_variant_access const& ex)
							{
								UE_LOG(LogTemp, Warning, TEXT("Fire; no enchantment function with expected signature from %s.  Details are: %s"), *FString(Source.first.c_str()), *FString(ex.what()));
							}
						}
					}
				}

				// Transmutation[Spawn] phase: lookup spawn effects for the current spell and run them on each bullet instance in the bullet array created above.
				FTransform SpawnTransform;
				SpawnTransform.SetLocation(MuzzleLocation);
				SpawnTransform.SetRotation(FQuat(AdjustedMuzzleRotation));
				SpawnTransform.SetScale3D(FVector(1.f));
				FVector LaunchDirection = AdjustedMuzzleRotation.Vector();
				bool Spawned = false;
				for(const auto& Source : SpellMap)
				{
					// check the current Source for Evocation effects and install them for invocation in OnHit later
					auto SourceTransmutationItr = Source.second.find(ARyddelmystCharacter::ID_SPELL_PHASE_TRANSMUTATION);
					if(SourceTransmutationItr != Source.second.end())
					{
						const auto& TransmutationMap = SourceTransmutationItr->second;
						auto TransmutationSpawnItr = TransmutationMap.find(ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_SPAWN);
						if(TransmutationSpawnItr != TransmutationMap.end())
						{
							const auto& SpawnFnVariant = TransmutationSpawnItr->second;
							try 
							{
								const auto& SpawnFn = std::get<std::function<void(ARyddelmystCharacter* /*TransmutingCharacter*/, const FTransform& /*SpawnTransform*/, const FVector& /*LaunchDirection*/, const std::vector<ASnowball*>& /*Bullets spawned in map*/)>>(SpawnFnVariant);
								UE_LOG(LogTemp, Warning, TEXT("Fire; custom spawn function discovered.  Running it..."));
								SpawnFn(this, SpawnTransform, LaunchDirection, Bullets);
								Spawned = true;
							}
							catch (std::bad_variant_access const& ex)
							{
								UE_LOG(LogTemp, Warning, TEXT("Fire; no spawn transmutation function with expected signature from %s.  Details are: %s"), *FString(Source.first.c_str()), *FString(ex.what()));
							}
						}
					}
				}
				
				// run default spawning behavior iff none of our metamagic sources had a Transmutation[Spawn] function
				if(!Spawned)
				{
				 	for(auto Bullet : Bullets)
				 	{
						UE_LOG(LogTemp, Warning, TEXT("Fire; default spawning Bullet %s"), *Bullet->GetName());
						// todo: should I be using UGameplayStatics::FinishSpawningActor instead here?
						Bullet->FinishSpawning(SpawnTransform);
						Bullet->Cast(this, LaunchDirection);
				 	}
				}

				// apply any other post-spawn metamagic transforms
				for(const auto& Source : SpellMap)
				{
					auto SourceTransmutationItr = Source.second.find(ARyddelmystCharacter::ID_SPELL_PHASE_TRANSMUTATION);
					if(SourceTransmutationItr != Source.second.end())
					{
						const auto& TransmutationMap = SourceTransmutationItr->second;
						for(const auto& Transmutation : TransmutationMap)
						{
							try 
							{
								// check the current Source for post-spawn Transmutation effects, which should only need the Bullet pointer as input and output nothing, and run them
								const auto& TransmutationFn = std::get<std::function<void(ASnowball* /*spell actor to transform*//*, Transform to apply is expected to already be bound or to be created within the lambda*/)>>(Transmutation.second);
								for(auto Bullet : Bullets)
								{
									TransmutationFn(Bullet);
								}
							}
							catch (std::bad_variant_access const& ex)
							{
								UE_LOG(LogTemp, Warning, TEXT("Fire; no post-spawn transmutation function with expected signature from %s.  Details are: %s"), *FString(Source.first.c_str()), *FString(ex.what()));
							}
						}
					}
				}
				// last, we incur the cost of casting the spell
				CDOSnowballAttack->ProcessCosts(this);
			}
		}
	}
}

float ARyddelmystCharacter::GetHealth() 
{
	return CharacterStats->StatsData.StatsMap["HP"];
}

float ARyddelmystCharacter::GetMaxHealth()
{
	return CharacterStats->StatsData.StatsMap["MaxHP"];
}

float ARyddelmystCharacter::GetMagic()
{
	return CharacterStats->StatsData.StatsMap["MP"];
}
float ARyddelmystCharacter::GetMaxMagic()
{
	return CharacterStats->StatsData.StatsMap["MaxMP"];
}

FText ARyddelmystCharacter::GetHealthText()
{
	int32 HP = FMath::RoundHalfFromZero(CharacterStats->StatsData.StatsMap["HP"]);
	FString HPS = FString::FromInt(HP);
	FString FullHPS = FString::FromInt(CharacterStats->StatsData.StatsMap["MaxHP"]);
	FString HealthHUD = HPS + FString(TEXT("/")) + FullHPS;
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

FText ARyddelmystCharacter::GetMagicText()
{
	int32 MP = FMath::RoundHalfFromZero(CharacterStats->StatsData.StatsMap["MP"]);
	FString MPS = FString::FromInt(MP);
	FString FullMPS = FString::FromInt(CharacterStats->StatsData.StatsMap["MaxMP"]);
	FString MagicHUD = MPS + FString(TEXT("/")) + FullMPS;
	FText MPText = FText::FromString(MagicHUD);
	return MPText;
}

void ARyddelmystCharacter::UpdateHealth(float HealthChange)
{
	CharacterStats->StatsData.StatsMap["HP"] += HealthChange;
	CharacterStats->StatsData.StatsMap["HP"] = FMath::Clamp(CharacterStats->StatsData.StatsMap["HP"], 0.0f, CharacterStats->StatsData.StatsMap["MaxHP"]);
	UE_LOG(LogTemp, Warning, TEXT("UpdateHealth; maya HP is %f"), CharacterStats->StatsData.StatsMap["HP"]);
	if (CharacterStats->StatsData.StatsMap["HP"] == 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateHealth; maya HP is now %f, so broadcasting player death event"), CharacterStats->StatsData.StatsMap["HP"]);
		Cast<URyddelmystGameInstance>(GetWorld()->GetGameInstance())->GetEventManager()->PlayerDeathEvent.Broadcast();
	}
}

void ARyddelmystCharacter::UpdateMagic(float MagicChange)
{
	CharacterStats->StatsData.StatsMap["MP"] += MagicChange;
	CharacterStats->StatsData.StatsMap["MP"] = FMath::Clamp(CharacterStats->StatsData.StatsMap["MP"], 0.0f, CharacterStats->StatsData.StatsMap["MaxMP"]);
}

void ARyddelmystCharacter::HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleDamage; ouch for %f to %s"), Damage, *DamagedActor->GetName());
	UpdateHealth(-Damage);
	// todo: would be cleaner to move iframe processing to UHitBoxerComponent or UAttack
	if (DamageCauser)
	{
		if (DamageCauser->Tags.Find(FName(UAttack::TAG_FLAG_IGNORE_IFRAMES)) == INDEX_NONE)
		{
			SetCanBeDamaged(false);
			DamageInvincibilityTimer();
		}
	}
	else
	{
		SetCanBeDamaged(false);
		DamageInvincibilityTimer();
	}
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
	if (ItemClass)
	{
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
			UE_LOG(LogTemp, Error, TEXT("AddInventoryItemFromActor; itemactor's item class %s does not implement the item interface"), *ItemClass->GetName());
			return false;
		}
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("AddInventoryItemFromActor; itemactor's item class came up null; please install an item object to the actor"));
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
		HUD->ShowDialogue(PortraitMap["weary"], NSLOCTEXT("NSFeedback", "KeyInvFull", "My inventory is full!  Insert overflowing junk in my trunk joke here."));
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

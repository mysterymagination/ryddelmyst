// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FawnCharacter.h"
#include "LookitYouPawn.h"
#include "Snowball.h"
#include "IceySnowball.h"
#include "FireSnowball.h"
#include "ElectricSnowball.h"
#include "Item.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "RyddelmystCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;

USTRUCT(BlueprintType)
struct FBattleStats 
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	uint8 Lvl = 1;
	UPROPERTY(EditAnywhere)
	float MagPwr = 10.f;
	UPROPERTY(EditAnywhere)
	float MagDef = 10.f;
	UPROPERTY(EditAnywhere)
	float Atk = 10.f;
	UPROPERTY(EditAnywhere)
	float Def = 10.f;
};

UCLASS(config=Game)
class ARyddelmystCharacter : public AFawnCharacter
{
	GENERATED_BODY()

	static const FString EquipSlotsData[];

	/**
	 * @brief mapping of string to inner maps that eventually terminate in a leaf node value of one of a set of unioned std::functions with various signatures as required for various effects. Phase order is Conjuration -> Evocation -> Enchantment -> Transmutation
	 * 
	 */
	std::unordered_map<std::string /*SpellID, which is derived from the UClass::GetName() result for classes like AFireSnowball*/,
		std::unordered_map<std::string /*SourceID, which is derived from the UClass:GetName() result for classes like UDiademHellfireMightItem*/,
			std::unordered_map<std::string /*PhaseID*/,
				std::unordered_map<std::string /*MetamagicID*/, std::variant<
						/*creation function*/
						std::function<std::vector<ASnowball*>/*created instances*/(ARyddelmystCharacter* /*creating character*/)>,
						/*spawn function*/
						std::function<void(ARyddelmystCharacter* /*TransmutingCharacter*/, const FTransform& /*SpawnTransform*/, const FVector& /*LaunchDirection*/, const std::vector<ASnowball*>& /*Bullets spawned in map*/)>,
						/*transform function*/
						std::function<void(ASnowball* /*TransformedActor*/, const FTransform& /*transform data*/)>,
						/*attr modifier function*/
						std::function<void(ASnowball* /*bullet to modify*/)>,
						/*attr modifier function for FireSnowball specifically*/
						//std::function<void(AFireSnowball* /*bullet to modify*/)>, // todo: why tf does this not work?  With it uncommented all of sudden everyone trying to install lambdas with the ASnowball sig throw "error no viable overloaded '='..."
						/*damage modifier function*/
						std::function<void(ASnowball* /*bullet to modify*/, float /*damage scaling factor*/, TSubclassOf<UDamageType> /*changing the damage type*/)>,
						/*duration modifier function*/
						std::function<void(ASnowball* /*bullet to modify*/, float /*duration scaling factor*/)>,
						/*enchantment function*/
						std::function<void(AActor* /*EnchantedActor*/, const FHitResult& /*HitResult data*/)>
					>
				>
			>
		>
	> MetamagicMap;
	
	// todo: multidimensional mapping of spell names to metamagic source id to spell aspect (borrowed fun names from the Pathfinder schools o' magic; they just serve to inform the timing and manner of lambda calling/application) to categorical metamagic fx to the actual function to run e.g.
	// {
	//   "snowball" : {
	//     "ID_CracklingVioletVial" : {
	//       "Conjuration" : {
	//         "CreationModifier" : lambda fn that creates one or more bullet Actors and returns the resultant ASnowball array 
	//       }
	//       "Evocation" : {
	//         "DamageModifier" : lambda fn that doubles damage,
	//	       "DurationModifier" : lambda fn that doubles duration
	//       },
	//       "Enchantment" : {
	//         "EffectModifier" : lambda fn that applies an effect when the spell hits a target
	//       },
	//       "Transmutation" : {
	//         "SpawnModifier" : lambda fn that defines how/how many/where/when etc. the spell Actor is spawned, if relevant...
	//       }
	//     }
	//   } 
	// }
	// In this example we'd cast a Snowball and see that we have a DamageModifier and DurationModifier functions under Evocation map (behavior modifying instance creation params), so those get called right away at instance creation and prior to spawning in the map.  Then we see we an EffectModifier under Transmutation map (behavior applied after the spell is completed/hits a target) and so we add its function to the spell's OnHit effects.  Finally we see we have a SpawnModifier under Conjuration map (behavior modifying how/where/when an Actor related to the spell is spawned in the map, if relevant) so we run that instead of the default spawn behavior.
	

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCameraComponent;

	/** Handle to an Actor the Character grabbed during interaction event */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	AActor* GrabbedActor;

	/** Handle to the HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	class ARyddelmystHUD* HUD;

private:
	UPROPERTY()
	bool FirstPersonCameraMode = true;
	UPROPERTY()
	bool IsRunning = false;
	UPROPERTY()
	uint8 SelectedWeaponIdx = 0;
	UPROPERTY()
	FTimeline MyTimeline;
	UPROPERTY()
	float CurveFloatValue;
	UPROPERTY()
	float TimelineValue;
	UPROPERTY()
	FTimerHandle MagicTimerHandle;
	UPROPERTY()
	FTimerHandle InvincibilityTimerHandle;
	UPROPERTY()
	FTimerHandle InventoryTimerHandle;
	UPROPERTY()
	uint8 SelectedItemIdx = 0;
	UPROPERTY()
	bool IsInventorySleeping = false;

public:
	ARyddelmystCharacter();
	virtual void OnLostFollower(ILookitYou* lookitYou) override;
	UFUNCTION(BlueprintCallable, Category = "Statistics")
	FBattleStats GetStats() { return CharacterStats; }
	auto& GetMetamagicMap() { return MetamagicMap; };

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	static const std::string ID_SPELL_ELECTRICSNOWBALL;
	static const std::string ID_SPELL_FIRESNOWBALL;
	static const std::string ID_SPELL_SNOWBALL;
	/**
	 * @brief Phase associated with creating one or more Actors to represent the spell effect in the world e.g. creating enough Actors for a spread shot pattern
	 * 
	 */
	static const std::string ID_SPELL_PHASE_CONJURATION;
	/**
	 * @brief Phase associated with determining and modifying the parameters of a spell's Actors e.g. damage dealt
	 * 
	 */
	static const std::string ID_SPELL_PHASE_EVOCATION;
	/**
	 * @brief Phase associated with installing behavior in the spell's Actors to be run later in their OnHit callback e.g. applying knockback to a target
	 * 
	 */
	static const std::string ID_SPELL_PHASE_ENCHANTMENT;
	/**
	 * @brief Phase associated with spawning and positioning the spell's Actors e.g. arranging bullets in a spread shot pattern
	 * 
	 */
	static const std::string ID_SPELL_PHASE_TRANSMUTATION;
	/**
	 * @brief Run during the Conjuration phase, Creation functions determine how many/which Actors will be spawned to represent the spell effect.  Only one Creation function can be active for a given spell at one time.
	 * 
	 */
	static const std::string ID_METAMAGIC_CATEGORY_CREATION;
	/**
	 * Run during the Evocation phase, Attribute functions modify a given spell instance in some way, which is a black box to the caller 
	 */
	static const std::string ID_METAMAGIC_CATEGORY_ATTR;
	/**
	 * @brief Run during the Evocation phase, Damage functions modify spell damage specifically; std::bind can be used to pre-package the desired scaling factor etc. so that the resultant signature looks like that of ATTR above.
	 * 
	 */
	static const std::string ID_METAMAGIC_CATEGORY_DAMAGE;
	/**
	 * @brief Run during the Evocation phase, Duration functions modify spell duration specifically; std::bind can be used to pre-package the desired scaling factor etc. so that the resultant signature looks like that of ATTR above.
	 * 
	 */
	static const std::string ID_METAMAGIC_CATEGORY_DURATION;
	/**
	 * @brief Installed during the Enchantment phase and run at spell Actors' OnHit callback, Effect functions apply their behavior to a target Actor stricken by the spell bullet e.g. knocking the stricken Actor back with a physics impulse.
	 * 
	 */
	static const std::string ID_METAMAGIC_CATEGORY_EFFECT;
	/**
	 * @brief Run during the Transmutation phase, Spawn functions determine how/where the spell should spawn its representative Actor(s).  Only one Spawn function can be active for a given spell at one time; further transforms can be applied with ID_METAMAGIC_CATEGORY_TRANSFORM.
	 * 
	 */
	static const std::string ID_METAMAGIC_CATEGORY_SPAWN;
	/**
	 * @brief Run during the Transmutation phase, Transform functions apply post-spawn transforms to the spell's Actor(s)
	 * 
	 */
	static const std::string ID_METAMAGIC_CATEGORY_TRANSFORM;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseLookUpRate;

	/** Handle to our LookitYou impl, which will be spawned when user inputs Free Cam Mode action.  */
	UPROPERTY()
	class ALookitYouPawn* LookitYouGo;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	// Distance at which the player can interact with objects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MaxInteractDistance = 200.f;

	// Max number of items that can be held in the inventory.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	uint8 MaxInventory = 4;

	// Distance at which the player will carry objects out from their mesh on X axis.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float CarryDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health = FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float FullMagic = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float Magic = FullMagic;

protected:
	// List of weapons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TArray<TSubclassOf<ASnowball>> Spells;
	// Array of inventory items
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UObject*> Inventory;
	// Mapping of equip slot string keys to equipped item objects
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TMap<FString, UObject*> Equipment;
	UPROPERTY(EditAnywhere, Category = "Statistics")
	FBattleStats CharacterStats;

protected:
	/** Handles switching between our cam and the LookitYouPawn's "3rd person" cam */
	void CameraToggle();

	/** Handles switching player input control over to LookitYouGo */
	void SendControl();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to Run toggle event
	 */
	void Run();

	/**
	 * Called via input to Cycle Weapon Up event
	 */
	void CycleWeaponUp();

	/**
	 * Called via input to Cycle Weapon Down event
	 */
	void CycleWeaponDown();

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void LookUp(float Value);
	/**
	 * Translates the character by 100 units in the direction they're facing to help escape stuck in geometry situations  
	 */
	void FixMe();

	/**
	 * Pauses the game and raises the pause menu.
	 */ 
	UFUNCTION()
	void PauseGame();

	/**
	 * Handles firing projectiles.
	 */ 
	UFUNCTION()
	void Fire();

	/**
	 * Handles activating interactable objects.
	 */
	UFUNCTION()
	void Interact();

	/**
	 * Handles crouch position toggle.
	 */
	UFUNCTION()
	void HandleCrouch();

	/**
	 * Traces an interact ray from the first person camera and returns the hit result
	 * @return HitResult a HitResult containing any Actor hit by the interact ray
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	FHitResult FireInteractRay();

	/**
	 * Adds a given Item to the inventory based on the Item class stored in the given ItemActor, then destroys the ItemActor.
	 * @param ItemActor the Actor representing the item in the stage, whose ItemType field defines what actual Item object will be added to the player inventory.
	 * @return true if the item was successfully added to inventory, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddInventoryItemFromActor(class AItemActor* ItemActor);

	/**
	 * Adds a given Item to the inventory.
	 * @param ItemObj the Item object that will be added to the player inventory.
	 * @return true if the item was successfully added to inventory, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddInventoryItem(UObject* ItemObj);

	/**
	 * Adds a given Item to the character's equipment.
	 * @param ItemObj the Item object that will be added to the player's equipment, granting its passive effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddEquippedItem(UObject* ItemObj);

	/** Update Health data and UI */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealth(float HealthChange);

	/** Update Magic data and UI */
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void UpdateMagic(float MagicChange);

	UFUNCTION()
	void HandleDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, class UDamageType* DamageType, AActor* DamageCauser);

	/** Damage Timer starts the invincibility clock after taking damage */
	UFUNCTION()
	void DamageInvincibilityTimer();

	/** Marks that the actor can be damaged again */
	UFUNCTION()
	void SetDamageState();

	/**
	 * Runs the selected Item's OnUse() iff an Item is selected.
	 */
	UFUNCTION()
	void UseItem();

	/**
	 * Cycles through the player's Inventory, selecting an item at the incremented index if the Value is positive and decremented index if Value is negative
	 * @param Value negative to cycle backwards/down and positive to cycle forwards/up
	 */
	UFUNCTION()
	void CycleItem(float Value);

	/** Wakes the inventory system to user input */
	UFUNCTION()
	void WakeInventory();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Get Health */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth();

	/** Get full Health value */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth();

	/** Get Health for display */
	UFUNCTION(BlueprintPure, Category = "Health")
	FText GetHealthText();

	/** Get Magic */
	UFUNCTION(BlueprintPure, Category = "Magic")
	float GetMagic();

	/** Get full Magic value */
	UFUNCTION(BlueprintPure, Category = "Magic")
	float GetMaxMagic();

	/** Get Magic for display */
	UFUNCTION(BlueprintPure, Category = "Magic")
	FText GetMagicText();

private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};


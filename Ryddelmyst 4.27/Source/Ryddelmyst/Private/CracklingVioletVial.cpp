// Fill out your copyright notice in the Description page of Project Settings.


#include "CracklingVioletVial.h"
#include "RyddelmystCharacter.h"
#include "ElectricSnowball.h"

const std::string UCracklingVioletVial::ID = "CracklingVioletVial";

void UCracklingVioletVial::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		// first for spreadshot we need to install conjuration metamagic to make the electric snowball spell create 5 AElectricSnowball Actors and return them in an array.  Later phases will then operate on that array.
		Character->GetMetamagicMap()[std::string(TCHAR_TO_UTF8(*AElectricSnowball::StaticClass()->GetName()))][std::string(TCHAR_TO_UTF8(*UCracklingVioletVial::StaticClass()->GetName()))][ARyddelmystCharacter::ID_SPELL_PHASE_CONJURATION][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_CREATION] = [=](ARyddelmystCharacter* ConjuringCharacter) -> std::vector<ASnowball*>
		{
			std::vector<ASnowball*> ResultArray;
			FTransform SpawnTransform;
			SpawnTransform.SetIdentity();
			for (int idx = 0; idx < 5; idx++)
			{
				ResultArray.emplace_back(ConjuringCharacter->GetWorld()->SpawnActorDeferred<AElectricSnowball>(AElectricSnowball::StaticClass(), SpawnTransform, ConjuringCharacter, ConjuringCharacter->GetInstigator()));
			}
			return ResultArray;
		};

		// finally for spreadshot we need to install transmutation metamagic that causes the electric snowball spell's 5 Actors to launch in direction vectors that have been rotated procedurally to form a fan shaped uniform distribution of rotation angles from the primary launch direction given by the caster's forward vector
		Character->GetMetamagicMap()[std::string(TCHAR_TO_UTF8(*AElectricSnowball::StaticClass()->GetName()))][std::string(TCHAR_TO_UTF8(*UCracklingVioletVial::StaticClass()->GetName()))][ARyddelmystCharacter::ID_SPELL_PHASE_TRANSMUTATION][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_SPAWN] = [=](ARyddelmystCharacter* TransmutingCharacter, const FTransform& SpawnTransform, const FVector& LaunchDirection, const std::vector<ASnowball*>& Bullets)
		{
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Electric lambda; transmutation behavior"));
			// we want our spreadshot pattern to spawn N electric snowballs in a uniform distribution arc; assuming N is odd, take the actual given launch direction and use it as the center snowball (0 degrees rotation), then make (N-1)/2 vectors rotated ccw and the same number rotated cw.  The angle between each 'fanning' vector will be 90/((N-1)/2 + 1) degrees (the +1 to account for the fact that we don't want to actually reach a full 90 degrees so we're creating a sort of phantom extra vector for the distribution that doesn't actually get used), so for 5 snowballs we'd have 0 degrees, 30 degrees, 60 degrees, -30 degrees, and -60 degrees vectors.
			size_t NumBullets = Bullets.size();
			// the number of bullets that will be to the ccw and cw of the central bullet, forming the sides of the fan shape
			size_t NumBulletsPerSide = (NumBullets - 1) / 2;
			// Distribution angle is the angle between our uniformly distributed vectors.  We calculate this per-side as 90 degrees divided by number of bullets per side +1 since we don't want to actually fire bullets at a 90 degree angle to the central launch direction vector (e.g. out to the side of the character) so we pretend there's an extra item in the per-side distribution when calculating the angle per bullet.  Another way of putting it is that we're calculating the angle of the sectors between the bullet 'edges' in our fan, which will be N+1 where N is the number of edges.
			float DistAngle = 90 / (NumBulletsPerSide + 1);
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Electric lambda; transmutation behavior -- NumBullets is %uz, NumBulletsPerSide is %uz, and DistAngle is %f"), NumBullets, NumBulletsPerSide, DistAngle);
			float CurrentAngle = -90;
			for (auto Bullet : Bullets)
			{
				CurrentAngle += DistAngle;
				FRotator DistRotation(0.f, CurrentAngle, 0.f);
				Bullet->FinishSpawning(SpawnTransform);
				FVector ModifiedLaunchDirection = DistRotation.RotateVector(LaunchDirection);
				Bullet->Cast(TransmutingCharacter, ModifiedLaunchDirection);
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Electric lambda; transmutation behavior -- current angle is %f, DistRotation is %s, SpawnTransform is %s, LaunchDirection is %s and ModifiedLaunchDirection is %s"), CurrentAngle, *DistRotation.ToString(), *SpawnTransform.ToString(), *LaunchDirection.ToString(), *ModifiedLaunchDirection.ToString());
			}
		};
	}
}
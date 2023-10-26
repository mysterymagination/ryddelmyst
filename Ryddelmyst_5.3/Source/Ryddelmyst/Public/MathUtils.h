// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <random>

/**
 * Math utility functions
 */
class RYDDELMYST_API MathUtils
{
public:
	/**
	 * Simulates rolling N dice with M sides each e.g. 5d6 will give a random number between 5 and 30 
	 */
	static uint8 RollNdM(uint8 NumberOfDice, uint8 NumberOfSides)
	{
		// random number generation in range of N to NM
		std::random_device random_device;
		std::default_random_engine generator(random_device());
		std::uniform_int_distribution<unsigned int> distribution(NumberOfDice, NumberOfDice * NumberOfSides);
		return static_cast<uint8>(distribution(generator));
	};
	/**
	 * Get a random integer in the given range 
	 * @param min inclusive range minimum
	 * @param max inclusive range maximum
	 * @return a random integer in the given range
	 */
	static int64 RandomInRange(int64 min, int64 max)
	{
		// random number generation in range of min to max
		std::random_device random_device;
		std::default_random_engine generator(random_device());
		std::uniform_int_distribution<int64> distribution(min, max);
		return distribution(generator);
	};
	/**
	 * Calculates the average value of the components of a 3D vector such as you might receive from GetActorScale(), in which case you get back a scaler scalar ;) 
	 */
	static float MeanVector3D(const FVector& InVector)
	{
		uint32 sum = InVector.X + InVector.Y + InVector.Z;
		return sum/3.f;
	}
};

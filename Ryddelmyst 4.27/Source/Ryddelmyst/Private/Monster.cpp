// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"




// todo: how does one modify the max speed of an AI Actor?  Only CharacterMovementComponent seems to expose speed setter functions... might need a custom subclass. EDIT: nope, CharacterMovementComponent is fine to use on AI Actors; the only distinction between a Pawn and a Character is that the latter has a Mesh, collision, and defined movement behavior.
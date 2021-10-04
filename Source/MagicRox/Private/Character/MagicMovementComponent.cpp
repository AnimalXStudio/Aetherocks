// Copyright AnimalXStudio 2021

#include "Character/MagicMovementComponent.h"

UMagicMovementComponent::UMagicMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bUseControllerDesiredRotation = true;
}

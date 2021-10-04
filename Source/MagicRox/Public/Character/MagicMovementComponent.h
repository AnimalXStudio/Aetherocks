// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MagicMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class MAGICROX_API UMagicMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UMagicMovementComponent(const FObjectInitializer& ObjectInitializer);
};

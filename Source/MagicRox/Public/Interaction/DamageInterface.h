// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Character/MagicCharacter.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MAGICROX_API IDamageInterface
{
	GENERATED_BODY()
	
public:

	virtual bool TakeDamage(AMagicCharacter* DamagingCharacter, float DamagePoints) = 0;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tools/MagicToolBase.h"
#include "AetherElevator.generated.h"

/**
 * 
 */
UCLASS()
class MAGICROX_API AAetherElevator : public AMagicToolBase
{
	GENERATED_BODY()

public:

	AAetherElevator(const FObjectInitializer& ObjectInitializer);
	virtual bool CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage) override;
	virtual void OnInteractionStarted(AMagicCharacter* TargetingCharacter) override;
	
};

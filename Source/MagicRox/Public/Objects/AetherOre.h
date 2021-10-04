// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tools/MagicToolBase.h"
#include "AetherOre.generated.h"

/**
 * 
 */
UCLASS()
class MAGICROX_API AAetherOre : public AMagicToolBase
{
	GENERATED_BODY()

public:

	AAetherOre(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnInteractionStarted(AMagicCharacter* TargetingCharacter) override;
	virtual bool CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage) override;

};

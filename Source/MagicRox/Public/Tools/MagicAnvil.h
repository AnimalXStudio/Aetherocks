// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tools/MagicToolBase.h"
#include "MagicAnvil.generated.h"

/**
 * 
 */
UCLASS()
class MAGICROX_API AMagicAnvil : public AMagicToolBase
{
	GENERATED_BODY()

public:

	AMagicAnvil(const FObjectInitializer& ObjectInitializer);
	
	virtual bool CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage) override;
	virtual void OnInteractionStarted(AMagicCharacter* TargetingCharacter) override;

};

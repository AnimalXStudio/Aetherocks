// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/MagicToolBase.h"
#include "MagicStabilityCrystal.generated.h"

/**
 * 
 */
UCLASS()
class MAGICROX_API AMagicStabilityCrystal : public AMagicToolBase
{
	GENERATED_BODY()
	
public:

	AMagicStabilityCrystal(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual bool CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage) override;
	virtual void OnInteractionStarted(AMagicCharacter* TargetingCharacter) override;

	float GetFuelPercent() const { return CurrentFuelLevel; }
protected:

	UPROPERTY(EditDefaultsOnly, Category="Components")
	UParticleSystemComponent* FireComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UAudioComponent* IdleFireComponent = nullptr;
	
	//Curve that specifies how fast fuel is burned given the game time input
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	UCurveFloat* FuelDrainRateCurve = nullptr;
	//Curve that specifies how fast stability drains given the current fuel level
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	UCurveFloat* StabilityDrainRateCurve = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinFuelValue = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxFuelLevel = 15.0f;
	
	float CurrentFuelLevel = 100.0f;
	float GameTime = 0.0f;
};

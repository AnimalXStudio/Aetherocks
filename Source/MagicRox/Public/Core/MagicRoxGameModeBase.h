// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Character/MagicCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "World/MagicStabilityCrystal.h"
#include "MagicRoxGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MAGICROX_API AMagicRoxGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AMagicRoxGameModeBase(const FObjectInitializer& ObjectInitializer);

	static AMagicRoxGameModeBase* Get(const UObject* WorldContextObject);

	void SetStabilityCrystal(AMagicStabilityCrystal* Crystal) { CachedStabilityCrystal = Crystal; };
	AMagicStabilityCrystal* GetStabilityCrystal() const { return CachedStabilityCrystal.Get(); }
	AMagicCharacter* GetCachedLocalPlayer() const;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	void OnPlayerHitByRock();
private:
	
	UPROPERTY(Transient)
	mutable TWeakObjectPtr<AMagicCharacter> CachedLocalPlayer;
	UPROPERTY(Transient)
	TWeakObjectPtr<AMagicStabilityCrystal> CachedStabilityCrystal;

////////////////////////////////////////////////////////////////////////////////
// Stability system
////////////////////////////////////////////////////////////////////////////////

public:

	float GetStabilityPercent() const;
	void SetDrainRate(float NewDrainRate);

	float GetScoreForDeposit(int32 NumDeposited) const;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category="Scoring")
	UCurveFloat* StabilityScoreCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Scoring")
	UCurveFloat* StabilityRockPenaltyCurve = nullptr;
	
	float CurrentStabilityLevel = 100.0f;
	float CurrentDrainRate = 0.0f;
};

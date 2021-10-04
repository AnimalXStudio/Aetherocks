// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MagicGameInstance.generated.h"

/**
 * 
 */
UCLASS(Config="MagicGameDetails")
class MAGICROX_API UMagicGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static UMagicGameInstance* Get(const UObject* WorldContextObject);

	void ResetGame();
	
////////////////////////////////////////////////////////////////////////////////
// Player inventory
////////////////////////////////////////////////////////////////////////////////

	UPROPERTY()
	int32 NumCarriedAether = 0;
	UPROPERTY()
	int32 MaxNumCarriedAether = 5;

////////////////////////////////////////////////////////////////////////////////
// Scoring
////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable)
	float GetGameScore() const {return GameScore; }
	void DepositAether();
	
protected:

	UPROPERTY()
	float GameScore = 0;
};

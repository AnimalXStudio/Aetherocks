// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Character/MagicCharacter.h"
#include "GameFramework/HUD.h"
#include "MagicHUD.generated.h"

class ALucidCharacter;

UENUM(BlueprintType)
enum class EPickupState : uint8 
{
	PickUp_No,
	PickUp_Invalid,
	PickUp_Yes
};

UENUM(BlueprintType)
enum class EInteractState : uint8
{
	Interact_No,
	Interact_Invalid,
	Interact_Yes
};

USTRUCT(BlueprintType)
struct FMagicInventory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int NumCarriedOres = 0;
	UPROPERTY(BlueprintReadOnly)
	int MaxNumCarriedOres = 0;
};

/**
 * 
 */
UCLASS()
class MAGICROX_API AMagicHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	static AMagicHUD* GetInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetPlayerEnergyPercentLeft() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EInteractState GetInteractionState() const { return m_InteractionState; };
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EPickupState GetPickupState() const {return m_PickUpState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FMagicInventory GetInventory() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStabilityPercent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetFuelPercent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsWieldingPickaxe() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetPickaxeDurability() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetGameScore() const;
	
	//HUD Events
	void OnInteractionStateChanged(EInteractState NewInteractionState) { m_InteractionState = NewInteractionState; }
	void OnPickUpStateChanged(EPickupState NewPickupState) { m_PickUpState = NewPickupState; }

	//Shows a text message to the player
	//Text is styled based on message priority, higher priority takes well, priority...
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowHUDMessage(const FString& MessageString, float MessageDuration, int32 MessagePriority);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetHUDVisibility(bool bNewVisibility, float Delay);

private:
	
	AMagicCharacter* GetCachedLocalPlayer() const;
	mutable TWeakObjectPtr<AMagicCharacter> m_CachedCharacter = nullptr;

	//State variables
	EInteractState m_InteractionState;
	EPickupState m_PickUpState;
};

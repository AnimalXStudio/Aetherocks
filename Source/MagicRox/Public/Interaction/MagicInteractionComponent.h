// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/MagicCharacter.h"
#include "MagicInteractionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInteractionDelegate, AMagicCharacter*)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAGICROX_API UMagicInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMagicInteractionComponent();

	float GetInteractionRange() const { return m_InteractionRange; };

	FInteractionDelegate& GetInteractionStartedDelegate() { return m_InteractionStartedDelegate; };
	FInteractionDelegate& GetOnBecomeActiveTargetDelegate() { return m_OnBecomeActiveTarget; };
	FInteractionDelegate& GetOnExitActiveTargetDelegate() { return m_OnExitActiveTarget; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

////////////////////////////////////////////////////////////////////////////////
// Interactions settings
////////////////////////////////////////////////////////////////////////////////

	//From how far away can the character interact with the owner
	UPROPERTY(EditDefaultsOnly)
	float m_InteractionRange = 250.0f;

private:
		
	void Internal_OnBecomeActiveTarget(AMagicCharacter* InteractingActor);
	void Internal_OnExittActiveTarget(AMagicCharacter* InteractingActor);
	void Internal_OnInteractionStarted(AMagicCharacter* InteractingActor);

	FInteractionDelegate m_InteractionStartedDelegate;
	FInteractionDelegate m_OnBecomeActiveTarget;
	FInteractionDelegate m_OnExitActiveTarget;

////////////////////////////////////////////////////////////////////////////////
// Interaction State
////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION(BlueprintCallable)
	bool GetIsTargeted() const { return bIsBeingTargeted; }

	UFUNCTION(BlueprintCallable)
	AMagicCharacter* GetTargetingCharacter() const { return m_TargetingActor.Get(); }

protected:
	bool bIsBeingTargeted = false;
	TWeakObjectPtr<AMagicCharacter> m_TargetingActor = nullptr;
};

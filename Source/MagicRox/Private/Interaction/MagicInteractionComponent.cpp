// Copyright AnimalXStudio 2021


#include "Interaction/MagicInteractionComponent.h"

UMagicInteractionComponent::UMagicInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	GetInteractionStartedDelegate().AddUObject(this, &ThisClass::Internal_OnInteractionStarted);
	GetOnBecomeActiveTargetDelegate().AddUObject(this, &ThisClass::Internal_OnBecomeActiveTarget);
	GetOnExitActiveTargetDelegate().AddUObject(this, &ThisClass::Internal_OnExittActiveTarget);
}

void UMagicInteractionComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UMagicInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetIsTargeted())
	{
		//Notify the targeting actor we are no longer a valid target
		GetOnExitActiveTargetDelegate().Broadcast(GetTargetingCharacter());
	}

	GetInteractionStartedDelegate().RemoveAll(this);
	GetOnBecomeActiveTargetDelegate().RemoveAll(this);
	GetOnExitActiveTargetDelegate().RemoveAll(this);
}

void UMagicInteractionComponent::Internal_OnBecomeActiveTarget(AMagicCharacter* InteractingActor)
{
	bIsBeingTargeted = true;
	m_TargetingActor = InteractingActor;
}

void UMagicInteractionComponent::Internal_OnExittActiveTarget(AMagicCharacter* InteractingActor)
{
	bIsBeingTargeted = false;
	m_TargetingActor = InteractingActor;
}

void UMagicInteractionComponent::Internal_OnInteractionStarted(AMagicCharacter* InteractingActor)
{
	
}

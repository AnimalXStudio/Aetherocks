// Fill out your copyright notice in the Description page of Project Settings.


#include "World/AetherElevator.h"

#include "Core/MagicGameInstance.h"
#include "Kismet/GameplayStatics.h"

AAetherElevator::AAetherElevator(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	m_CanBePickedUp = false;
	m_AllowInstantActiation = true;
	m_EnergyCost = 0.f;
	m_UsageCooldownTime = 10.0f;
}

bool AAetherElevator::CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage)
{
	const bool bSuperCanInteract = Super::CanInteract(TargetingActor, bShowHUDMessage);

	if(const UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
	{
		return bSuperCanInteract && GameInstance->NumCarriedAether > 0;
	}
	return bSuperCanInteract;
}

void AAetherElevator::OnInteractionStarted(AMagicCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);

	if(UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
	{
		GameInstance->DepositAether();
	}
	UGameplayStatics::PlaySoundAtLocation(this, InteractionSFX, GetActorLocation());
}

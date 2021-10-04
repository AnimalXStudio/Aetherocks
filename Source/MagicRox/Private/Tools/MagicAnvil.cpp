// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MagicAnvil.h"

#include "Character/MagicCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/MagicPickaxe.h"

AMagicAnvil::AMagicAnvil(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	m_CanBePickedUp = false;
	m_EnergyCost = 0.f;
	m_AllowInstantActiation = true;
	m_UsageCooldownTime = 1.0f;
}

bool AMagicAnvil::CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage)
{
	if(TargetingActor)
	{
		if(const AMagicPickaxe* Pickaxe = Cast<AMagicPickaxe>(TargetingActor->GetCurrentPickUp()))
		{
			return Pickaxe->GetCurrentDurabilityPercent() < 100.0f;
		}
	}
	return false;
}

void AMagicAnvil::OnInteractionStarted(AMagicCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);

	if(TargetingCharacter)
	{
		if(AMagicPickaxe* Pickaxe = Cast<AMagicPickaxe>(TargetingCharacter->GetCurrentPickUp()))
		{
			UGameplayStatics::PlaySoundAtLocation(this, InteractionSFX, GetActorLocation());
			Pickaxe->Repair();
		}
	}
}

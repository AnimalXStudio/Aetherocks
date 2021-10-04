// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/AetherOre.h"

#include "Core/MagicGameInstance.h"

AAetherOre::AAetherOre(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	m_EnergyCost = 0;
	m_AllowInstantActiation = true;
	m_ShouldDestroyAfterInteraction = true;
	m_CanBePickedUp = true;
	m_ShouldSimulatePhysics = true;
	if(m_CollisionComponent)
	{
		m_CollisionComponent->SetSimulatePhysics(true);
	}
}

void AAetherOre::OnInteractionStarted(AMagicCharacter* TargetingCharacter)
{
	if(UMagicGameInstance* GI = UMagicGameInstance::Get(this))
	{
		++GI->NumCarriedAether;
	}
	Super::OnInteractionStarted(TargetingCharacter);
}

bool AAetherOre::CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage)
{
	bool CanAddToInventory = true;
	if(UMagicGameInstance* GI = UMagicGameInstance::Get(this))
	{
		CanAddToInventory = GI->NumCarriedAether < GI->MaxNumCarriedAether;
	}
	return CanAddToInventory && Super::CanInteract(TargetingActor, bShowHUDMessage);
}
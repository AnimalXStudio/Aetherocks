// Copyright AnimalXStudio 2021


#include "UI/MagicHUD.h"

#include "Core/MagicGameInstance.h"
#include "Core/MagicRoxGameModeBase.h"
#include "Tools/MagicPickaxe.h"

AMagicHUD* AMagicHUD::GetInstance(const UObject* WorldContextObject)
{
	if (const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr)
	{
		if (const APlayerController* PC = World->GetFirstPlayerController<APlayerController>())
		{
			return Cast<AMagicHUD>(PC->GetHUD());
		}
	}
	return nullptr;
}

float AMagicHUD::GetPlayerEnergyPercentLeft() const
{
	if (const AMagicCharacter* CachedCharacter = GetCachedLocalPlayer())
	{
		//Hard coded as I won't change it from 100 for the jam.
		return CachedCharacter->GetRemainingEnergy() / 100.0f;
	}
	return 0.0f;
}

FMagicInventory AMagicHUD::GetInventory() const
{
	if(const UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
	{
		FMagicInventory Inventory;
		Inventory.NumCarriedOres = GameInstance->NumCarriedAether;
		Inventory.MaxNumCarriedOres = GameInstance->MaxNumCarriedAether;
		return Inventory;
	}
	return FMagicInventory{};
}

float AMagicHUD::GetStabilityPercent() const
{
	if(const AMagicRoxGameModeBase* GameMode = AMagicRoxGameModeBase::Get(this))
	{
		return GameMode->GetStabilityPercent();
	}
	return 0.0f;
}

float AMagicHUD::GetFuelPercent() const
{
	if(const AMagicRoxGameModeBase* GameMode = AMagicRoxGameModeBase::Get(this))
	{
		if(const AMagicStabilityCrystal* Crystal = GameMode->GetStabilityCrystal())
		{
			return Crystal->GetFuelPercent();
		}
	}
	return 0.0f;
}

bool AMagicHUD::IsWieldingPickaxe() const
{
	if(const AMagicCharacter* CurrentCharacter = GetCachedLocalPlayer())
	{
		if(const AMagicToolBase* Tool = CurrentCharacter->GetCurrentPickUp())
		{
			return Tool->IsA(AMagicPickaxe::StaticClass());
		}		
	}
	return false;
}

float AMagicHUD::GetPickaxeDurability() const
{
	if(const AMagicCharacter* CurrentCharacter = GetCachedLocalPlayer())
	{
		if(const AMagicPickaxe* Pickaxe = Cast<AMagicPickaxe>(CurrentCharacter->GetCurrentPickUp()))
		{
			return Pickaxe->GetCurrentDurabilityPercent();
		}		
	}
	
	return .0f;
}

float AMagicHUD::GetGameScore() const
{
	if(const UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
	{
		return GameInstance->GetGameScore();
	}
	return 0.f;
}

AMagicCharacter* AMagicHUD::GetCachedLocalPlayer() const
{
	if (AMagicCharacter* CachedCharacter = m_CachedCharacter.Get())
	{
		return CachedCharacter;
	}

	if (const AMagicRoxGameModeBase* GM = AMagicRoxGameModeBase::Get(this))
	{
		m_CachedCharacter = GM->GetCachedLocalPlayer();
		return m_CachedCharacter.Get();
	}
	return nullptr;
}

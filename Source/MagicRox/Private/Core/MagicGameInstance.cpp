// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MagicGameInstance.h"

#include "Core/MagicRoxGameModeBase.h"

UMagicGameInstance* UMagicGameInstance::Get(const UObject* WorldContextObject)
{
	if(WorldContextObject)
	{
		if(const UWorld* World = WorldContextObject->GetWorld())
		{
			return World->GetGameInstance<UMagicGameInstance>();
		}
	}
	return nullptr;
}

void UMagicGameInstance::ResetGame()
{
	NumCarriedAether = 0;
	GameScore = 0;
}

void UMagicGameInstance::DepositAether()
{
	if(const AMagicRoxGameModeBase* GameModeBase = AMagicRoxGameModeBase::Get(this))
	{
		GameScore += GameModeBase->GetScoreForDeposit(NumCarriedAether);
	}
	else
	{
		GameScore += NumCarriedAether;
	}

	NumCarriedAether = 0;
}

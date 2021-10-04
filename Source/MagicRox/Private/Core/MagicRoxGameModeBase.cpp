// Copyright AnimalXStudio 2021
#include "Core/MagicRoxGameModeBase.h"

#include "Character/MagicCharacter.h"
#include "Core/MagicGameInstance.h"
#include "Kismet/GameplayStatics.h"

AMagicRoxGameModeBase::AMagicRoxGameModeBase(const FObjectInitializer& ObjectInitializer)
{
	DefaultPawnClass = AMagicCharacter::StaticClass();
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetActorTickInterval(0.0f);
	SetActorTickEnabled(true);
}

PRAGMA_DISABLE_OPTIMIZATION // #Sumo_DisableOptimisation
AMagicRoxGameModeBase* AMagicRoxGameModeBase::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr)
	{
		return Cast<AMagicRoxGameModeBase>(World->GetAuthGameMode());
	}
	return nullptr;
}

PRAGMA_ENABLE_OPTIMIZATION //Sumo_EnableOptimisation

AMagicCharacter* AMagicRoxGameModeBase::GetCachedLocalPlayer() const
{
	if (AMagicCharacter* LocalCharacter = CachedLocalPlayer.Get())
	{
		return LocalCharacter;
	}

	if (const UWorld* World = GetWorld())
	{
		if (const APlayerController* PC = World->GetFirstPlayerController())
		{
			if (AMagicCharacter* LucidCharacter = Cast<AMagicCharacter>(PC->GetPawn()))
			{
				CachedLocalPlayer = LucidCharacter;
				return LucidCharacter;
			}
		}
	}

	return nullptr;
}

void AMagicRoxGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if(UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
	{
		GameInstance->ResetGame();
	}
}

void AMagicRoxGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentStabilityLevel -= DeltaSeconds * CurrentDrainRate;
	if(CurrentStabilityLevel <= 0.f)
	{
		//Just open the end screen to display the scores
		UGameplayStatics::OpenLevel(this, FName{"MAP_EndGame"});
	}
}

void AMagicRoxGameModeBase::OnPlayerHitByRock()
{
	if(StabilityRockPenaltyCurve)
	{
		CurrentStabilityLevel -= StabilityRockPenaltyCurve->GetFloatValue(CurrentStabilityLevel);
	}
	else
	{			
		//Fallback for missing curve
		CurrentStabilityLevel -= 5.f;
	}
}

float AMagicRoxGameModeBase::GetStabilityPercent() const
{
	return CurrentStabilityLevel;
}

void AMagicRoxGameModeBase::SetDrainRate(float NewDrainRate)
{
	CurrentDrainRate = NewDrainRate;
}

float AMagicRoxGameModeBase::GetScoreForDeposit(int32 NumDeposited) const
{
	if (StabilityScoreCurve)
	{
		return StabilityScoreCurve->GetFloatValue(CurrentStabilityLevel) * NumDeposited;
	}
	return float(NumDeposited);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/StalagmiteManager.h"

#include "Components/AudioComponent.h"
#include "Core/MagicRoxGameModeBase.h"
#include "Kismet/GameplayStatics.h"

TWeakObjectPtr<AStalagmiteManager> AStalagmiteManager::Singleton = nullptr;

// Sets default values
AStalagmiteManager::AStalagmiteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetActorTickEnabled(true);

	WaveSettings.Add(FWaveSettings{});

	TrembleComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TrembleComponent"));
	SetRootComponent(TrembleComponent);
}

AStalagmiteManager* AStalagmiteManager::Get(const UObject* WorldContextObejct)
{
	if(!Singleton.IsValid())
	{
		TArray<AActor*> OutManagers;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObejct, AStalagmiteManager::StaticClass(), OutManagers);
		for (AActor* Manager: OutManagers)
		{
			if(AStalagmiteManager* AsManager = Cast<AStalagmiteManager>(Manager))
			{
				Singleton = AsManager;
				break;
			}
		}
	}
	
	return Singleton.Get();
}

void AStalagmiteManager::MarkStalagtiteFallen(AMagicStalagtite* Fallen)
{
	if(!Fallen)
	{
		return;
	}
	
	const FWaveSettings& WaveSetting = GetCurrentWaveSettings();
	if(FallenStalagtites.Num() == WaveSetting.MaxNumFallen)
	{
		//Mark one of the fallen stalagtites as cleared as the max num fallen has been reached for this stability level
		const int32 RandomIndex = FMath::RandRange(0, FallenStalagtites.Num() - 1);
		FallenStalagtites[RandomIndex]->SetState(State_Cleared);
	}

	const int32 ActiveIndex = ActiveStalagtites.Find(Fallen);
	if(ActiveIndex > INDEX_NONE)
	{
		ActiveStalagtites.RemoveAtSwap(ActiveIndex, 1, false);
	}
	FallenStalagtites.Add(Fallen);
}

void AStalagmiteManager::MarkStalagtiteAvailable(AMagicStalagtite* Available)
{
	if(!Available)
	{
		return;
	}
	
	AvailableLevelStalagtites.Add(Available);
	
	const int32 ActiveIndex = ActiveStalagtites.Find(Available);
	if(ActiveIndex != INDEX_NONE)
	{
		ActiveStalagtites.RemoveAtSwap(ActiveIndex, 1,false);
	}
	
	const int32 FallenIndex = FallenStalagtites.Find(Available);
	if(FallenIndex != INDEX_NONE)
	{
		FallenStalagtites.RemoveAtSwap(FallenIndex, 1, false);
	}
}

// Called when the game starts or when spawned
void AStalagmiteManager::BeginPlay()
{
	Super::BeginPlay();

	const FWaveSettings& WaveSetting = GetCurrentWaveSettings();
	while (IdleStalagtites.Num() != WaveSetting.TargetNumActive && AvailableLevelStalagtites.Num() != 0)
	{
		SetIdleFromAvailablePool();
	}
}

// Called every frame
void AStalagmiteManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FWaveSettings& WaveSetting = GetCurrentWaveSettings();

	if(WaveSetting.StabilityLevel < PreviousStabilityLevel)
	{
		PreviousStabilityLevel = WaveSetting.StabilityLevel;
		if (TrembleComponent)
		{
			TrembleComponent->Play();
		}
	}
	
	//Update current Potential
	int32 CurrentPotential = IdleStalagtites.Num() + ActiveStalagtites.Num() + FallenStalagtites.Num();
	while (CurrentPotential < WaveSetting.TargetNumActive && AvailableLevelStalagtites.Num() != 0)
	{
		SetIdleFromAvailablePool();
		++CurrentPotential;
	}
	
	if(IdleStalagtites.Num() <= 0)
	{
		TimeOverflow = 0;
		return;
	}
	
	const int32 NumRolls = (DeltaTime + TimeOverflow) / (1 / WaveSetting.TickRate);
	TimeOverflow = FMath::Fmod(DeltaTime + TimeOverflow, 1 / WaveSetting.TickRate);

	for (int32 RollIndex = 0; RollIndex < NumRolls; ++RollIndex)
	{
		if(IdleStalagtites.Num() > 0)
		{
			//Do a random roll
			const float RandomRoll = FMath::FRand();
		
			if(RandomRoll < WaveSetting.TriggerChancePerTick)
			{
				const int32 RandIndex = FMath::RandRange(0, IdleStalagtites.Num() - 1);
				IdleStalagtites[RandIndex]->TriggerStalagtite();
				ActiveStalagtites.Add(IdleStalagtites[RandIndex]);
				IdleStalagtites.RemoveAtSwap(RandIndex, 1, false);
			}
		}
		else
		{
			break;
		}
	}
}

void AStalagmiteManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	AStalagmiteManager::Singleton = nullptr;
}

#if WITH_EDITOR
void AStalagmiteManager::GatherAllStalagtites()
{
	TArray<AActor*> OutStalagtites;
	UGameplayStatics::GetAllActorsOfClass(this, AMagicStalagtite::StaticClass(),OutStalagtites);

	AvailableLevelStalagtites.Empty(OutStalagtites.Num());
	for(AActor* Actor: OutStalagtites)
	{
		if(AMagicStalagtite* Stalagtite = Cast<AMagicStalagtite>(Actor))
		{
			AvailableLevelStalagtites.Add(Stalagtite);
		}		
	}
}
#endif

PRAGMA_DISABLE_OPTIMIZATION // #Sumo_DisableOptimisation
const FWaveSettings& AStalagmiteManager::GetCurrentWaveSettings() const
{
	if(const AMagicRoxGameModeBase* GameMode =  Cast<AMagicRoxGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		const float StabilityLevel = GameMode->GetStabilityPercent();

		for (const FWaveSettings& WaveSetting : WaveSettings)
		{
			if(StabilityLevel >= WaveSetting.StabilityLevel)
			{
				return WaveSetting;
			}
		}
	}

	return WaveSettings[0];
}

PRAGMA_ENABLE_OPTIMIZATION //Sumo_EnableOptimisation

void AStalagmiteManager::SetIdleFromAvailablePool()
{
	if(AvailableLevelStalagtites.Num() == 0)
	{
		return;
	}
	
	const int32 RandomIndex = FMath::RandRange(0, AvailableLevelStalagtites.Num() - 1 );
	IdleStalagtites.Add(AvailableLevelStalagtites[RandomIndex]);
	AvailableLevelStalagtites[RandomIndex]->SetState(State_Idle);
	AvailableLevelStalagtites.RemoveAtSwap(RandomIndex, 1, false);
}

PRAGMA_DISABLE_OPTIMIZATION // #Sumo_DisableOptimisation
AMagicRoxGameModeBase* AStalagmiteManager::GetCachedGameMode() const
{
	if(!CachedGameMode.IsValid())
	{
		CachedGameMode = AMagicRoxGameModeBase::Get(GetWorld());
	}

	return CachedGameMode.Get();
}

PRAGMA_ENABLE_OPTIMIZATION //Sumo_EnableOptimisation


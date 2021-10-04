// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/MagicStalagtite.h"
#include "StalagmiteManager.generated.h"

USTRUCT()
struct FWaveSettings
{
	GENERATED_BODY()

	//If stability level is greater or equal to the entry, these settings are chosen
	//Containing Array should be ordered from Low to high
	UPROPERTY(EditAnywhere)
	float StabilityLevel = 100.0f;
	UPROPERTY(EditAnywhere)
	int32 TargetNumActive = 3;
	UPROPERTY(EditAnywhere)
	int32 MaxNumFallen = 2;
	//What's the chance an active stalagtite gets triggered per tick
	UPROPERTY(EditAnywhere)
	float TriggerChancePerTick = 0.01f;
	UPROPERTY(EditAnywhere)
	float TickRate = 60.f;
};

UCLASS()
class MAGICROX_API AStalagmiteManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AStalagmiteManager();

	static AStalagmiteManager* Get(const UObject* WorldContextObejct);

	void MarkStalagtiteFallen(AMagicStalagtite* Fallen);
	void MarkStalagtiteAvailable(AMagicStalagtite* Available);
	
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void GatherAllStalagtites();
#endif
	
protected:

	UPROPERTY(EditAnywhere)
	UAudioComponent* TrembleComponent = nullptr;
	
	UPROPERTY(VisibleInstanceOnly)
	TArray<TSoftObjectPtr<AMagicStalagtite>> AvailableLevelStalagtites;

	UPROPERTY(Transient)
	TArray<TSoftObjectPtr<AMagicStalagtite>> IdleStalagtites;
	UPROPERTY(Transient)
	TArray<TSoftObjectPtr<AMagicStalagtite>> ActiveStalagtites;
	UPROPERTY(Transient)
	TArray<TSoftObjectPtr<AMagicStalagtite>> FallenStalagtites;
	
	UPROPERTY(EditAnywhere)
	TArray<FWaveSettings> WaveSettings;

private:

	float TimeOverflow = 0;
	
	const FWaveSettings& GetCurrentWaveSettings() const;

	void SetIdleFromAvailablePool();

	UPROPERTY(Transient)
	mutable TWeakObjectPtr<class AMagicRoxGameModeBase> CachedGameMode = nullptr;;
	AMagicRoxGameModeBase* GetCachedGameMode() const;

	static TWeakObjectPtr<AStalagmiteManager> Singleton;

	float PreviousStabilityLevel = 100.0f;
};

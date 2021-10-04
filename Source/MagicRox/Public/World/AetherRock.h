// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Actor.h"
#include "Interaction/DamageInterface.h"
#include "Objects/AetherOre.h"
#include "AetherRock.generated.h"

UCLASS()
class MAGICROX_API AAetherRock : public AActor, public IDamageInterface
{

	GENERATED_BODY()

public:	
	AAetherRock();
	virtual bool TakeDamage(AMagicCharacter* DamagingCharacter, float DamagePoints) override;
	
protected:
	
	virtual void BeginPlay() override;

////////////////////////////////////////////////////////////////////////////////
// Visuals
////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	UStaticMeshComponent* RockMeshComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	UMaterial* BaseMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	FName DepletedParameter = FName{"IsDepleted"};
	UPROPERTY(EditDefaultsOnly, Category="Visuals")
	FName HealthPercentParam = FName{"HealthPercent"};
	
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* RockMaterial;

////////////////////////////////////////////////////////////////////////////////
// Audio
////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	USoundBase* HitImpactSFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Audio")
	USoundBase* DestroySFX = nullptr;
	
////////////////////////////////////////////////////////////////////////////////
// Health
////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, Category="Health")
	float MaxHealth = 10.f;
	UPROPERTY()
	float CurrentHealth = MaxHealth;

////////////////////////////////////////////////////////////////////////////////
// Depletion
////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, Category="Depletion")
	float MinRespawnTime = 10.0f;
	UPROPERTY(EditAnywhere, Category="Depletion")
	float MaxRespawnTime = 25.0f;

	bool bIsDepleted = false;

	FTimerHandle RespawnTimerHandle;
	
	UPROPERTY(EditAnywhere, Category="Ore", meta = (MakeEditWidget))
	FTransform OreSpawnTransform = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, Category="Ore")
	TSubclassOf<AAetherOre> OreClassToSpawn = AAetherOre::StaticClass();
	UPROPERTY(EditAnywhere, Category="Ore")
	float ZSpawnImpulse = 50.0f;
	UPROPERTY(EditAnywhere, Category="Ore")
	float MaxLateralSpawnImpulse = 25.f;
	
	UFUNCTION()
	void RespawnRock();
};

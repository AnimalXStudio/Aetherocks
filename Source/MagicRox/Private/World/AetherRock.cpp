// Copyright AnimalXStudio 2021

#include "World/AetherRock.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

AAetherRock::AAetherRock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);

	USceneComponent* RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComp);
	
	RockMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RockMesh"));
	RockMeshComponent->SetupAttachment(RootSceneComp);
}

bool AAetherRock::TakeDamage(AMagicCharacter* DamagingCharacter, float DamagePoints)
{
	if(!bIsDepleted)
	{
		CurrentHealth -= DamagePoints;
		
		if(RockMaterial)
		{
			RockMaterial->SetScalarParameterValue(HealthPercentParam, CurrentHealth / MaxHealth);
		}
		
		if(CurrentHealth <= 0.f)
		{
			CurrentHealth = 0.f;
			bIsDepleted = true;
			UGameplayStatics::PlaySoundAtLocation(this, DestroySFX, GetActorLocation());
			if(RockMaterial)
			{
				RockMaterial->SetScalarParameterValue(DepletedParameter, 1);
			}
			
			if(UWorld* World = GetWorld())
			{
				if (OreClassToSpawn)
				{
					FActorSpawnParameters SpawnParameters;
					SpawnParameters.bNoFail = true;
					SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

					const FTransform SpawnTransform{GetActorTransform().TransformPosition(OreSpawnTransform.GetLocation())};
					if(AAetherOre* Ore = World->SpawnActor<AAetherOre>(OreClassToSpawn, SpawnTransform, SpawnParameters))
					{
						FVector ImpulseVector = SpawnTransform.GetLocation() - DamagingCharacter->GetActorLocation();
						ImpulseVector.Normalize();
						ImpulseVector *= MaxLateralSpawnImpulse;
						ImpulseVector.Z = ZSpawnImpulse;
						Ore->GetInteractionCollision()->AddImpulse(ImpulseVector, NAME_None, true);
					}
				}
				
				FTimerManager& TimerManager = World->GetTimerManager();
				TimerManager.SetTimer(RespawnTimerHandle, this, &ThisClass::RespawnRock, FMath::RandRange(MinRespawnTime, MaxRespawnTime), false);
			}
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitImpactSFX, GetActorLocation());
		}
		return true;
	}
	return false;
}

void AAetherRock::BeginPlay()
{
	Super::BeginPlay();
	
	if(BaseMaterial)
	{
		RockMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, BaseMaterial);
		RockMaterial->SetScalarParameterValue(DepletedParameter, 0);

		RockMeshComponent->SetMaterial(0, RockMaterial);
	}

	CurrentHealth = MaxHealth;
}

void AAetherRock::RespawnRock()
{
	CurrentHealth = MaxHealth;
	if(RockMaterial)
	{
		RockMaterial->SetScalarParameterValue(DepletedParameter, 0);
		RockMaterial->SetScalarParameterValue(HealthPercentParam, CurrentHealth / MaxHealth);
	}
	bIsDepleted = false;
}

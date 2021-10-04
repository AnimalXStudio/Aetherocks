// Copyright AnimalXStudio 2021

#include "World/MagicStabilityCrystal.h"

#include "Character/MagicCharacter.h"
#include "Components/AudioComponent.h"
#include "Core/MagicGameInstance.h"
#include "Core/MagicRoxGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/AetherOre.h"
#include "Particles/ParticleSystemComponent.h"

AMagicStabilityCrystal::AMagicStabilityCrystal(const FObjectInitializer& ObjectInitializer)\
	:Super(ObjectInitializer)
{
	m_EnergyCost = .0f;
	m_AllowInstantActiation = true;
	m_UsageCooldownTime = 1.0f;
	m_CanBePickedUp = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetActorTickEnabled(true);

	FireComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireSystem"));
	FireComponent->SetupAttachment(m_CollisionComponent);

	IdleFireComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("IdleFireSound"));
	IdleFireComponent->SetupAttachment(FireComponent);
}

void AMagicStabilityCrystal::BeginPlay()
{
	Super::BeginPlay();

	if(AMagicRoxGameModeBase* GameModeBase = AMagicRoxGameModeBase::Get(this))
	{
		GameModeBase->SetStabilityCrystal(this);
	}
}

void AMagicStabilityCrystal::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GameTime += DeltaSeconds;

	const float CurrentFuelDrainRate = FuelDrainRateCurve ? FuelDrainRateCurve->GetFloatValue(GameTime) : 0.0f;
	CurrentFuelLevel -= DeltaSeconds * CurrentFuelDrainRate;
	if(CurrentFuelLevel <= 0.0f)
	{
		CurrentFuelLevel = 0.0f;
		if(FireComponent && IdleFireComponent)
		{
			FireComponent->Deactivate();
			IdleFireComponent->Deactivate();
		}
	}
	else
	{
		if(FireComponent && IdleFireComponent)
		{
			const float FuelRatio = CurrentFuelLevel / 100.f;
			FireComponent->SetVectorParameter(FName{"ParticleVelocity"}, FVector{FMath::FRand(), FMath::FRand(), FuelRatio});
			FireComponent->SetFloatParameter(FName{"SpawnIntensity"}, FuelRatio);
			FireComponent->SetFloatParameter(FName{"SpawnSphere"}, FuelRatio);
			FireComponent->SetFloatParameter(FName{"OrbitSize"}, FuelRatio);
			
			IdleFireComponent->SetVolumeMultiplier(FMath::Lerp( .2f, 1.f, FuelRatio));
		}
	}

	
	const float CurrentStabilityDrainRate = StabilityDrainRateCurve ? StabilityDrainRateCurve->GetFloatValue(CurrentFuelLevel) : 0.0f;
	if(AMagicRoxGameModeBase* GameModeBase = AMagicRoxGameModeBase::Get(this))
	{
		GameModeBase->SetDrainRate(CurrentStabilityDrainRate);
	}	
}

bool AMagicStabilityCrystal::CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage)
{
		const bool SuperResult = Super::CanInteract(TargetingActor, bShowHUDMessage);
	if(!SuperResult)
	{
		return false;
	}
	
	if(CurrentFuelLevel >= 100.0f || !TargetingActor)
	{
		return false;
	}

	if(const UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
	{
		return GameInstance->NumCarriedAether > 0;
	} 

	return false;
}

void AMagicStabilityCrystal::OnInteractionStarted(AMagicCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);

	if(UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
	{
		CurrentFuelLevel += FMath::RandRange(MinFuelValue, MaxFuelLevel);
		--GameInstance->NumCarriedAether;
		UGameplayStatics::PlaySoundAtLocation(this, InteractionSFX, GetActorLocation());

		if(FireComponent && !FireComponent->IsActive() && IdleFireComponent)
		{
			FireComponent->Activate();
			IdleFireComponent->Activate();
		}
	}
}

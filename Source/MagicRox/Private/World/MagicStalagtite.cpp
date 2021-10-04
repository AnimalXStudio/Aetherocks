// Fill out your copyright notice in the Description page of Project Settings.


#include "World/MagicStalagtite.h"

#include "Character/MagicCharacter.h"
#include "Components/AudioComponent.h"
#include "Core/MagicGameInstance.h"
#include "Core/MagicRoxGameModeBase.h"
#include "Core/StalagmiteManager.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
AMagicStalagtite::AMagicStalagtite()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneRoot);
	
	Stalagtite = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stalagtite"));
	Stalagtite->SetupAttachment(SceneRoot);
	
	Rock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rock"));
	Rock->SetupAttachment(Stalagtite);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(Rock);
	
	IndicatorDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("IndicatorDecal"));
	IndicatorDecal->SetupAttachment(SceneRoot);
	IndicatorDecal->SetRelativeLocation(EndRelativeLocation);
	
	IndicatorDecal->SetVisibility(false);
	SetActorTickEnabled(false);
	Rock->SetHiddenInGame(true);
	Rock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Rock->SetRelativeLocation(FVector::ZeroVector);
	Stalagtite->SetHiddenInGame(true);
}

void AMagicStalagtite::TriggerStalagtite()
{
	if(GetCurrentState() == State_Idle)
	{
		SetState(State_Triggered);
	}
}

void AMagicStalagtite::ResetStalagtite()
{
	if(GetCurrentState() == State_Cleared)
	{
		SetState(State_Idle);
	}
}

bool AMagicStalagtite::TakeDamage(AMagicCharacter* DamagingCharacter, float DamagePoints)
{
	if(GetCurrentState() == State_Collapsed)
	{
		CurrentHealth -= DamagePoints;
		
		UGameplayStatics::PlaySoundAtLocation(this, HitImpactSFX, GetActorLocation());
		if(CurrentHealth <= 0.f)
		{
			SetState(State_Cleared);
		}
		return true;
	}
	return false;
}

#if WITH_EDITOR
void AMagicStalagtite::UpdateRelativeEndLocation()
{
	FHitResult TraceResult{};
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
		
	GetWorld()->LineTraceSingleByChannel(TraceResult, Stalagtite->GetComponentLocation(), Stalagtite->GetComponentLocation() + FVector{0.f, 0.f, -1.f} * 5000.0f, ECollisionChannel::ECC_Visibility, Params);
	if(TraceResult.bBlockingHit)
	{
		EndRelativeLocation = GetTransform().InverseTransformPosition(TraceResult.Location);
		IndicatorDecal->SetRelativeLocation(EndRelativeLocation);
	}
}
#endif

void AMagicStalagtite::SetState(EStalagtiteState NewState)
{
	if(NewState == CurrentState)
	{
		return;
	}
	
	if(CurrentState == State_Triggered)
	{
		Stalagtite->SetRelativeRotation(DefaultRelativeRotation);
	}

	AStalagmiteManager* Manager = AStalagmiteManager::Get(this);
	
	switch (NewState)
	{
	case State_Idle:
		Rock->SetHiddenInGame(true);
		Rock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Stalagtite->SetHiddenInGame(false);
		SetActorTickEnabled(false);
		IndicatorDecal->SetVisibility(false);
		break;
	case State_Triggered:
		SetActorTickEnabled(true);
		Rock->SetHiddenInGame(true);
		Rock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Stalagtite->SetHiddenInGame(false);
		IndicatorDecal->SetVisibility(true);
		OnRockTriggered();
		break;
	case State_Falling:
		SetActorTickEnabled(true);
		Rock->SetHiddenInGame(false);
		Rock->SetRelativeLocation(FVector::ZeroVector);
		Rock->SetCollisionProfileName(FName{"FallingStalagtite"});
		Rock->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Stalagtite->SetHiddenInGame(true);
		IndicatorDecal->SetVisibility(true);
		OnRockFalling();
		break;
	case State_Collapsed:
		CurrentHealth = MaxHealth;
		SetActorTickEnabled(false);
		Rock->SetHiddenInGame(false);
		Rock->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Stalagtite->SetHiddenInGame(true);
		IndicatorDecal->SetVisibility(false);
		Rock->SetCollisionProfileName(FName{"BlockAll"});
		BPOnRockHit();
		if(Manager)
		{
			Manager->MarkStalagtiteFallen(this);
		}
		break;
	case State_Cleared:		
		SetActorTickEnabled(false);
		Rock->SetHiddenInGame(true);
		Rock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Rock->SetRelativeLocation(FVector::ZeroVector);
		Stalagtite->SetHiddenInGame(true);
		IndicatorDecal->SetVisibility(false);
		if(Manager)
		{
			Manager->MarkStalagtiteAvailable(this);
		}
		
		break;
	default: ;
	}

	CurrentStateDuration = 0.f;
	CurrentState = NewState;
}

void AMagicStalagtite::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if(EndRelativeLocation == FVector::ZeroVector)
	{
		UpdateRelativeEndLocation();
	}
#endif
}

void AMagicStalagtite::BeginPlay()
{
	Super::BeginPlay();

	DefaultRelativeRotation = Stalagtite->GetRelativeRotation();
	Rock->OnComponentHit.AddDynamic(this, &ThisClass::OnRockHit);

	if(IndicatorDecal && IndicatorDecal->GetMaterial(0))
	{
		WarningDecalMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, IndicatorDecal->GetMaterial(0));
		WarningDecalMaterial->SetScalarParameterValue(FName{"WarningSize"}, 0.f);
		IndicatorDecal->SetMaterial(0, WarningDecalMaterial);
	}

}

void AMagicStalagtite::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentStateDuration += DeltaSeconds;
	
	switch (CurrentState)
	{
	case State_Triggered:
		{
			const float CurrentRotationOffset = FMath::Lerp(MinTriggeredRotationOffset, MaxTriggeredRotationOffset, CurrentStateDuration/TriggerDuration);
			const FRotator TargetRotation = DefaultRelativeRotation + FRotator{
				FMath::RandRange(-CurrentRotationOffset, CurrentRotationOffset),
				FMath::RandRange(-CurrentRotationOffset, CurrentRotationOffset),
				FMath::RandRange(-CurrentRotationOffset, CurrentRotationOffset)};
			Stalagtite->SetRelativeRotation(FMath::Lerp(Stalagtite->GetRelativeRotation(), TargetRotation, DeltaSeconds));

			if(WarningDecalMaterial)
			{
				WarningDecalMaterial->SetScalarParameterValue(FName{"WarningSize"}, CurrentStateDuration / TriggerDuration);
			}
			
			if(CurrentStateDuration >= TriggerDuration)
			{
				SetState(State_Falling);
			}

			break;
		}
	case State_Falling:
		{
			float CurrentAlpha = FallingCurve ? FallingCurve->GetFloatValue(CurrentStateDuration / FallDuration) : CurrentStateDuration / FallDuration;
			if(CurrentAlpha > 1.f)
			{
				CurrentAlpha = 1.f;
			}
			
			Rock->SetRelativeLocation(FMath::Lerp(FVector::ZeroVector, EndRelativeLocation, CurrentAlpha), true);
		}
		break;
	default: ;
	}
}

void AMagicStalagtite::OnRockHit(UPrimitiveComponent* HitComponent,
                                 AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(CurrentState != State_Falling)
	{
		return;
	}
	
	if(AMagicCharacter* Character = Cast<AMagicCharacter>(OtherActor))
	{
		SetState(State_Cleared);
		
		if(UMagicGameInstance* GameInstance = UMagicGameInstance::Get(this))
		{
			GameInstance->NumCarriedAether = 0;
		}

		if(AMagicRoxGameModeBase* GameMode = AMagicRoxGameModeBase::Get(this))
		{
			//Let the gamemode handle the reduction in stability itself
			GameMode->OnPlayerHitByRock();
		}

		if(PlayerHitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PlayerHitSound, Character->GetActorLocation());
		}
	}
	else
	{
		if(Hit.Normal.Z > 0.5f)
		{
			SetState(State_Collapsed);
		}
	}
}


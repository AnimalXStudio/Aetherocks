// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DamageInterface.h"
#include "MagicStalagtite.generated.h"

UENUM()
enum EStalagtiteState
{
	State_Idle,
	State_Triggered,
	State_Falling,
	State_Collapsed,
	State_Cleared
};

UCLASS()
class MAGICROX_API AMagicStalagtite : public AActor, public IDamageInterface
{
	
	GENERATED_BODY()

public:	

	AMagicStalagtite();

	void TriggerStalagtite();
	void ResetStalagtite();
	EStalagtiteState GetCurrentState() const { return CurrentState; }

	virtual bool TakeDamage(AMagicCharacter* DamagingCharacter, float DamagePoints) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BPOnRockHit();
	UFUNCTION(BlueprintImplementableEvent)
	void OnRockTriggered();
	UFUNCTION(BlueprintImplementableEvent)
	void OnRockFalling();
	
#if WITH_EDITOR

	UFUNCTION(CallInEditor)
	void UpdateRelativeEndLocation();
	
#endif
	
	void SetState(EStalagtiteState NewState);
	
protected:
	
	EStalagtiteState CurrentState = EStalagtiteState::State_Cleared;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
////////////////////////////////////////////////////////////////////////////////
// Components
////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category="Components")
	UStaticMeshComponent* Stalagtite = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UStaticMeshComponent* Rock = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UDecalComponent* IndicatorDecal = nullptr;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Components")
	UAudioComponent* AudioComponent = nullptr;
	
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget))
	FVector EndRelativeLocation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float MinTriggeredRotationOffset = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float MaxTriggeredRotationOffset = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float TriggerDuration = 3.5f;
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float FallDuration = .8f;
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	UCurveFloat* FallingCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	float MaxHealth = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category="Sound")
	USoundBase* HitImpactSFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Sound")
	USoundBase* PlayerHitSound = nullptr;
	
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* WarningDecalMaterial = nullptr;
	
	FRotator DefaultRelativeRotation = FRotator::ZeroRotator;

	float CurrentStateDuration = 0.0f;
	float CurrentHealth = MaxHealth;
	
////////////////////////////////////////////////////////////////////////////////
// Hit logic
////////////////////////////////////////////////////////////////////////////////

public:
	UFUNCTION()
	void OnRockHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
};

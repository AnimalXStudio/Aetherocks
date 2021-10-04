// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Tools/MagicToolBase.h"
#include "MagicPickaxe.generated.h"

/**
 * 
 */
UCLASS()
class MAGICROX_API AMagicPickaxe : public AMagicToolBase
{
	GENERATED_BODY()

	friend class AMagicAnvil;
	
public:

	AMagicPickaxe(const FObjectInitializer& ObjectInitializer);

	float GetCurrentDurabilityPercent() const {return m_CurrentDurabilty / m_MaxDurability * 100.0f;}

	UFUNCTION()
	void OnPickaxeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
protected:

	virtual void BeginPlay() override;
	
	virtual void OnAttack() override;

	virtual void OnBeginAttack() override;
	virtual void OnEndAttack() override;
	
	virtual bool CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage) override;

	float GetDamageForHit() const;
	
	UPROPERTY(EditDefaultsOnly, Category="Attack")
	UBoxComponent* m_PickaxeCollider = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "BaseDamage"))
	float m_MaxDamage = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "BaseDamage"))
	float m_MinDamage = 4.0f;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "BaseDamage"))
	UCurveFloat* DurabilityDamageCurve = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "DegradedDamage"))
	float m_DegradedDamage = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category="Durability", meta = (DisplayName = "MinDegradationPerHit"))
	float m_MinDegradationPerHit = 1.2f;
	UPROPERTY(EditDefaultsOnly, Category="Durability", meta = (DisplayName = "MaxDegradationPerHit"))
	float m_MaxDegradationPerHit = 2.5f;
	UPROPERTY(EditDefaultsOnly, Category="Durability", meta = (DisplayName = "Durability"))
	float m_MaxDurability = 100.0f;
	float m_CurrentDurabilty = m_MaxDurability;

	void Repair();
};

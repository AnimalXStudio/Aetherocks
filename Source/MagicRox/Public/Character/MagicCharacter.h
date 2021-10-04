// Copyright AnimalXStudio 2021
#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "MagicCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UMagicInteractionComponent;
class AMagicToolBase;

UCLASS()
class MAGICROX_API AMagicCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMagicCharacter(const FObjectInitializer& ObjectInitializer);

	FTransform GetPickUpAttachTransform() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Input functions
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	void Jump();

	//Character components
	UPROPERTY(EditDefaultsOnly, Category = "Core", DisplayName = "CameraBoom")
	USpringArmComponent* m_CameraBoom;
	UPROPERTY(EditDefaultsOnly, Category = "Core", DisplayName = "FirstPersonCamera")
	UCameraComponent* m_CameraComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Core|Sockets", DisplayName = "ToolsAttachmentComponent")
	USceneComponent* m_ToolsAttachment = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Interaction functionality
////////////////////////////////////////////////////////////////////////////////
protected:

	void Interact();

	//Interaction settings
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Settings", DisplayName = "InteractionRange")
	float m_InteractionRange = 500.0f;

	//Tracks which actor is currently targeted by the character
	UPROPERTY()
	AMagicToolBase* m_CurrentInteractionTarget = nullptr;
	UPROPERTY()
	UMagicInteractionComponent* m_CurrentTargetInteractionComponent = nullptr;
private:

	void UpdateInteraction();

	void UpdateHUDState();

	void ResetInteractionState();

////////////////////////////////////////////////////////////////////////////////
// Pickup functionality
////////////////////////////////////////////////////////////////////////////////

public:
	
	AMagicToolBase* GetCurrentPickUp() const { return m_CurrentCarriedObject.Get(); }
	bool CanPickupObject() const { return m_CurrentCarriedObject.Get() == nullptr; }
	bool PickupObject(AMagicToolBase* WeaponToPickUp);

	//Object input
	void Attack();
	void DropObject();
	void PickupTargetObject();

private:
	FRotator m_AttachRotationOffset = FRotator::ZeroRotator;
	TWeakObjectPtr<AMagicToolBase> m_CurrentCarriedObject;

////////////////////////////////////////////////////////////////////////////////
// Energy system
////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION(BlueprintCallable)
	bool HasEnoughEnergy(float EnergyCost) const { return m_CurrentEnergy >= EnergyCost; };
	UFUNCTION(BlueprintCallable)
	float GetRemainingEnergy() const { return m_CurrentEnergy; };

	float ReduceEnergy(float DeltaEnergy);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Energy", DisplayName = "CurrentEnergy")
	float m_CurrentEnergy = 100.0f;

////////////////////////////////////////////////////////////////////////////////
// Movement Control
////////////////////////////////////////////////////////////////////////////////

public:
	void BlockCharacterMovement();
	void BlockCharacterLook();

	void UnBlockCharacterMovement();
	void UnBlockCharacterLook();

private:

	bool m_IsMovementBlocked = false;
	bool m_IsLookBlocked = false;

////////////////////////////////////////////////////////////////////////////////
// Attack functionality
////////////////////////////////////////////////////////////////////////////////

public:

	//Called when a weapon hits an object to reverse the swing
	void OnAttackHitObject();
	
protected:

	UFUNCTION()
	void OnAttackTimelineUpdated(FVector CurrentTransform);
	UFUNCTION()
	void OnSwingStarted();
	UFUNCTION()
	void OnAttackPeak();
	UFUNCTION()
	void OnAttackFinished();
	
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "AttackTimeline"))
	UTimelineComponent* m_AttackTimeline = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "VectorCurve"))
	UCurveVector* m_AttackVectorCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "AttackCurveTrackName"))
	FName m_AttackCurveTrackName = FName{"AttackCurve"};

	UPROPERTY()
	FOnTimelineVector m_OnAttackTimelineUpdated;
	UPROPERTY()
	FOnTimelineEvent m_OnSwingStarted;
	UPROPERTY()
	FOnTimelineEvent m_OnAttackPeak;
	UPROPERTY()
	FOnTimelineEvent m_OnAttackFinished;
	
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "AttackDuration"))
	float m_AttackDuration = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "AttackPeak"))
	float m_AttackPeakTime = .4f;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta = (DisplayName = "AttackPeak"))
	float m_SwingHighPoint = .2f;

	bool m_HitSomething = false;
	FRotator HitRelativeRotation = FRotator::ZeroRotator;
};

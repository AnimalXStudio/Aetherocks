// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "MagicToolBase.generated.h"

class UMagicInteractionComponent;
class AMagicCharacter;

UCLASS()
class MAGICROX_API AMagicToolBase : public AActor
{
	GENERATED_BODY()

public:	
	
	AMagicToolBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	UMagicInteractionComponent* GetInteracationComponent() const { return m_InteractionComponent; }

	//Use this name to override the class of the root primitive component in child classes
	static FName RootComponentClassName;

	//@ param bPlaceOnFloor: if set to true, places the object on the floor if it shouldn't simulate physics
	virtual void DropTool(bool bPlaceOnFloor = true);

	bool GetCanBePickedUp() const { return m_CanBePickedUp; }
	void OnToolPickedUp();

	UBoxComponent* GetInteractionCollision() const {return m_CollisionComponent;}
	
	virtual bool CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage = false);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FString& GetToolDisplayName() const { return m_Displayname; };

	const FRotator& GetAttachRotationOffset() const { return m_AttachRotationOffset; };
	const FVector& GetAttachLocationOffset() const { return m_AttachLocationOffset; };
protected:
	
	UFUNCTION(BlueprintCallable)
	void SendOnInteractionFailedMessage(float Duration = 3.0f);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostLoad() override;

	//Interaction callbacks
	virtual void OnBecomeActiveTarget(AMagicCharacter* TargetingCharacter);
	virtual void OnExitActiveTarget(AMagicCharacter* TargetingCharacter);
	virtual void OnInteractionStarted(AMagicCharacter* TargetingCharacter);

	//Blueprint interface for tools, used for quick iteration
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveBecomeActiveTarget(AMagicCharacter* TargetingCharacter);
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveExitActiveTarget(AMagicCharacter* TargetingCharacter);
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveInteractionStarted(AMagicCharacter* TargetingCharacter);

////////////////////////////////////////////////////////////////////////////////
// Attack stuff
////////////////////////////////////////////////////////////////////////////////

public:
	
	virtual void OnAttack();

	virtual void OnBeginAttack() {};
	virtual void OnEndAttack() {};
////////////////////////////////////////////////////////////////////////////////
// Default components
////////////////////////////////////////////////////////////////////////////////

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Collision", DisplayName = "CollisionComponent")
	UBoxComponent* m_CollisionComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals", DisplayName = "ToolMesh", BlueprintReadOnly)
	UStaticMeshComponent* m_ToolMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", DisplayName = "InteractionComponent")
	UMagicInteractionComponent* m_InteractionComponent = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Pickup settings
////////////////////////////////////////////////////////////////////////////////

	//If this object can be picked up
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "CanBePickedUp")
	bool m_CanBePickedUp = false;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "ShouldSimulatePhysics")
	bool m_ShouldSimulatePhysics = true;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "DestroyAfterInteraction")
	bool m_ShouldDestroyAfterInteraction = false;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "UsageCooldownTimeSeconds")
	float m_UsageCooldownTime = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "AllowInstantActiation")
	bool m_AllowInstantActiation = false;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "DropFloorOffset")
	float m_DropFloorOffset = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "EnergyCost")
	float m_EnergyCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "DisplayName")
	FString m_Displayname = "BaseTool";
	UPROPERTY(EditAnywhere, Category = "Settings", DisplayName = "On Interaction Failed Message")
	FString m_OnInteractionFailed = "Can't use this right now";

	UPROPERTY(EditDefaultsOnly, Category = "Runtime", DisplayName = "ObjectFinished", BlueprintReadWrite)
	bool m_IsInteractionFinished = false;

	UPROPERTY(EditDefaultsOnly, Category= "Settings|Attach", meta = (DisplayName = "AttachRotationOffset"))
	FRotator m_AttachRotationOffset = FRotator::ZeroRotator;
	UPROPERTY(EditDefaultsOnly, Category= "Settings|Attach", meta = (DisplayName = "AttachRotationOffset"))
	FVector m_AttachLocationOffset = FVector::ZeroVector;


	UPROPERTY(EditDefaultsOnly, Category="Sound")
	USoundBase* InteractionSFX = nullptr;
	
////////////////////////////////////////////////////////////////////////////////
// Runtime tracking variables
////////////////////////////////////////////////////////////////////////////////

private:
	
	float GetWorldTimeSeconds() const;
	float m_LastUseTimeStamp = 0.0f;
	
};
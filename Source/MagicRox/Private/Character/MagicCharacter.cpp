// Copyright AnimalXStudio 2021

#include "Character/MagicCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/MagicMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interaction/MagicInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/MagicToolBase.h"
#include "UI/MagicHUD.h"

AMagicCharacter::AMagicCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMagicMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* CharacterCapsuleComponent = GetCapsuleComponent();

	//Create the camera boom component and attach it to the capsule
	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraBoom->SetupAttachment(CharacterCapsuleComponent);
	m_CameraBoom->SetRelativeLocation(FVector{ -40.0f, 0.0f , 60.0f });
	m_CameraBoom->TargetArmLength = 0.0f;
	m_CameraBoom->bUsePawnControlRotation = true;

	//Create the camera component and attach it to the capsule
	m_CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	m_CameraComponent->SetupAttachment(m_CameraBoom);

	//Create the scene component used to attach tools to you find in the world
	m_ToolsAttachment = CreateDefaultSubobject<USceneComponent>(TEXT("ToolAttachment"));
	m_ToolsAttachment->SetupAttachment(m_CameraComponent);
	m_ToolsAttachment->SetRelativeLocation(FVector{ 90.0f, 30.0f , -20.0f });

	m_AttackTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AttackTimeline"));
	m_AttackTimeline->SetTimelineLength(1.f);
	m_AttackTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	m_AttackTimeline->SetPlayRate( 1 / m_AttackDuration);

	//Bind timeline functions
	m_OnAttackTimelineUpdated.BindUFunction(this, TEXT("OnAttackTimelineUpdated"));
	m_OnAttackPeak.BindUFunction(this, TEXT("OnAttackPeak"));
	m_OnAttackFinished.BindUFunction(this, TEXT("OnAttackFinished"));
	m_OnSwingStarted.BindUFunction(this, TEXT("OnSwingStarted"));
	
	AutoReceiveInput = EAutoReceiveInput::Player0;
}

FTransform AMagicCharacter::GetPickUpAttachTransform() const
{
	if (m_ToolsAttachment)
	{
		return m_ToolsAttachment->GetComponentTransform();
	}

	//Return identity if the attachment component is not valid
	return FTransform{};
}

void AMagicCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(m_AttackTimeline)
	{
		m_AttackTimeline->AddInterpVector(m_AttackVectorCurve, m_OnAttackTimelineUpdated, m_AttackCurveTrackName, m_AttackCurveTrackName);
		m_AttackTimeline->AddEvent(m_AttackPeakTime, m_OnAttackPeak);
		m_AttackTimeline->AddEvent(m_SwingHighPoint, m_OnSwingStarted);
		m_AttackTimeline->SetTimelineFinishedFunc(m_OnAttackFinished);
	}
}

void AMagicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update the interaction settings based on current state
	UpdateInteraction();
}

void AMagicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent)
	{
		//Bind axis callbacks

		//Movement
		PlayerInputComponent->BindAxis(
			FName{ "MoveForward" }, this, &ThisClass::MoveForward
		);
		PlayerInputComponent->BindAxis(
			FName{ "MoveRight" }, this, &ThisClass::MoveRight
		);

		//Look
		PlayerInputComponent->BindAxis(
			FName{ "LookUp" }, this, &ThisClass::LookUp
		);
		PlayerInputComponent->BindAxis(
			FName{ "LookRight" }, this, &ThisClass::LookRight
		);

		//Bind action events

		//Movement
		PlayerInputComponent->BindAction(
			FName{ "Jump" }, EInputEvent::IE_Pressed, this, &ThisClass::Jump
		);

		//Interaction
		PlayerInputComponent->BindAction(
			FName{ "Interact" }, EInputEvent::IE_Pressed, this, &ThisClass::Interact
		);

		//Objects
		PlayerInputComponent->BindAction(
			FName{ "Attack" }, EInputEvent::IE_Pressed, this, &ThisClass::Attack
		);
		PlayerInputComponent->BindAction(
			FName{ "DropObject" }, EInputEvent::IE_Pressed, this, &ThisClass::DropObject
		);
		PlayerInputComponent->BindAction(
			FName{ "PickUp" }, EInputEvent::IE_Pressed, this, &ThisClass::PickupTargetObject
		);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Input functions
////////////////////////////////////////////////////////////////////////////////

void AMagicCharacter::MoveForward(float AxisValue)
{
	if (!m_IsMovementBlocked)
	{
		AddMovementInput(GetActorForwardVector(), AxisValue);
	}
}

void AMagicCharacter::MoveRight(float AxisValue)
{
	if (!m_IsMovementBlocked)
	{
		AddMovementInput(GetActorRightVector(), AxisValue);
	}
}

void AMagicCharacter::LookUp(float AxisValue)
{
	if (!m_IsLookBlocked)
	{
		AddControllerPitchInput(AxisValue);
	}
}

void AMagicCharacter::LookRight(float AxisValue)
{
	if (!m_IsLookBlocked)
	{
		AddControllerYawInput(AxisValue);
	}
}

void AMagicCharacter::Jump()
{
	if (!m_IsMovementBlocked)
	{
		bPressedJump = true;
	}
}

void AMagicCharacter::Interact()
{
	if (m_CurrentInteractionTarget && m_CurrentTargetInteractionComponent)
	{
		if (m_CurrentInteractionTarget->CanInteract(this, true))
		{
			m_CurrentTargetInteractionComponent->GetInteractionStartedDelegate().Broadcast(this);
		}
	}
}

void AMagicCharacter::UpdateInteraction()
{
	if (const UWorld* World = GetWorld())
	{
		const FVector TraceStart = m_CameraComponent->GetComponentLocation();
		const FVector TraceEnd = TraceStart + m_CameraComponent->GetForwardVector() * m_InteractionRange;

		FHitResult HitResult;
		if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel1))
		{
			if (AMagicToolBase* HitActor = Cast<AMagicToolBase>(HitResult.Actor.Get()))
			{
				if (m_CurrentInteractionTarget)
				{
					//If we hit a different actor compared to last frame, reset the state
					if (m_CurrentInteractionTarget != HitActor)
					{
						ResetInteractionState();
					}
					else
					{
						//Update the HUD in case the interaction state on the object changed
						UpdateHUDState();
						//We hit the same actor as last frame, all required delegates have been fired already
						return;
					}
				}

				if (UMagicInteractionComponent* InteractionComponent = HitActor->FindComponentByClass<UMagicInteractionComponent>())
				{
					if (InteractionComponent->GetInteractionRange() >= HitResult.Distance)
					{
						m_CurrentInteractionTarget = HitActor;
						m_CurrentTargetInteractionComponent = InteractionComponent;
						m_CurrentTargetInteractionComponent->GetOnBecomeActiveTargetDelegate().Broadcast(this);
					}
				}
			}
			else
			{
				ResetInteractionState();
			}
		}
		else
		{
			ResetInteractionState();
		}

		UpdateHUDState();
	}
}

void AMagicCharacter::UpdateHUDState()
{
	if (AMagicHUD* HUD = AMagicHUD::GetInstance(this))
	{
		if (m_CurrentInteractionTarget)
		{
			if (m_CurrentInteractionTarget->CanInteract(this))
			{
				HUD->OnInteractionStateChanged(EInteractState::Interact_Yes);
			}
			else
			{
				HUD->OnInteractionStateChanged(EInteractState::Interact_Invalid);
			}

			if (m_CurrentInteractionTarget->GetCanBePickedUp())
			{
				if (CanPickupObject())
				{
					HUD->OnPickUpStateChanged(EPickupState::PickUp_Yes);
				}
				else
				{
					HUD->OnPickUpStateChanged(EPickupState::PickUp_Invalid);
				}
			}
			else
			{
				HUD->OnPickUpStateChanged(EPickupState::PickUp_No);
			}
		}
		else
		{
			//No target actors so nothing to show here
			HUD->OnPickUpStateChanged(EPickupState::PickUp_No);
			HUD->OnInteractionStateChanged(EInteractState::Interact_No);
		}
	}
}

void AMagicCharacter::ResetInteractionState()
{
	if (m_CurrentTargetInteractionComponent)
	{
		//Reset the current target interaction data
		m_CurrentTargetInteractionComponent->GetOnExitActiveTargetDelegate().Broadcast(this);
		m_CurrentTargetInteractionComponent = nullptr;
		m_CurrentInteractionTarget = nullptr;
	}
}

bool AMagicCharacter::PickupObject(AMagicToolBase* WeaponToPickUp)
{
	//Validate data
	if (! WeaponToPickUp || GetCurrentPickUp() != nullptr)
	{
		return false;
	}

	WeaponToPickUp->SetOwner(this);
	
	//Save pointer to our current weapon
	m_CurrentCarriedObject = WeaponToPickUp;

	//Attach the weapon to our socket
	WeaponToPickUp->AttachToComponent(m_ToolsAttachment, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	WeaponToPickUp->SetActorRelativeLocation(WeaponToPickUp->GetAttachLocationOffset());
	
	m_AttachRotationOffset = WeaponToPickUp->GetAttachRotationOffset();
	m_ToolsAttachment->SetRelativeRotation(m_AttachRotationOffset);
	
	return true;
}

void AMagicCharacter::Attack()
{
	if (AMagicToolBase* CurrentPickup = GetCurrentPickUp())
	{
		CurrentPickup->AttachToComponent(m_ToolsAttachment, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		
		if(!m_AttackTimeline->IsPlaying() && m_CurrentCarriedObject.IsValid())
		{
			m_AttackTimeline->PlayFromStart();
		}
	}
}

void AMagicCharacter::DropObject()
{
	if (AMagicToolBase* CurrentPickup = GetCurrentPickUp())
	{
		CurrentPickup->SetOwner(nullptr);
		
		CurrentPickup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentPickup->DropTool(true);
		
		m_CurrentCarriedObject = nullptr;
	}
}

void AMagicCharacter::PickupTargetObject()
{
	if (AMagicToolBase* TargetTool = m_CurrentInteractionTarget)
	{
		//If this object can be picked up, pick it up
		if (TargetTool->GetCanBePickedUp() && CanPickupObject())
		{
			TargetTool->OnToolPickedUp();
			PickupObject(TargetTool);
		}
	}
}

float AMagicCharacter::ReduceEnergy(float DeltaEnergy)
{
	m_CurrentEnergy -= DeltaEnergy;
	if (m_CurrentEnergy < 0.0f)
	{
		m_CurrentEnergy = 0.0f;
	}

	return m_CurrentEnergy;
}


//At this point I had been going for 15 hours with 3 hours sleep, so yeah :D
void AMagicCharacter::BlockCharacterMovement()
{
	m_IsMovementBlocked = true;
}

void AMagicCharacter::BlockCharacterLook()
{
	m_IsLookBlocked = true;
}

void AMagicCharacter::UnBlockCharacterMovement()
{
	m_IsMovementBlocked = false;
}

void AMagicCharacter::UnBlockCharacterLook()
{
	m_IsLookBlocked = false;
}

void AMagicCharacter::OnAttackHitObject()
{
	if(!m_HitSomething)
	{
		if(AMagicToolBase* CurrentTool = m_CurrentCarriedObject.Get())
		{
			CurrentTool->OnAttack();
		}

		m_HitSomething = true;
		HitRelativeRotation = m_ToolsAttachment->GetRelativeRotation();
	}
}

void AMagicCharacter::OnAttackTimelineUpdated(FVector CurrentTransform)
{
	if(m_ToolsAttachment)
	{
		if (!m_HitSomething || m_AttackTimeline->GetPlaybackPosition() >= m_AttackPeakTime)
		{
			const FRotator TargetRotation = !m_HitSomething ? m_AttachRotationOffset + FRotator::MakeFromEuler(CurrentTransform)
				: FMath::Lerp(HitRelativeRotation, m_AttachRotationOffset, (m_AttackTimeline->GetPlaybackPosition() - m_AttackPeakTime) / (1.f - m_AttackPeakTime));
			
			m_ToolsAttachment->SetRelativeRotation(TargetRotation, true);
			if(AMagicToolBase* CurrentTool = m_CurrentCarriedObject.Get())
			{
				CurrentTool->UpdateOverlaps();
			}	
		}
	}
}

void AMagicCharacter::OnSwingStarted()
{
	if (AMagicToolBase* CurrentPickup = GetCurrentPickUp())
	{
		CurrentPickup->OnBeginAttack();
	}
}

void AMagicCharacter::OnAttackPeak()
{
	if(AMagicToolBase* CurrentTool = m_CurrentCarriedObject.Get())
	{
		if(!m_HitSomething)
		{
			CurrentTool->OnAttack();
		}

		CurrentTool->OnEndAttack();
	}
}

void AMagicCharacter::OnAttackFinished()
{
	m_HitSomething = false;
}

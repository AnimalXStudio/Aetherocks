// Copyright AnimalXStudio 2021

#include "Tools/MagicToolBase.h"
#include "Interaction/MagicInteractionComponent.h"
#include "Character/MagicCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"


FName AMagicToolBase::RootComponentClassName(TEXT("ToolCollision"));

AMagicToolBase::AMagicToolBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//m_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectRoot"));
	//SetRootComponent(m_RootComponent);
	
	//Default the collision component to a box, can be overridden in sub classes if required
	m_CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	m_CollisionComponent->SetCollisionProfileName(FName{ "Tool" });
	SetRootComponent(m_CollisionComponent);

	//#todo: Could make this optional so subclasses can choose to have no mesh (or the mesh is the root component etc)
	m_ToolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToolMesh"));
	if (m_ToolMesh)
	{
		m_ToolMesh->SetupAttachment(m_CollisionComponent);
		m_ToolMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	m_InteractionComponent = CreateDefaultSubobject<UMagicInteractionComponent>(TEXT("InteractionComponent"));
	if (m_InteractionComponent)
	{
		m_InteractionComponent->GetOnBecomeActiveTargetDelegate().AddUObject(this, &ThisClass::OnBecomeActiveTarget);
		m_InteractionComponent->GetOnExitActiveTargetDelegate().AddUObject(this, &ThisClass::OnExitActiveTarget);
		m_InteractionComponent->GetInteractionStartedDelegate().AddUObject(this, &ThisClass::OnInteractionStarted);
	}
}

void AMagicToolBase::DropTool(bool bPlaceOnFloor)
{
	if (m_CollisionComponent)
	{
		m_CollisionComponent->SetSimulatePhysics(m_ShouldSimulatePhysics);
		m_CollisionComponent->SetCollisionProfileName(FName{"Tool"});
	}

	
	if (!m_ShouldSimulatePhysics && bPlaceOnFloor)
	{
		if (UWorld* World = GetWorld())
		{
			FVector ActorForwardVector = GetActorForwardVector();
			FVector RayStart = GetActorLocation() + ActorForwardVector * 30.0f;
			FVector RayEnd = RayStart - FVector::UpVector * 250.0f;
			
			FCollisionObjectQueryParams QueryParams{FCollisionObjectQueryParams::InitType::AllStaticObjects};
			FHitResult HitResult;
			if (World->LineTraceSingleByObjectType(HitResult, RayStart, RayEnd, QueryParams))
			{
				SetActorLocationAndRotation(HitResult.Location + HitResult.Normal * m_DropFloorOffset, UKismetMathLibrary::MakeRotFromZX(HitResult.Normal, ActorForwardVector));
			}
		}
	}
}

void AMagicToolBase::OnToolPickedUp()
{
	if (m_CollisionComponent)
	{
		m_CollisionComponent->SetCollisionProfileName(FName{"CarriedTool"});
		m_CollisionComponent->SetSimulatePhysics(false);
	}
}

bool AMagicToolBase::CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage)
{
	if (!TargetingActor || m_IsInteractionFinished)
	{
		return false;
	}

	const float CurrentTimeStamp = GetWorldTimeSeconds();
	const bool bTimeConstraintMet = CurrentTimeStamp > (m_LastUseTimeStamp + m_UsageCooldownTime) || (m_AllowInstantActiation && m_LastUseTimeStamp <= 0.0f);
	const bool bEnergyConstraintMet = TargetingActor->HasEnoughEnergy(m_EnergyCost);
	return bTimeConstraintMet && bEnergyConstraintMet;
}

void AMagicToolBase::SendOnInteractionFailedMessage(float Duration)
{
}

void AMagicToolBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMagicToolBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (m_InteractionComponent)
	{
		//Clean up interaction callbacks on destruction
		m_InteractionComponent->GetOnBecomeActiveTargetDelegate().RemoveAll(this);
		m_InteractionComponent->GetOnExitActiveTargetDelegate().RemoveAll(this);
		m_InteractionComponent->GetInteractionStartedDelegate().RemoveAll(this);
	}
}

void AMagicToolBase::PostLoad()
{
	Super::PostLoad();
}

void AMagicToolBase::OnBecomeActiveTarget(AMagicCharacter* TargetingCharacter)
{
	ReceiveBecomeActiveTarget(TargetingCharacter);
}

void AMagicToolBase::OnExitActiveTarget(AMagicCharacter* TargetingCharacter)
{
	ReceiveExitActiveTarget(TargetingCharacter);
}

void AMagicToolBase::OnInteractionStarted(AMagicCharacter* TargetingCharacter)
{
	if (!TargetingCharacter)
	{
		return;
	}
	
	ReceiveInteractionStarted(TargetingCharacter);

	//Take the energy cost after interacting
	TargetingCharacter->ReduceEnergy(m_EnergyCost);

	//Auto destroy if required
	if (m_ShouldDestroyAfterInteraction)
	{
		Destroy();
	}
	else
	{
		m_LastUseTimeStamp = GetWorldTimeSeconds();
	}
}

void AMagicToolBase::OnAttack()
{
	//Base doesn't do anything
}

float AMagicToolBase::GetWorldTimeSeconds() const
{
	if (const UWorld* World = GetWorld())
	{
		return World->GetTimeSeconds();
	}
	return 0.0f;
}

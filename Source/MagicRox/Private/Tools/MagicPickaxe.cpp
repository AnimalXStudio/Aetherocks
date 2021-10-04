// Copyright AnimalXStudio 2021


#include "Tools/MagicPickaxe.h"

#include "Interaction/DamageInterface.h"

AMagicPickaxe::AMagicPickaxe(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	m_PickaxeCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("PickaxeCollider"));
	m_PickaxeCollider->SetupAttachment(m_ToolMesh);

	if(m_ToolMesh)
	{
		m_ToolMesh->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPickaxeBeginOverlap);
	}
}

void AMagicPickaxe::OnPickaxeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(AMagicCharacter* OwnerCharacter = Cast<AMagicCharacter>(GetOwner()))
	{
		OwnerCharacter->OnAttackHitObject();
	}
}

void AMagicPickaxe::BeginPlay()
{
	Super::BeginPlay();
	if(m_ToolMesh)
	{
		m_ToolMesh->SetGenerateOverlapEvents(false);
		m_ToolMesh->IgnoreActorWhenMoving(this, true);
	}
}

void AMagicPickaxe::OnAttack()
{
	Super::OnAttack();

	if(m_PickaxeCollider)
	{
		m_PickaxeCollider->UpdateOverlaps();
		
		TArray<AActor*> OverlappingActors;
		m_PickaxeCollider->GetOverlappingActors(OverlappingActors);

		bool HitValidActor = false;
		for(AActor* HitActor : OverlappingActors)
		{
			if(IDamageInterface* const DamageInterface = Cast<IDamageInterface>(HitActor))
			{
				HitValidActor |= DamageInterface->TakeDamage( Cast<AMagicCharacter>(GetOwner()), GetDamageForHit());
			}			
		}

		if(HitValidActor && m_CurrentDurabilty > 0.0f)
		{
			m_CurrentDurabilty -= FMath::RandRange(m_MinDegradationPerHit, m_MaxDegradationPerHit);
		}
	}
}

void AMagicPickaxe::OnBeginAttack()
{
	Super::OnBeginAttack();

	if(m_ToolMesh)
	{
		m_ToolMesh->SetGenerateOverlapEvents(true);
	}
}

void AMagicPickaxe::OnEndAttack()
{
	Super::OnEndAttack();
	
	if(m_ToolMesh)
	{
		m_ToolMesh->SetGenerateOverlapEvents(false);
	}
}

bool AMagicPickaxe::CanInteract(AMagicCharacter* TargetingActor, bool bShowHUDMessage)
{
	return false;
}

float AMagicPickaxe::GetDamageForHit() const
{
	if (m_CurrentDurabilty <= 0.0f)
	{
		return m_DegradedDamage;
	}

	float Alpha = m_CurrentDurabilty / m_MaxDurability;	
	if(DurabilityDamageCurve)
	{
		 Alpha = DurabilityDamageCurve->GetFloatValue(Alpha);
	}

	return FMath::Lerp(m_MinDamage, m_MaxDamage, Alpha);
}

void AMagicPickaxe::Repair()
{
	m_CurrentDurabilty = m_MaxDurability;
}

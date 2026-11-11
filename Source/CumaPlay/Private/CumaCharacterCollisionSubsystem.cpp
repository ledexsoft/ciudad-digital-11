#include "CumaCharacterCollisionSubsystem.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Stats/Stats.h"

bool UCumaCharacterCollisionSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UCumaCharacterCollisionSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	bCharacterConfigured = ConfigurePlayerCharacter();
}

void UCumaCharacterCollisionSubsystem::Tick(float DeltaTime)
{
	RetryTimeRemaining -= DeltaTime;
	if (RetryTimeRemaining > 0.0f)
	{
		return;
	}

	RetryTimeRemaining = 0.1f;
	bCharacterConfigured = ConfigurePlayerCharacter();
}

TStatId UCumaCharacterCollisionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCumaCharacterCollisionSubsystem, STATGROUP_Tickables);
}

bool UCumaCharacterCollisionSubsystem::ConfigurePlayerCharacter()
{
	UWorld* World = GetWorld();
	ACharacter* Character = World ? UGameplayStatics::GetPlayerCharacter(World, 0) : nullptr;
	if (!Character)
	{
		return false;
	}

	UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	if (!Capsule || !Movement)
	{
		return false;
	}

	// CharacterMovement uses the capsule as the physical body; the rendered feet
	// are visual only. Ensure that body blocks the Landscape and floor checks do
	// not let it settle below uneven terrain or balance on tiny edges.
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Capsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	Movement->MaxStepHeight = 45.0f;
	Movement->PerchRadiusThreshold = 10.0f;
	Movement->PerchAdditionalHeight = 0.0f;
	Movement->SetWalkableFloorAngle(50.0f);
	Movement->bUseFlatBaseForFloorChecks = true;
	Movement->bAlwaysCheckFloor = true;
	Movement->bCanWalkOffLedges = true;

	return true;
}

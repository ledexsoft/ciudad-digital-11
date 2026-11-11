#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CumaCharacterCollisionSubsystem.generated.h"

/** Applies terrain-friendly floor and capsule settings to the template player character. */
UCLASS()
class CUMAPLAY_API UCumaCharacterCollisionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return !bCharacterConfigured; }

private:
	bool ConfigurePlayerCharacter();

	float RetryTimeRemaining = 0.0f;
	bool bCharacterConfigured = false;
};

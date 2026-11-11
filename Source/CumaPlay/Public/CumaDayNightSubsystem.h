#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SunPosition.h"
#include "CumaDayNightSubsystem.generated.h"

class ADirectionalLight;
class USkyLightComponent;

/**
 * Drives the sun over Cumanayagua using the device's current civil clock and
 * the Epic Sun Position Calculator plugin. It is created automatically for
 * gameplay worlds; no placed Blueprint actor is required.
 */
UCLASS()
class CUMAPLAY_API UCumaDayNightSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	/** Local wall-clock time from the device, including its configured time zone. */
	UFUNCTION(BlueprintPure, Category="Cumanayagua|Day Night")
	FDateTime GetDeviceLocalDateTime() const { return DeviceLocalDateTime; }

	/** Geometric solar elevation at Cumanayagua, in degrees above the horizon. */
	UFUNCTION(BlueprintPure, Category="Cumanayagua|Day Night")
	float GetSunElevationDegrees() const { return SolarElevationDegrees; }

	/** Solar azimuth clockwise from geographic north, in degrees. */
	UFUNCTION(BlueprintPure, Category="Cumanayagua|Day Night")
	float GetSunAzimuthDegrees() const { return SolarAzimuthDegrees; }

	/** Local device time of sunrise, in decimal hours from midnight. */
	UFUNCTION(BlueprintPure, Category="Cumanayagua|Day Night")
	float GetSunriseLocalHours() const { return SunriseLocalHours; }

	/** Local device time of sunset, in decimal hours from midnight. */
	UFUNCTION(BlueprintPure, Category="Cumanayagua|Day Night")
	float GetSunsetLocalHours() const { return SunsetLocalHours; }

	UFUNCTION(BlueprintPure, Category="Cumanayagua|Day Night")
	bool IsSunAboveHorizon() const { return SolarElevationDegrees > -0.833f; }

private:
	void UpdateFromDeviceClock();
	void ResolveLightingActors();
	void ApplySolarLighting();

	TWeakObjectPtr<ADirectionalLight> SunActor;
	TWeakObjectPtr<USkyLightComponent> SkyLight;
	FDateTime DeviceLocalDateTime;
	FSunPositionData SunPositionData;
	double TimeZoneOffsetHours = 0.0;
	float SolarElevationDegrees = 0.0f;
	float SolarAzimuthDegrees = 0.0f;
	float SunriseLocalHours = 6.0f;
	float SunsetLocalHours = 18.0f;
	float CachedSkyLightIntensity = 1.0f;
	float TickAccumulator = 0.0f;
	bool bHasCachedSkyLightIntensity = false;
};

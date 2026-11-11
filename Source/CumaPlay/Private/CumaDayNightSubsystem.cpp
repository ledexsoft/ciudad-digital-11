#include "CumaDayNightSubsystem.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponentBase.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "EngineUtils.h"
#include "Stats/Stats.h"
#include "SunPosition.h"

namespace CumaDayNight
{
	constexpr float LatitudeDegrees = 22.14938f;
	constexpr float LongitudeDegrees = -80.20212f;
	constexpr float UpdateIntervalSeconds = 1.0f;
	constexpr float FullSunIlluminanceLux = 100000.0f;
	constexpr float TwilightStartDegrees = -6.0f;
	constexpr float SunriseFullDaylightDegrees = 2.0f;
	const FName SunActorTag(TEXT("CumaSun"));
}

bool UCumaDayNightSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UCumaDayNightSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	ResolveLightingActors();
	UpdateFromDeviceClock();
}

void UCumaDayNightSubsystem::Deinitialize()
{
	SunActor.Reset();
	SkyLight.Reset();
	Super::Deinitialize();
}

void UCumaDayNightSubsystem::Tick(float DeltaTime)
{
	TickAccumulator += DeltaTime;
	if (TickAccumulator < CumaDayNight::UpdateIntervalSeconds)
	{
		return;
	}

	TickAccumulator = FMath::Fmod(TickAccumulator, CumaDayNight::UpdateIntervalSeconds);
	if (!SunActor.IsValid() || !SunActor->GetLightComponent())
	{
		ResolveLightingActors();
	}
	UpdateFromDeviceClock();
}

TStatId UCumaDayNightSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCumaDayNightSubsystem, STATGROUP_Tickables);
}

void UCumaDayNightSubsystem::UpdateFromDeviceClock()
{
	// The device's UTC clock is the unambiguous instant used for the astronomical
	// calculation. Read the local clock too: its difference from UTC includes
	// the operating system's current time-zone and DST rules, and is used to
	// express sunrise/sunset in the device's local civil time.
	DeviceLocalDateTime = FDateTime::Now();
	const FDateTime UtcDateTime = FDateTime::UtcNow();
	TimeZoneOffsetHours = (DeviceLocalDateTime - UtcDateTime).GetTotalHours();

	USunPositionFunctionLibrary::GetSunPosition(
		CumaDayNight::LatitudeDegrees,
		CumaDayNight::LongitudeDegrees,
		0.0f,
		false,
		UtcDateTime.GetYear(),
		UtcDateTime.GetMonth(),
		UtcDateTime.GetDay(),
		UtcDateTime.GetHour(),
		UtcDateTime.GetMinute(),
		UtcDateTime.GetSecond(),
		SunPositionData);

	SolarElevationDegrees = SunPositionData.Elevation - 180.0f;
	SolarAzimuthDegrees = SunPositionData.Azimuth;
	const auto ToLocalHour = [this](double UtcHour)
	{
		double LocalHour = FMath::Fmod(UtcHour + TimeZoneOffsetHours, 24.0);
		if (LocalHour < 0.0)
		{
			LocalHour += 24.0;
		}
		return static_cast<float>(LocalHour);
	};
	SunriseLocalHours = ToLocalHour(SunPositionData.SunriseTime.GetTotalHours());
	SunsetLocalHours = ToLocalHour(SunPositionData.SunsetTime.GetTotalHours());
	ApplySolarLighting();
}

void UCumaDayNightSubsystem::ResolveLightingActors()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Keep the feature self-contained when the level has only its basic lights.
	TActorIterator<ASkyAtmosphere> SkyAtmosphereIt(World);
	const bool bHasSkyAtmosphere = static_cast<bool>(SkyAtmosphereIt);
	if (!bHasSkyAtmosphere)
	{
		World->SpawnActor<ASkyAtmosphere>();
	}

	ADirectionalLight* FirstDirectionalLight = nullptr;
	ADirectionalLight* AtmosphereSunLight = nullptr;
	ADirectionalLight* TaggedSun = nullptr;

	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		ADirectionalLight* Candidate = *It;
		UDirectionalLightComponent* Component = Cast<UDirectionalLightComponent>(Candidate->GetLightComponent());
		if (!Component)
		{
			continue;
		}

		if (!FirstDirectionalLight)
		{
			FirstDirectionalLight = Candidate;
		}
		if (Candidate->ActorHasTag(CumaDayNight::SunActorTag))
		{
			TaggedSun = Candidate;
			break;
		}
		if (!AtmosphereSunLight && Component->IsUsedAsAtmosphereSunLight())
		{
			AtmosphereSunLight = Candidate;
		}
	}

	SunActor = TaggedSun ? TaggedSun : (AtmosphereSunLight ? AtmosphereSunLight : FirstDirectionalLight);
	if (!SunActor.IsValid())
	{
		SunActor = World->SpawnActor<ADirectionalLight>();
	}

	UDirectionalLightComponent* SunComponent = Cast<UDirectionalLightComponent>(SunActor->GetLightComponent());
	if (SunComponent)
	{
		// The sun must be movable for its real position to advance while playing.
		SunComponent->SetMobility(EComponentMobility::Movable);
		SunComponent->SetAtmosphereSunLight(true);
		SunComponent->SetAtmosphereSunLightIndex(0);
	}

	bool bFoundSkyLight = false;
	for (TActorIterator<ASkyLight> It(World); It; ++It)
	{
		USkyLightComponent* Candidate = It->GetLightComponent();
		if (Candidate)
		{
			SkyLight = Candidate;
			CachedSkyLightIntensity = Candidate->Intensity;
			bHasCachedSkyLightIntensity = true;
			Candidate->SetMobility(EComponentMobility::Movable);
			bFoundSkyLight = true;
			break;
		}
	}
	if (!bFoundSkyLight)
	{
		if (ASkyLight* SpawnedSkyLight = World->SpawnActor<ASkyLight>())
		{
			SkyLight = SpawnedSkyLight->GetLightComponent();
			if (USkyLightComponent* Component = SkyLight.Get())
			{
				Component->SetMobility(EComponentMobility::Movable);
				CachedSkyLightIntensity = Component->Intensity;
				bHasCachedSkyLightIntensity = true;
			}
		}
	}
}

void UCumaDayNightSubsystem::ApplySolarLighting()
{
	if (!SunActor.IsValid())
	{
		return;
	}

	UDirectionalLightComponent* SunComponent = Cast<UDirectionalLightComponent>(SunActor->GetLightComponent());
	if (!SunComponent)
	{
		return;
	}

	// The plugin reports compass azimuth clockwise from north. In this project,
	// +X is east and -Y is north, so convert compass bearing to Unreal yaw.
	const float UnrealYawDegrees = SolarAzimuthDegrees - 90.0f;
	SunActor->SetActorRotation(FRotator(SunPositionData.Elevation, UnrealYawDegrees, 0.0f));

	const float ElevationFade = FMath::SmoothStep(
		CumaDayNight::TwilightStartDegrees,
		CumaDayNight::SunriseFullDaylightDegrees,
		SolarElevationDegrees);
	const float AboveHorizon = FMath::Max(0.0f, FMath::Sin(FMath::DegreesToRadians(SolarElevationDegrees)));
	SunComponent->SetIntensity(CumaDayNight::FullSunIlluminanceLux * AboveHorizon * ElevationFade);

	// Warmer direct light near the horizon; the Sky Atmosphere adds the visible
	// scattering and twilight colors when the level contains that component.
	const float TemperatureBlend = FMath::SmoothStep(0.0f, 18.0f, SolarElevationDegrees);
	const float ColorTemperatureKelvin = FMath::Lerp(2200.0f, 6500.0f, TemperatureBlend);
	if (!SunComponent->bUseTemperature)
	{
		SunComponent->SetUseTemperature(true);
	}
	SunComponent->SetTemperature(ColorTemperatureKelvin);

	if (!SkyLight.IsValid())
	{
		return;
	}

	USkyLightComponent* SkyComponent = SkyLight.Get();
	if (!bHasCachedSkyLightIntensity)
	{
		CachedSkyLightIntensity = SkyComponent->Intensity;
		bHasCachedSkyLightIntensity = true;
	}

	const float AmbientBlend = FMath::SmoothStep(-18.0f, 8.0f, SolarElevationDegrees);
	SkyComponent->SetIntensity(CachedSkyLightIntensity * FMath::Lerp(0.025f, 1.0f, AmbientBlend));

#if PLATFORM_ANDROID || PLATFORM_IOS
	// Keep mobile rendering inexpensive; the sky light still fades and changes
	// color, but avoids per-frame real-time scene capture.
	if (SkyComponent->bRealTimeCapture)
	{
		SkyComponent->SetRealTimeCapture(false);
	}
	const FLinearColor NightTint(0.18f, 0.24f, 0.42f, 1.0f);
	const FLinearColor DawnTint(1.0f, 0.52f, 0.30f, 1.0f);
	const FLinearColor DayTint(1.0f, 1.0f, 1.0f, 1.0f);
	const float TwilightBlend = FMath::SmoothStep(-12.0f, 0.0f, SolarElevationDegrees);
	const float DayBlend = FMath::SmoothStep(-1.0f, 10.0f, SolarElevationDegrees);
	SkyComponent->SetLightColor(FLinearColor::LerpUsingHSV(NightTint, DawnTint, TwilightBlend) * (1.0f - DayBlend) + DayTint * DayBlend);
#else
	// Desktop capture follows SkyAtmosphere and clouds as the sun moves.
	if (!SkyComponent->bRealTimeCapture)
	{
		SkyComponent->SetRealTimeCapture(true);
	}
	SkyComponent->SetLightColor(FLinearColor::White);
#endif
}

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CineSRTStreamSettings.generated.h"

UENUM(BlueprintType)
enum class ESRTStreamQuality : uint8
{
    Preview_480p    UMETA(DisplayName = "Preview (480p 15fps)"),
    HD_720p         UMETA(DisplayName = "HD Ready (720p 30fps)"),
    HD_1080p        UMETA(DisplayName = "Full HD (1080p 30fps)"),
    HD_1080p60      UMETA(DisplayName = "Full HD (1080p 60fps)"),
    UHD_4K          UMETA(DisplayName = "4K UHD (2160p 30fps)"),
    Custom          UMETA(DisplayName = "Custom")
};

UENUM(BlueprintType)
enum class ESRTEncoderType : uint8
{
    Auto            UMETA(DisplayName = "Auto Select"),
    NVENC           UMETA(DisplayName = "NVIDIA NVENC"),
    AMF             UMETA(DisplayName = "AMD AMF"),
    QuickSync       UMETA(DisplayName = "Intel QuickSync"),
    Software        UMETA(DisplayName = "Software (x264)")
};

UCLASS(config = CineSRTStream, defaultconfig, meta = (DisplayName = "Cine SRT Stream"))
class CINESRTSTREAM_API UCineSRTStreamSettings : public UDeveloperSettings
{
    GENERATED_BODY()
    
public:
    UCineSRTStreamSettings();
    
    /** Default streaming settings */
    UPROPERTY(config, EditAnywhere, Category = "Default Settings")
    bool bAutoStartStreaming = true;
    
    UPROPERTY(config, EditAnywhere, Category = "Default Settings")
    ESRTStreamQuality DefaultQuality = ESRTStreamQuality::HD_1080p;
    
    UPROPERTY(config, EditAnywhere, Category = "Default Settings", meta = (ClampMin = 1000, ClampMax = 50000))
    int32 DefaultBitrate = 8000;
    
    /** Encoder settings */
    UPROPERTY(config, EditAnywhere, Category = "Encoder")
    ESRTEncoderType PreferredEncoder = ESRTEncoderType::Auto;
    
    UPROPERTY(config, EditAnywhere, Category = "Encoder")
    bool bUseHardwareAcceleration = true;
    
    UPROPERTY(config, EditAnywhere, Category = "Encoder", meta = (ClampMin = 1, ClampMax = 10))
    int32 EncoderThreadCount = 4;
    
    /** Network settings */
    UPROPERTY(config, EditAnywhere, Category = "Network")
    FString DefaultBindAddress = TEXT("0.0.0.0");
    
    UPROPERTY(config, EditAnywhere, Category = "Network", meta = (ClampMin = 1, ClampMax = 10))
    int32 ReconnectAttempts = 3;
    
    UPROPERTY(config, EditAnywhere, Category = "Network", meta = (ClampMin = 1000, ClampMax = 30000))
    int32 ReconnectDelayMs = 5000;
    
    /** Performance settings */
    UPROPERTY(config, EditAnywhere, Category = "Performance")
    bool bUseAsyncCapture = true;
    
    UPROPERTY(config, EditAnywhere, Category = "Performance", meta = (ClampMin = 1, ClampMax = 10))
    int32 CaptureBufferCount = 3;
    
    virtual FName GetCategoryName() const override { return TEXT("Streaming"); }
}; 
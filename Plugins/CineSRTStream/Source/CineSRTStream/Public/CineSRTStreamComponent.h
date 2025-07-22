// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CineSRTStreamSettings.h"
#include "CineSRTStreamComponent.generated.h"

// Forward declarations
class UCameraComponent;
class USceneCaptureComponent2D;
class FSRTEncoder;
class FSRTTransmitter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreamingStateChanged, bool, bIsStreaming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreamingError, const FString&, ErrorMessage);

UCLASS(ClassGroup=(Streaming), meta=(BlueprintSpawnableComponent, DisplayName="SRT Stream"))
class CINESRTSTREAM_API USRTStreamComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USRTStreamComponent();

    // Editor properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SRT Settings", meta=(ClampMin=1024, ClampMax=65535))
    int32 StreamPort = 9001;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SRT Settings")
    FString StreamID = TEXT("Camera1");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SRT Settings")
    bool bAutoStartStream = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
    ESRTStreamQuality StreamQuality = ESRTStreamQuality::HD_1080p;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality", 
        meta=(EditCondition="StreamQuality==ESRTStreamQuality::Custom", EditConditionHides))
    FIntPoint CustomResolution = FIntPoint(1920, 1080);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality", meta=(ClampMin=1, ClampMax=120))
    int32 TargetFPS = 30;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality", meta=(ClampMin=500, ClampMax=50000))
    int32 Bitrate = 8000; // Kbps
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
    ESRTEncoderType EncoderType = ESRTEncoderType::Auto;
    
    // Runtime control
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    void StartStreaming();
    
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    void StopStreaming();
    
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    bool IsStreaming() const { return bIsStreaming; }
    
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    void SetStreamQuality(ESRTStreamQuality NewQuality);
    
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    void SetDynamicBitrate(int32 NewBitrate);
    
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    FString GetStreamURL() const;
    
    // Events
    UPROPERTY(BlueprintAssignable, Category = "SRT Stream")
    FOnStreamingStateChanged OnStreamingStateChanged;
    
    UPROPERTY(BlueprintAssignable, Category = "SRT Stream")
    FOnStreamingError OnStreamingError;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // Camera references
    UPROPERTY()
    UCameraComponent* CachedCameraComponent;
    
    UPROPERTY()
    USceneCaptureComponent2D* SceneCaptureComponent;
    
    UPROPERTY()
    UTextureRenderTarget2D* RenderTarget;
    
    // SRT components
    TSharedPtr<FSRTEncoder> Encoder;
    TSharedPtr<FSRTTransmitter> Transmitter;
    
    // State
    bool bIsStreaming = false;
    float TimeSinceLastCapture = 0.0f;
    float CaptureInterval = 0.033f; // 30 FPS default
    
    // Internal methods
    void FindCameraComponent();
    void CreateRenderTarget();
    void InitializeEncoder();
    void InitializeTransmitter();
    void CaptureFrame();
    void UpdateCaptureInterval();
    FIntPoint GetTargetResolution() const;
    bool GetFrameDataFromRenderTarget(TArray<uint8>& OutFrameData);
    
    // Callbacks
    void OnStreamingErrorInternal(const FString& Error);
}; 
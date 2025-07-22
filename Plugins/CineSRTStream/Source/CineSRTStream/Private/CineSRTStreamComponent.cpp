// Copyright Epic Games, Inc. All Rights Reserved.

#include "CineSRTStreamComponent.h"
#include "CineSRTStream.h"
#include "CineSRTStreamSettings.h"
#include "SRTEncoder.h"
#include "SRTTransmitter.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "RenderingThread.h"
#include "RHI.h"
#include "HAL/RunnableThread.h"

USRTStreamComponent::USRTStreamComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    
    // Load default settings
    const UCineSRTStreamSettings* Settings = GetDefault<UCineSRTStreamSettings>();
    if (Settings)
    {
        bAutoStartStream = Settings->bAutoStartStreaming;
        StreamQuality = Settings->DefaultQuality;
        Bitrate = Settings->DefaultBitrate;
        EncoderType = Settings->PreferredEncoder;
    }
}

void USRTStreamComponent::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogCineSRT, Log, TEXT("SRT Stream Component BeginPlay - StreamID: %s, Port: %d"), 
        *StreamID, StreamPort);
    
    // Find camera
    FindCameraComponent();
    
    if (!CachedCameraComponent)
    {
        UE_LOG(LogCineSRT, Warning, TEXT("No camera component found on actor %s"), 
            *GetOwner()->GetName());
        OnStreamingErrorInternal(TEXT("No camera component found"));
        return;
    }
    
    // Create render target
    CreateRenderTarget();
    
    // Initialize encoder and transmitter
    InitializeEncoder();
    InitializeTransmitter();
    
    // Auto start if enabled
    if (bAutoStartStream)
    {
        StartStreaming();
    }
}

void USRTStreamComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopStreaming();
    
    // Cleanup
    if (SceneCaptureComponent)
    {
        SceneCaptureComponent->DestroyComponent();
        SceneCaptureComponent = nullptr;
    }
    
    // Cleanup encoder and transmitter
    Encoder.Reset();
    Transmitter.Reset();
    
    Super::EndPlay(EndPlayReason);
}

void USRTStreamComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    static int TickCount = 0;
    if (TickCount++ % 30 == 0)
    {
        UE_LOG(LogCineSRT, Warning, TEXT("Tick running, streaming: %s"), bIsStreaming ? TEXT("YES") : TEXT("NO"));
    }
    
    if (!bIsStreaming || !SceneCaptureComponent)
    {
        return;
    }
    
    TimeSinceLastCapture += DeltaTime;
    
    if (TimeSinceLastCapture >= CaptureInterval)
    {
        CaptureFrame();
        TimeSinceLastCapture -= CaptureInterval;
    }
}

void USRTStreamComponent::FindCameraComponent()
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }
    
    // Try to find CineCamera first
    CachedCameraComponent = Owner->FindComponentByClass<UCineCameraComponent>();
    
    // Fallback to regular camera
    if (!CachedCameraComponent)
    {
        CachedCameraComponent = Owner->FindComponentByClass<UCameraComponent>();
    }
}

void USRTStreamComponent::CreateRenderTarget()
{
    if (!CachedCameraComponent)
    {
        return;
    }
    
    FIntPoint Resolution = GetTargetResolution();
    
    // Create render target
    RenderTarget = NewObject<UTextureRenderTarget2D>(this);
    RenderTarget->InitAutoFormat(Resolution.X, Resolution.Y);
    RenderTarget->UpdateResourceImmediate(true);
    
    // Create scene capture component
    SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(GetOwner());
    SceneCaptureComponent->SetupAttachment(CachedCameraComponent);
    SceneCaptureComponent->RegisterComponent();
    
    // Configure capture settings
    SceneCaptureComponent->TextureTarget = RenderTarget;
    SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    SceneCaptureComponent->bCaptureEveryFrame = false;
    SceneCaptureComponent->bCaptureOnMovement = false;
    SceneCaptureComponent->bAlwaysPersistRenderingState = true;
    
    // Copy camera settings
    SceneCaptureComponent->FOVAngle = CachedCameraComponent->FieldOfView;
    SceneCaptureComponent->bUseCustomProjectionMatrix = false;
}

void USRTStreamComponent::InitializeEncoder()
{
    FSRTEncoder::FEncoderSettings EncoderSettings;
    EncoderSettings.Width = GetTargetResolution().X;
    EncoderSettings.Height = GetTargetResolution().Y;
    EncoderSettings.FPS = TargetFPS;
    EncoderSettings.Bitrate = Bitrate;
    EncoderSettings.EncoderType = EncoderType;
    
    Encoder = MakeShared<FSRTEncoder>(EncoderSettings);
    
    if (!Encoder->Initialize())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to initialize SRT encoder"));
        OnStreamingErrorInternal(TEXT("Failed to initialize encoder"));
    }
}

void USRTStreamComponent::InitializeTransmitter()
{
    FSRTTransmitter::FTransmitterSettings TransmitterSettings;
    TransmitterSettings.BindAddress = TEXT("0.0.0.0");
    TransmitterSettings.Port = StreamPort;
    TransmitterSettings.LatencyTolerance = 120; // 120ms
    TransmitterSettings.MaxBW = 0; // Unlimited
    TransmitterSettings.InputBW = 0; // Unlimited
    TransmitterSettings.Overhead = 25; // 25%
    
    Transmitter = MakeShared<FSRTTransmitter>(TransmitterSettings);
    
    // Bind delegates
    Transmitter->OnFrameTransmitted.BindLambda([this](const TArray<uint8>& FrameData)
    {
        // Frame transmitted successfully
        static int32 TransmittedFrames = 0;
        TransmittedFrames++;
        
        if (TransmittedFrames % 30 == 0)
        {
            UE_LOG(LogCineSRT, Verbose, TEXT("Transmitted %d frames"), TransmittedFrames);
        }
    });
    
    Transmitter->OnError.BindLambda([this](const FString& Error)
    {
        OnStreamingErrorInternal(Error);
    });
}

void USRTStreamComponent::StartStreaming()
{
    if (bIsStreaming)
    {
        UE_LOG(LogCineSRT, Warning, TEXT("Streaming already active"));
        return;
    }
    
    if (!SceneCaptureComponent)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Scene capture component not initialized"));
        OnStreamingErrorInternal(TEXT("Scene capture component not initialized"));
        return;
    }
    
    if (!Encoder || !Encoder->IsInitialized())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Encoder not initialized"));
        OnStreamingErrorInternal(TEXT("Encoder not initialized"));
        return;
    }
    
    if (!Transmitter)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Transmitter not initialized"));
        OnStreamingErrorInternal(TEXT("Transmitter not initialized"));
        return;
    }
    
    // Start transmitter
    if (!Transmitter->StartTransmission())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to start SRT transmission"));
        OnStreamingErrorInternal(TEXT("Failed to start transmission"));
        return;
    }
    
    // Update capture interval
    UpdateCaptureInterval();
    
    // Enable tick
    PrimaryComponentTick.bStartWithTickEnabled = true;
    
    bIsStreaming = true;
    
    UE_LOG(LogCineSRT, Log, TEXT("Started SRT streaming on port %d"), StreamPort);
    
    // Broadcast event
    OnStreamingStateChanged.Broadcast(true);
}

void USRTStreamComponent::StopStreaming()
{
    if (!bIsStreaming)
    {
        return;
    }
    
    // Stop transmitter
    if (Transmitter)
    {
        Transmitter->StopTransmission();
    }
    
    // Disable tick
    PrimaryComponentTick.bStartWithTickEnabled = false;
    
    bIsStreaming = false;
    
    UE_LOG(LogCineSRT, Log, TEXT("Stopped SRT streaming"));
    
    // Broadcast event
    OnStreamingStateChanged.Broadcast(false);
}

void USRTStreamComponent::SetStreamQuality(ESRTStreamQuality NewQuality)
{
    if (StreamQuality == NewQuality)
    {
        return;
    }
    
    StreamQuality = NewQuality;
    
    // Update resolution if streaming
    if (bIsStreaming && RenderTarget)
    {
        FIntPoint NewResolution = GetTargetResolution();
        RenderTarget->InitAutoFormat(NewResolution.X, NewResolution.Y);
        RenderTarget->UpdateResourceImmediate(true);
    }
    
    // Update encoder settings
    if (Encoder)
    {
        FSRTEncoder::FEncoderSettings NewSettings;
        NewSettings.Width = GetTargetResolution().X;
        NewSettings.Height = GetTargetResolution().Y;
        NewSettings.FPS = TargetFPS;
        NewSettings.Bitrate = Bitrate;
        NewSettings.EncoderType = EncoderType;
        
        Encoder->UpdateSettings(NewSettings);
    }
    
    // Update capture interval
    UpdateCaptureInterval();
    
    UE_LOG(LogCineSRT, Log, TEXT("Stream quality changed to %d"), (int32)NewQuality);
}

void USRTStreamComponent::SetDynamicBitrate(int32 NewBitrate)
{
    if (Bitrate == NewBitrate)
    {
        return;
    }
    
    Bitrate = FMath::Clamp(NewBitrate, 500, 50000);
    
    // Update encoder settings
    if (Encoder)
    {
        FSRTEncoder::FEncoderSettings NewSettings;
        NewSettings.Width = GetTargetResolution().X;
        NewSettings.Height = GetTargetResolution().Y;
        NewSettings.FPS = TargetFPS;
        NewSettings.Bitrate = Bitrate;
        NewSettings.EncoderType = EncoderType;
        
        Encoder->UpdateSettings(NewSettings);
    }
    
    UE_LOG(LogCineSRT, Log, TEXT("Bitrate changed to %d Kbps"), Bitrate);
}

FString USRTStreamComponent::GetStreamURL() const
{
    return FString::Printf(TEXT("srt://localhost:%d"), StreamPort);
}

void USRTStreamComponent::CaptureFrame()
{
    UE_LOG(LogCineSRT, Warning, TEXT("=== CaptureFrame called ==="));
    if (!SceneCaptureComponent || !RenderTarget)
    {
        UE_LOG(LogCineSRT, Error, TEXT("No capture components!"));
        return;
    }
    
    // Capture the frame
    SceneCaptureComponent->CaptureScene();
    
    // Get frame data from render target
    TArray<uint8> FrameData;
    if (GetFrameDataFromRenderTarget(FrameData))
    {
        UE_LOG(LogCineSRT, Warning, TEXT("Frame captured: %d bytes"), FrameData.Num());
        // Encode frame
        if (Encoder && Encoder->SubmitFrame(FrameData))
        {
            UE_LOG(LogCineSRT, Warning, TEXT("Frame submitted to encoder"));
            TArray<uint8> EncodedData;
            if (Encoder->GetEncodedFrame(EncodedData))
            {
                UE_LOG(LogCineSRT, Warning, TEXT("Encoded data: %d bytes"), EncodedData.Num());
                // Transmit encoded frame
                if (Transmitter)
                {
                    Transmitter->TransmitFrame(EncodedData);
                    UE_LOG(LogCineSRT, Warning, TEXT("Frame transmitted"));
                }
            }
        }
    }
}

bool USRTStreamComponent::GetFrameDataFromRenderTarget(TArray<uint8>& OutFrameData)
{
    if (!RenderTarget || !RenderTarget->GetRenderTargetResource())
    {
        return false;
    }
    
    // Get render target resource
    FTextureRenderTargetResource* RTResource = RenderTarget->GetRenderTargetResource();
    if (!RTResource)
    {
        return false;
    }
    
    // Read pixels from render target
    FIntPoint Size = RenderTarget->SizeX;
    TArray<FColor> Pixels;
    Pixels.SetNum(Size.X * Size.Y);
    
    FReadSurfaceDataFlags ReadFlags;
    ReadFlags.SetLinearToGamma(false);
    
    if (RTResource->ReadPixels(Pixels, ReadFlags))
    {
        // Convert to raw data
        OutFrameData.SetNum(Pixels.Num() * 4); // RGBA
        FMemory::Memcpy(OutFrameData.GetData(), Pixels.GetData(), OutFrameData.Num());
        return true;
    }
    
    return false;
}

void USRTStreamComponent::UpdateCaptureInterval()
{
    CaptureInterval = 1.0f / TargetFPS;
}

FIntPoint USRTStreamComponent::GetTargetResolution() const
{
    switch (StreamQuality)
    {
        case ESRTStreamQuality::Preview_480p:
            return FIntPoint(854, 480);
        case ESRTStreamQuality::HD_720p:
            return FIntPoint(1280, 720);
        case ESRTStreamQuality::HD_1080p:
        case ESRTStreamQuality::HD_1080p60:
            return FIntPoint(1920, 1080);
        case ESRTStreamQuality::UHD_4K:
            return FIntPoint(3840, 2160);
        case ESRTStreamQuality::Custom:
            return CustomResolution;
        default:
            return FIntPoint(1920, 1080);
    }
}

void USRTStreamComponent::OnStreamingErrorInternal(const FString& Error)
{
    UE_LOG(LogCineSRT, Error, TEXT("Streaming error: %s"), *Error);
    OnStreamingError.Broadcast(Error);
} 
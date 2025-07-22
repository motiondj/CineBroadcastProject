// Copyright Epic Games, Inc. All Rights Reserved.

#include "SRTEncoder.h"
#include "CineSRTStream.h"
#include "HAL/PlatformProcess.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"

FSRTEncoder::FSRTEncoder(const FEncoderSettings& InSettings)
    : Settings(InSettings)
{
    FrameEvent = FPlatformProcess::GetSynchEventFromPool();
}

FSRTEncoder::~FSRTEncoder()
{
    Shutdown();
    FPlatformProcess::ReturnSynchEventToPool(FrameEvent);
}

bool FSRTEncoder::Initialize()
{
    if (bIsInitialized)
        return true;
    ApplyQualitySettings();
    Encoder = CreateEncoder();
    if (!Encoder || !Encoder->Initialize())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to create encoder"));
        return false;
    }
    Thread = FRunnableThread::Create(this, TEXT("SRTEncoderThread"));
    if (!Thread)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to create encoder thread"));
        return false;
    }
    bIsInitialized = true;
    UE_LOG(LogCineSRT, Log, TEXT("SRT Encoder initialized:"));
    UE_LOG(LogCineSRT, Log, TEXT("  - Resolution: %dx%d"), Settings.Width, Settings.Height);
    UE_LOG(LogCineSRT, Log, TEXT("  - FPS: %d"), Settings.FPS);
    UE_LOG(LogCineSRT, Log, TEXT("  - Bitrate: %d Kbps"), Settings.Bitrate);
    UE_LOG(LogCineSRT, Log, TEXT("  - Format: %d"), (int32)Settings.Format);
    return true;
}

void FSRTEncoder::Shutdown()
{
    if (!bIsInitialized) return;
    bShouldStop = true;
    FrameEvent->Trigger();
    if (Thread)
    {
        Thread->WaitForCompletion();
        delete Thread;
        Thread = nullptr;
    }
    if (Encoder)
    {
        Encoder->Shutdown();
        Encoder.Reset();
    }
    bIsInitialized = false;
}

bool FSRTEncoder::SubmitFrame(const TArray<uint8>& FrameData)
{
    if (!bIsInitialized) return false;
    FScopeLock Lock(&QueueMutex);
    if (InputQueueSize > 5)
    {
        TArray<uint8> DroppedFrame;
        InputQueue.Dequeue(DroppedFrame);
        InputQueueSize--;
        Stats.FramesDropped++;
        UE_LOG(LogCineSRT, Warning, TEXT("Encoder queue full, dropping frame"));
    }
    InputQueue.Enqueue(FrameData);
    InputQueueSize++;
    FrameEvent->Trigger();
    return true;
}

bool FSRTEncoder::GetEncodedFrame(TArray<uint8>& OutEncodedData)
{
    FScopeLock Lock(&QueueMutex);
    return OutputQueue.Dequeue(OutEncodedData);
}

void FSRTEncoder::UpdateSettings(const FEncoderSettings& NewSettings)
{
    bool bNeedsRestart = (Settings.Width != NewSettings.Width ||
                         Settings.Height != NewSettings.Height ||
                         Settings.Format != NewSettings.Format);
    Settings = NewSettings;
    ApplyQualitySettings();
    if (bNeedsRestart && bIsInitialized)
    {
        UE_LOG(LogCineSRT, Log, TEXT("Encoder settings changed, restarting..."));
        Shutdown();
        Initialize();
    }
}

bool FSRTEncoder::Init() { return true; }

uint32 FSRTEncoder::Run()
{
    while (!bShouldStop)
    {
        FrameEvent->Wait();
        TArray<uint8> InputFrame;
        while (InputQueue.Dequeue(InputFrame))
        {
            InputQueueSize--;
            if (bShouldStop) break;
            double StartTime = FPlatformTime::Seconds();
            TArray<uint8> EncodedFrame;
            if (Encoder && Encoder->EncodeFrame(InputFrame, EncodedFrame))
            {
                FScopeLock Lock(&QueueMutex);
                OutputQueue.Enqueue(MoveTemp(EncodedFrame));
                Stats.FramesEncoded++;
                Stats.TotalBytesEncoded += EncodedFrame.Num();
                double EncodeTime = FPlatformTime::Seconds() - StartTime;
                Stats.AverageEncodeTime = (Stats.AverageEncodeTime * (Stats.FramesEncoded - 1) + EncodeTime) / Stats.FramesEncoded;
            }
        }
    }
    return 0;
}

void FSRTEncoder::Stop() { bShouldStop = true; }

TUniquePtr<IVideoEncoder> FSRTEncoder::CreateEncoder()
{
    switch (Settings.Format)
    {
        case EEncodingFormat::MJPEG:
            return MakeUnique<FMJPEGEncoder>(Settings);
        case EEncodingFormat::H264:
            UE_LOG(LogCineSRT, Warning, TEXT("H264 encoder not implemented, falling back to MJPEG"));
            Settings.Format = EEncodingFormat::MJPEG;
            return MakeUnique<FMJPEGEncoder>(Settings);
        default:
            return nullptr;
    }
}

void FSRTEncoder::ApplyQualitySettings()
{
    switch (Settings.StreamQuality)
    {
        case ESRTStreamQuality::Preview_480p:
            Settings.Width = 854; Settings.Height = 480; Settings.FPS = 15; Settings.Bitrate = 1500; Settings.JpegQuality = 70; break;
        case ESRTStreamQuality::HD_720p:
            Settings.Width = 1280; Settings.Height = 720; Settings.FPS = 30; Settings.Bitrate = 4000; Settings.JpegQuality = 80; break;
        case ESRTStreamQuality::HD_1080p:
            Settings.Width = 1920; Settings.Height = 1080; Settings.FPS = 30; Settings.Bitrate = 8000; Settings.JpegQuality = 85; break;
        case ESRTStreamQuality::HD_1080p60:
            Settings.Width = 1920; Settings.Height = 1080; Settings.FPS = 60; Settings.Bitrate = 12000; Settings.JpegQuality = 85; break;
        case ESRTStreamQuality::UHD_4K:
            Settings.Width = 3840; Settings.Height = 2160; Settings.FPS = 30; Settings.Bitrate = 25000; Settings.JpegQuality = 90; break;
        case ESRTStreamQuality::Custom:
            break;
    }
}

// --- MJPEG 인코더 구현 ---
FMJPEGEncoder::FMJPEGEncoder(const FSRTEncoder::FEncoderSettings& InSettings)
    : Settings(InSettings)
{
}

bool FMJPEGEncoder::Initialize()
{
    return true;
}

bool FMJPEGEncoder::EncodeFrame(const TArray<uint8>& RawData, TArray<uint8>& OutEncodedData)
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
    if (!ImageWrapper.IsValid()) return false;
    if (!ImageWrapper->SetRaw(RawData.GetData(), RawData.Num(), Settings.Width, Settings.Height, ERGBFormat::BGRA, 8)) return false;
    OutEncodedData = ImageWrapper->GetCompressed(Settings.JpegQuality);
    return OutEncodedData.Num() > 0;
}

void FMJPEGEncoder::Shutdown() {}

// --- H264 인코더 스텁 ---
FH264Encoder::FH264Encoder(const FSRTEncoder::FEncoderSettings& InSettings) : Settings(InSettings) {}
bool FH264Encoder::Initialize() { return false; }
bool FH264Encoder::EncodeFrame(const TArray<uint8>&, TArray<uint8>&) { return false; }
void FH264Encoder::Shutdown() {} 
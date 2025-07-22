// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CineSRTStreamSettings.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

UENUM()
enum class EEncodingFormat : uint8
{
    None,
    MJPEG,
    H264,
    H265
};

// 인코더 인터페이스
class IVideoEncoder
{
public:
    virtual ~IVideoEncoder() = default;
    virtual bool Initialize() = 0;
    virtual bool EncodeFrame(const TArray<uint8>& RawData, TArray<uint8>& OutEncodedData) = 0;
    virtual void Shutdown() = 0;
    virtual EEncodingFormat GetFormat() const = 0;
};

class CINESRTSTREAM_API FSRTEncoder : public FRunnable
{
public:
    struct FEncoderSettings
    {
        int32 Width = 1920;
        int32 Height = 1080;
        int32 FPS = 30;
        int32 Bitrate = 8000;
        ESRTStreamQuality StreamQuality = ESRTStreamQuality::HD_1080p;
        ESRTEncoderType EncoderType = ESRTEncoderType::Auto;
        EEncodingFormat Format = EEncodingFormat::MJPEG;
        int32 JpegQuality = 85;
        int32 KeyframeInterval = 60;
        FString Preset = TEXT("fast");
    };

    FSRTEncoder(const FEncoderSettings& InSettings);
    virtual ~FSRTEncoder();

    bool Initialize();
    void Shutdown();
    bool SubmitFrame(const TArray<uint8>& FrameData);
    bool GetEncodedFrame(TArray<uint8>& OutEncodedData);
    void UpdateSettings(const FEncoderSettings& NewSettings);
    bool IsInitialized() const { return bIsInitialized; }

    struct FEncoderStats
    {
        int32 FramesEncoded = 0;
        int32 FramesDropped = 0;
        float AverageEncodeTime = 0.0f;
        int64 TotalBytesEncoded = 0;
    };
    FEncoderStats GetStats() const { return Stats; }

    // FRunnable
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

private:
    FEncoderSettings Settings;
    TUniquePtr<IVideoEncoder> Encoder;
    bool bIsInitialized = false;
    FRunnableThread* Thread = nullptr;
    FThreadSafeBool bShouldStop = false;
    TQueue<TArray<uint8>> InputQueue;
    TQueue<TArray<uint8>> OutputQueue;
    FCriticalSection QueueMutex;
    FEvent* FrameEvent = nullptr;
    FEncoderStats Stats;
    int32 InputQueueSize = 0;
    TUniquePtr<IVideoEncoder> CreateEncoder();
    void ApplyQualitySettings();
};

class FMJPEGEncoder : public IVideoEncoder
{
public:
    FMJPEGEncoder(const FSRTEncoder::FEncoderSettings& Settings);
    virtual bool Initialize() override;
    virtual bool EncodeFrame(const TArray<uint8>& RawData, TArray<uint8>& OutEncodedData) override;
    virtual void Shutdown() override;
    virtual EEncodingFormat GetFormat() const override { return EEncodingFormat::MJPEG; }
private:
    FSRTEncoder::FEncoderSettings Settings;
};

class FH264Encoder : public IVideoEncoder
{
public:
    FH264Encoder(const FSRTEncoder::FEncoderSettings& Settings);
    virtual bool Initialize() override;
    virtual bool EncodeFrame(const TArray<uint8>& RawData, TArray<uint8>& OutEncodedData) override;
    virtual void Shutdown() override;
    virtual EEncodingFormat GetFormat() const override { return EEncodingFormat::H264; }
private:
    FSRTEncoder::FEncoderSettings Settings;
    void* EncoderHandle = nullptr;
}; 
// Copyright Epic Games, Inc. All Rights Reserved.

#include "SRTEncoder.h"
#include "CineSRTStream.h"

FSRTEncoder::FSRTEncoder(const FEncoderSettings& InSettings)
    : Settings(InSettings)
{
}

FSRTEncoder::~FSRTEncoder()
{
    bIsInitialized = false;
}

bool FSRTEncoder::Initialize()
{
    UE_LOG(LogCineSRT, Log, TEXT("Initializing SRT Encoder - %dx%d, %d FPS, %d Kbps"), 
        Settings.Width, Settings.Height, Settings.FPS, Settings.Bitrate);
    
    // 현재는 소프트웨어 인코더만 구현
    // 하드웨어 인코더는 나중에 NVENC, AMF 등으로 확장 가능
    bool bSuccess = InitializeSoftwareEncoder();
    
    if (bSuccess)
    {
        bIsInitialized = true;
        UE_LOG(LogCineSRT, Log, TEXT("SRT Encoder initialized successfully"));
    }
    else
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to initialize SRT Encoder"));
    }
    
    return bSuccess;
}

bool FSRTEncoder::EncodeFrame(const TArray<uint8>& FrameData)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogCineSRT, Warning, TEXT("Encoder not initialized"));
        return false;
    }
    
    // 현재는 간단한 패스스루 구현
    // 실제로는 H.264/H.265 인코딩이 필요
    return EncodeFrameSoftware(FrameData);
}

bool FSRTEncoder::GetEncodedData(TArray<uint8>& OutEncodedData)
{
    if (EncodedBuffer.Num() > 0)
    {
        OutEncodedData = EncodedBuffer;
        EncodedBuffer.Empty();
        return true;
    }
    
    return false;
}

void FSRTEncoder::UpdateSettings(const FEncoderSettings& NewSettings)
{
    Settings = NewSettings;
    
    // 재초기화가 필요한 경우
    if (bIsInitialized)
    {
        bIsInitialized = false;
        Initialize();
    }
}

bool FSRTEncoder::InitializeSoftwareEncoder()
{
    // 현재는 간단한 구현
    // 실제로는 x264, x265 등의 소프트웨어 인코더 연동 필요
    UE_LOG(LogCineSRT, Log, TEXT("Initializing software encoder"));
    return true;
}

bool FSRTEncoder::InitializeHardwareEncoder()
{
    // 하드웨어 인코더 초기화 (NVENC, AMF, QuickSync)
    UE_LOG(LogCineSRT, Log, TEXT("Hardware encoder not implemented yet"));
    return false;
}

bool FSRTEncoder::EncodeFrameSoftware(const TArray<uint8>& FrameData)
{
    // 현재는 간단한 패스스루
    // 실제로는 H.264/H.265 인코딩 구현 필요
    EncodedBuffer = FrameData;
    
    static int32 FrameCount = 0;
    FrameCount++;
    
    if (FrameCount % 30 == 0) // 30프레임마다 로그
    {
        UE_LOG(LogCineSRT, Verbose, TEXT("Software encoded frame %d, size: %d bytes"), 
            FrameCount, FrameData.Num());
    }
    
    return true;
}

bool FSRTEncoder::EncodeFrameHardware(const TArray<uint8>& FrameData)
{
    // 하드웨어 인코딩 구현 (나중에 추가)
    UE_LOG(LogCineSRT, Log, TEXT("Hardware encoding not implemented yet"));
    return false;
} 
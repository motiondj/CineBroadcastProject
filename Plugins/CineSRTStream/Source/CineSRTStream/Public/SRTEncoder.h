// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CineSRTStreamSettings.h"

// SRT 헤더 포함
#if WITH_SRT
#include "srt/srt.h"
#endif

class FSRTEncoder
{
public:
    struct FEncoderSettings
    {
        int32 Width = 1920;
        int32 Height = 1080;
        int32 FPS = 30;
        int32 Bitrate = 8000; // Kbps
        ESRTEncoderType EncoderType = ESRTEncoderType::Auto;
    };

    FSRTEncoder(const FEncoderSettings& InSettings);
    ~FSRTEncoder();

    // 인코더 초기화
    bool Initialize();
    
    // 프레임 인코딩
    bool EncodeFrame(const TArray<uint8>& FrameData);
    
    // 인코딩된 데이터 가져오기
    bool GetEncodedData(TArray<uint8>& OutEncodedData);
    
    // 인코더 상태 확인
    bool IsInitialized() const { return bIsInitialized; }
    
    // 설정 업데이트
    void UpdateSettings(const FEncoderSettings& NewSettings);

private:
    FEncoderSettings Settings;
    bool bIsInitialized = false;
    
    // 인코딩된 데이터 버퍼
    TArray<uint8> EncodedBuffer;
    
    // 인코더 초기화 내부 함수
    bool InitializeSoftwareEncoder();
    bool InitializeHardwareEncoder();
    
    // 프레임 인코딩 내부 함수
    bool EncodeFrameSoftware(const TArray<uint8>& FrameData);
    bool EncodeFrameHardware(const TArray<uint8>& FrameData);
}; 
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"

// SRT 헤더 포함
#if WITH_SRT
#include "srt/srt.h"
#endif

// 전송 완료 델리게이트
DECLARE_DELEGATE_OneParam(FOnFrameTransmitted, const TArray<uint8>&);

// 에러 델리게이트
DECLARE_DELEGATE_OneParam(FOnTransmissionError, const FString&);

class FSRTTransmitter : public FRunnable
{
public:
    struct FTransmitterSettings
    {
        FString BindAddress = TEXT("0.0.0.0");
        int32 Port = 9001;
        int32 LatencyTolerance = 120; // ms
        int32 MaxBW = 0; // 0 = unlimited
        int32 InputBW = 0; // 0 = unlimited
        int32 Overhead = 25; // %
    };

    FSRTTransmitter(const FTransmitterSettings& InSettings);
    ~FSRTTransmitter();

    // FRunnable 인터페이스
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

    // 전송 시작/중지
    bool StartTransmission();
    void StopTransmission();
    
    // 프레임 전송
    bool TransmitFrame(const TArray<uint8>& FrameData);
    
    // 전송 상태 확인
    bool IsTransmitting() const { return bIsTransmitting; }
    
    // 설정 업데이트
    void UpdateSettings(const FTransmitterSettings& NewSettings);
    
    // 델리게이트
    FOnFrameTransmitted OnFrameTransmitted;
    FOnTransmissionError OnError;

private:
    static bool bSRTInitialized;
    static FCriticalSection SRTInitLock;
    FTransmitterSettings Settings;
    bool bIsTransmitting = false;
    FThreadSafeBool bShouldStop = false;
    
    // SRT 소켓
#if WITH_SRT
    SRTSOCKET ServerSocket = SRT_INVALID_SOCK;
    SRTSOCKET ClientSocket = SRT_INVALID_SOCK;
#endif
    
    // 전송 큐
    TArray<TArray<uint8>> TransmissionQueue;
    FCriticalSection QueueCriticalSection;
    
    // 스레드 관리
    FRunnableThread* Thread = nullptr;
    
    // SRT 초기화
    bool InitializeSRT();
    
    // 소켓 설정
    bool ConfigureSocket();
    
    // 클라이언트 연결 대기
    bool WaitForClient();
    
    // 프레임 전송 내부 함수
    bool SendFrameData(const TArray<uint8>& FrameData);
    
    // SRT 정리
    void CleanupSRT();
}; 
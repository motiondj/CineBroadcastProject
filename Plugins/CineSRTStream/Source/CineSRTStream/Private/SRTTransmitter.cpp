// Copyright Epic Games, Inc. All Rights Reserved.

#include "SRTTransmitter.h"
#include "CineSRTStream.h"
#include "HAL/RunnableThread.h"

FSRTTransmitter::FSRTTransmitter(const FTransmitterSettings& InSettings)
    : Settings(InSettings)
{
}

FSRTTransmitter::~FSRTTransmitter()
{
    StopTransmission();
    CleanupSRT();
}

bool FSRTTransmitter::Init()
{
    UE_LOG(LogCineSRT, Log, TEXT("Initializing SRT Transmitter - %s:%d"), 
        *Settings.BindAddress, Settings.Port);
    
    return InitializeSRT();
}

uint32 FSRTTransmitter::Run()
{
    UE_LOG(LogCineSRT, Log, TEXT("SRT Transmitter thread started"));
    
    while (!bShouldStop)
    {
        // 클라이언트 연결 대기
        if (ClientSocket == SRT_INVALID_SOCK)
        {
            if (!WaitForClient())
            {
                FPlatformProcess::Sleep(0.1f); // 100ms 대기
                continue;
            }
        }
        
        // 전송 큐에서 프레임 가져와서 전송
        TArray<uint8> FrameData;
        {
            FScopeLock Lock(&QueueCriticalSection);
            if (TransmissionQueue.Num() > 0)
            {
                FrameData = TransmissionQueue[0];
                TransmissionQueue.RemoveAt(0);
            }
        }
        
        if (FrameData.Num() > 0)
        {
            if (SendFrameData(FrameData))
            {
                OnFrameTransmitted.ExecuteIfBound(FrameData);
            }
        }
        else
        {
            FPlatformProcess::Sleep(0.001f); // 1ms 대기
        }
    }
    
    UE_LOG(LogCineSRT, Log, TEXT("SRT Transmitter thread stopped"));
    return 0;
}

void FSRTTransmitter::Stop()
{
    bShouldStop = true;
}

void FSRTTransmitter::Exit()
{
    CleanupSRT();
}

bool FSRTTransmitter::StartTransmission()
{
    if (bIsTransmitting)
    {
        UE_LOG(LogCineSRT, Warning, TEXT("Transmission already active"));
        return false;
    }
    
    if (!InitializeSRT())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to initialize SRT"));
        return false;
    }
    
    if (!ConfigureSocket())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to configure socket"));
        return false;
    }
    
    bIsTransmitting = true;
    bShouldStop = false;
    
    UE_LOG(LogCineSRT, Log, TEXT("Started SRT transmission on port %d"), Settings.Port);
    return true;
}

void FSRTTransmitter::StopTransmission()
{
    if (!bIsTransmitting)
    {
        return;
    }
    
    bIsTransmitting = false;
    bShouldStop = true;
    
    UE_LOG(LogCineSRT, Log, TEXT("Stopped SRT transmission"));
}

bool FSRTTransmitter::TransmitFrame(const TArray<uint8>& FrameData)
{
    if (!bIsTransmitting)
    {
        return false;
    }
    
    FScopeLock Lock(&QueueCriticalSection);
    TransmissionQueue.Add(FrameData);
    
    return true;
}

void FSRTTransmitter::UpdateSettings(const FTransmitterSettings& NewSettings)
{
    Settings = NewSettings;
}

bool FSRTTransmitter::InitializeSRT()
{
#if WITH_SRT
    if (srt_startup() == 0)
    {
        UE_LOG(LogCineSRT, Log, TEXT("SRT library initialized"));
        return true;
    }
    else
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to initialize SRT library"));
        return false;
    }
#else
    UE_LOG(LogCineSRT, Error, TEXT("SRT library not available"));
    return false;
#endif
}

bool FSRTTransmitter::ConfigureSocket()
{
#if WITH_SRT
    // 서버 소켓 생성
    ServerSocket = srt_create_socket();
    if (ServerSocket == SRT_INVALID_SOCK)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to create SRT socket"));
        return false;
    }
    
    // 소켓 옵션 설정
    srt_setsockopt(ServerSocket, 0, SRTO_LATENCY, &Settings.LatencyTolerance, sizeof(Settings.LatencyTolerance));
    srt_setsockopt(ServerSocket, 0, SRTO_MAXBW, &Settings.MaxBW, sizeof(Settings.MaxBW));
    srt_setsockopt(ServerSocket, 0, SRTO_INPUTBW, &Settings.InputBW, sizeof(Settings.InputBW));
    srt_setsockopt(ServerSocket, 0, SRTO_OHEADBW, &Settings.Overhead, sizeof(Settings.Overhead));
    
    // 바인딩
    sockaddr_in sa;
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(Settings.Port);
    
    if (Settings.BindAddress == TEXT("0.0.0.0"))
    {
        sa.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        inet_pton(AF_INET, TCHAR_TO_UTF8(*Settings.BindAddress), &sa.sin_addr);
    }
    
    if (srt_bind(ServerSocket, (sockaddr*)&sa, sizeof sa) == SRT_ERROR)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to bind SRT socket to port %d"), Settings.Port);
        return false;
    }
    
    // 리스닝 시작
    if (srt_listen(ServerSocket, 1) == SRT_ERROR)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to start listening"));
        return false;
    }
    
    UE_LOG(LogCineSRT, Log, TEXT("SRT socket configured successfully"));
    return true;
#else
    return false;
#endif
}

bool FSRTTransmitter::WaitForClient()
{
#if WITH_SRT
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    
    ClientSocket = srt_accept(ServerSocket, (sockaddr*)&clientAddr, &addrLen);
    if (ClientSocket != SRT_INVALID_SOCK)
    {
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        UE_LOG(LogCineSRT, Log, TEXT("Client connected from %s:%d"), UTF8_TO_TCHAR(clientIP), ntohs(clientAddr.sin_port));
        return true;
    }
    
    return false;
#else
    return false;
#endif
}

bool FSRTTransmitter::SendFrameData(const TArray<uint8>& FrameData)
{
#if WITH_SRT
    if (ClientSocket == SRT_INVALID_SOCK)
    {
        return false;
    }
    
    int sent = srt_send(ClientSocket, (char*)FrameData.GetData(), FrameData.Num());
    if (sent == SRT_ERROR)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to send frame data: %s"), UTF8_TO_TCHAR(srt_getlasterror_str()));
        OnError.ExecuteIfBound(FString::Printf(TEXT("Send error: %s"), UTF8_TO_TCHAR(srt_getlasterror_str())));
        return false;
    }
    
    return true;
#else
    return false;
#endif
}

void FSRTTransmitter::CleanupSRT()
{
#if WITH_SRT
    if (ClientSocket != SRT_INVALID_SOCK)
    {
        srt_close(ClientSocket);
        ClientSocket = SRT_INVALID_SOCK;
    }
    
    if (ServerSocket != SRT_INVALID_SOCK)
    {
        srt_close(ServerSocket);
        ServerSocket = SRT_INVALID_SOCK;
    }
    
    srt_cleanup();
    UE_LOG(LogCineSRT, Log, TEXT("SRT cleanup completed"));
#endif
} 
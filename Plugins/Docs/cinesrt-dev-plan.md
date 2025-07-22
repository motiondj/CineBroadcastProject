# CineSRTStream 플러그인 개발계획서 v1.0
> 언리얼엔진 5.5 기반 실시간 SRT 스트리밍 컴포넌트 플러그인

---

## 📌 프로젝트 개요

### 1.1 목적
게임 내 카메라를 선택하여 컴포넌트 추가만으로 즉시 SRT 프로토콜 기반 실시간 스트리밍이 가능한 플러그인 개발

### 1.2 핵심 컨셉
```
카메라 액터 선택 → Add Component → SRT Stream → 포트 설정 → 자동 스트리밍
```

### 1.3 목표
- **간편성**: 블루프린트 없이 컴포넌트 추가만으로 작동
- **성능**: GPU 인코딩으로 게임 성능 영향 최소화
- **확장성**: 프리뷰/최종 출력 품질 동적 전환
- **안정성**: 언리얼엔진 5.5 최적화 및 에러 핸들링

---

## 🏗️ 기술 아키텍처

### 2.1 플러그인 구조
```
CineSRTStream/
├── Source/
│   ├── CineSRTStream/
│   │   ├── Public/
│   │   │   ├── CineSRTStreamComponent.h      // 핵심 컴포넌트
│   │   │   ├── SRTEncoder.h                  // 인코딩 담당
│   │   │   ├── SRTTransmitter.h             // 전송 담당
│   │   │   └── CineSRTStreamSettings.h      // 프로젝트 설정
│   │   └── Private/
│   │       ├── CineSRTStreamComponent.cpp
│   │       ├── SRTEncoder.cpp
│   │       ├── SRTTransmitter.cpp
│   │       └── CineSRTStreamModule.cpp
│   └── ThirdParty/
│       └── SRT/                              // SRT SDK
│           ├── include/
│           ├── lib/Win64/
│           └── lib/Linux/
├── Resources/
│   └── Icon128.png
└── CineSRTStream.uplugin
```

### 2.2 의존성
```json
{
  "Modules": [
    {
      "Name": "CineSRTStream",
      "Type": "Runtime",
      "LoadingPhase": "Default"
    }
  ],
  "Dependencies": [
    "Core",
    "CoreUObject", 
    "Engine",
    "RenderCore",
    "RHI",
    "CinematicCamera",
    "MediaAssets"
  ]
}
```

---

## 🔧 핵심 클래스 설계

### 3.1 USRTStreamComponent

```cpp
UCLASS(ClassGroup=(Streaming), meta=(BlueprintSpawnableComponent, DisplayName="SRT Stream"))
class CINESRTSTREAM_API USRTStreamComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // 에디터 노출 프로퍼티
    UPROPERTY(EditAnywhere, Category = "SRT Settings", meta=(ClampMin=1024, ClampMax=65535))
    int32 StreamPort = 9001;
    
    UPROPERTY(EditAnywhere, Category = "SRT Settings")
    FString StreamID = "Camera1";
    
    UPROPERTY(EditAnywhere, Category = "SRT Settings")
    bool bAutoStartStream = true;
    
    UPROPERTY(EditAnywhere, Category = "Quality")
    ESRTStreamQuality StreamQuality = ESRTStreamQuality::HD_1080p;
    
    UPROPERTY(EditAnywhere, Category = "Quality", meta=(EditCondition="StreamQuality==ESRTStreamQuality::Custom"))
    FIntPoint CustomResolution = FIntPoint(1920, 1080);
    
    UPROPERTY(EditAnywhere, Category = "Quality", meta=(ClampMin=1, ClampMax=60))
    int32 TargetFPS = 30;
    
    UPROPERTY(EditAnywhere, Category = "Quality", meta=(ClampMin=1000, ClampMax=50000))
    int32 Bitrate = 8000; // Kbps
    
    // 런타임 제어
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    void StartStreaming();
    
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    void StopStreaming();
    
    UFUNCTION(BlueprintCallable, Category = "SRT Stream")
    void SetStreamQuality(bool bHighQuality);

private:
    UPROPERTY()
    class UCameraComponent* CachedCameraComponent;
    
    TUniquePtr<FSRTEncoder> Encoder;
    TUniquePtr<FSRTTransmitter> Transmitter;
    
    FRenderTarget2DResource* RenderTargetResource;
    bool bIsStreaming = false;
};
```

### 3.2 품질 프리셋

```cpp
UENUM(BlueprintType)
enum class ESRTStreamQuality : uint8
{
    Preview_480p    UMETA(DisplayName = "Preview (480p 15fps)"),
    HD_720p         UMETA(DisplayName = "HD (720p 30fps)"),
    HD_1080p        UMETA(DisplayName = "Full HD (1080p 30fps)"),
    HD_1080p60      UMETA(DisplayName = "Full HD (1080p 60fps)"),
    UHD_4K          UMETA(DisplayName = "4K (2160p 30fps)"),
    Custom          UMETA(DisplayName = "Custom")
};
```

---

## 💻 구현 세부사항

### 4.1 컴포넌트 초기화

```cpp
void USRTStreamComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 1. 카메라 컴포넌트 찾기
    FindCameraComponent();
    
    // 2. 렌더타겟 생성
    CreateRenderTarget();
    
    // 3. 인코더 초기화
    InitializeEncoder();
    
    // 4. 자동 시작 옵션 확인
    if (bAutoStartStream && CachedCameraComponent)
    {
        StartStreaming();
    }
}
```

### 4.2 카메라 캡처 프로세스

```cpp
void USRTStreamComponent::CaptureFrame()
{
    if (!CachedCameraComponent || !RenderTarget) return;
    
    // 언리얼 5.5 새로운 렌더링 파이프라인 활용
    ENQUEUE_RENDER_COMMAND(CaptureCommand)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            // GPU에서 직접 캡처
            FTexture2DRHIRef SourceTexture = RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
            
            // NVENC로 직접 전달 (복사 최소화)
            Encoder->SubmitFrame(SourceTexture);
        }
    );
}
```

### 4.3 인코딩 설정

```cpp
void USRTStreamComponent::InitializeEncoder()
{
    FEncoderSettings Settings;
    
    // 품질 프리셋에 따른 설정
    switch (StreamQuality)
    {
        case ESRTStreamQuality::Preview_480p:
            Settings.Width = 854;
            Settings.Height = 480;
            Settings.FPS = 15;
            Settings.Bitrate = 1500;
            break;
            
        case ESRTStreamQuality::HD_1080p:
            Settings.Width = 1920;
            Settings.Height = 1080;
            Settings.FPS = 30;
            Settings.Bitrate = 8000;
            break;
            
        // ... 기타 프리셋
    }
    
    // GPU 인코더 우선순위
    Settings.PreferredEncoder = EEncoderType::NVENC; // AMD AMF, Intel QSV 자동 폴백
    
    Encoder = MakeUnique<FSRTEncoder>(Settings);
}
```

---

## 🚀 개발 로드맵

### Phase 1: 기초 구현 (1주)

#### Day 1-2: 프로젝트 설정
- [x] 플러그인 구조 생성
- [x] SRT SDK 통합 (Windows)
- [x] 기본 Build.cs 설정
- [x] 언리얼 5.5 모듈 시스템 적용

#### Day 3-4: 컴포넌트 개발
- [ ] USRTStreamComponent 기본 구현
- [ ] 에디터 UI 프로퍼티 노출
- [ ] 카메라 자동 감지 로직

#### Day 5-7: 캡처 시스템
- [ ] SceneCapture2D 기반 캡처
- [ ] RenderTarget 관리
- [ ] 프레임 버퍼링

### Phase 2: 인코딩 & 전송 (1주)

#### Day 8-10: 인코더 통합
- [ ] NVENC SDK 연동
- [ ] CPU 폴백 인코더
- [ ] 품질 프리셋 구현

#### Day 11-14: SRT 전송
- [ ] SRT 소켓 관리
- [ ] 연결 관리 (재연결 로직)
- [ ] 에러 핸들링

### Phase 3: 최적화 & 기능 확장 (1주)

#### Day 15-17: 성능 최적화
- [ ] GPU 직접 메모리 접근
- [ ] 멀티스레드 파이프라인
- [ ] 메모리 풀 구현

#### Day 18-21: 고급 기능
- [ ] 동적 품질 전환
- [ ] 멀티 스트림 지원
- [ ] 통계 및 모니터링

---

## ⚙️ 성능 목표

### 6.1 리소스 사용량
```
단일 스트림 (1080p 30fps):
- CPU: < 5%
- GPU: < 15% (인코딩 포함)
- 메모리: < 200MB
- 네트워크: 8Mbps

프리뷰 모드 (480p 15fps):
- CPU: < 2%
- GPU: < 5%
- 메모리: < 50MB
- 네트워크: 1.5Mbps
```

### 6.2 지연시간
- 캡처 → 인코딩: < 16ms (1프레임)
- 인코딩 → 전송: < 10ms
- 총 엔드투엔드: < 100ms

---

## 🔍 에러 처리

### 7.1 일반적인 에러 상황
```cpp
// 포트 충돌
if (!Transmitter->BindPort(StreamPort))
{
    UE_LOG(LogSRT, Error, TEXT("Port %d already in use"), StreamPort);
    // 자동으로 다음 포트 시도
    StreamPort++;
}

// 인코더 실패
if (!Encoder->IsAvailable())
{
    UE_LOG(LogSRT, Warning, TEXT("GPU encoder not available, falling back to CPU"));
    Encoder->SetEncoderType(EEncoderType::CPU_x264);
}

// 네트워크 끊김
Transmitter->OnConnectionLost.AddLambda([this]()
{
    // 지수 백오프로 재연결
    RetryConnection();
});
```

---

## 📝 사용 가이드

### 8.1 기본 사용법
```
1. Content Browser에서 CineCameraActor 생성
2. 레벨에 배치
3. Details 패널 → Add Component → "SRT Stream" 검색
4. 컴포넌트 추가 후 설정:
   - Stream Port: 9001 (겹치지 않게)
   - Stream Quality: HD_1080p
   - Auto Start Stream: ✓
5. Play → 자동으로 스트리밍 시작
```

### 8.2 OBS 설정
```
1. Sources → Add → Media Source
2. Input: srt://localhost:9001
3. 바로 영상 확인 가능
```

### 8.3 멀티 카메라 설정
```
Camera_Main → SRT Port 9001
Camera_Side → SRT Port 9002  
Camera_Top → SRT Port 9003
Camera_Detail → SRT Port 9004

OBS에서 4개 소스로 받아 멀티뷰 구성
```

---

## ✅ 검증 계획

### 9.1 단위 테스트
- 컴포넌트 생성/삭제
- 인코더 초기화
- 네트워크 연결

### 9.2 통합 테스트
- 4개 카메라 동시 스트리밍
- 품질 전환 테스트
- 장시간 안정성

### 9.3 성능 테스트
- GPU 사용률 측정
- 메모리 누수 검사
- 네트워크 대역폭

---

## 🎯 성공 지표

1. **사용성**: 컴포넌트 추가 후 2분 내 스트리밍
2. **성능**: 4개 스트림 동시 전송시 60fps 유지
3. **안정성**: 24시간 연속 스트리밍 무중단
4. **호환성**: 언리얼 5.5 모든 기능 정상 작동

---

**문서 버전**: 1.0  
**작성일**: 2025.01.22  
**대상**: 언리얼엔진 5.5 개발자
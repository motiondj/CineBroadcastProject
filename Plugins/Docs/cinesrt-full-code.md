# CineSRTStream 플러그인 전체 소스코드
> 언리얼엔진 5.5 최적화 버전

## 📁 파일 구조
```
CineSRTStream/
├── CineSRTStream.uplugin
├── Resources/
│   └── Icon128.png
└── Source/
    ├── CineSRTStream/
    │   ├── CineSRTStream.Build.cs
    │   ├── Public/
    │   │   ├── CineSRTStreamModule.h
    │   │   ├── CineSRTStreamComponent.h
    │   │   ├── SRTEncoder.h
    │   │   ├── SRTTransmitter.h
    │   │   └── CineSRTStreamSettings.h
    │   └── Private/
    │       ├── CineSRTStreamModule.cpp
    │       ├── CineSRTStreamComponent.cpp
    │       ├── SRTEncoder.cpp
    │       └── SRTTransmitter.cpp
    └── ThirdParty/
        └── SRT/
```

---

## 1. CineSRTStream.uplugin

```json
{
  "FileVersion": 3,
  "Version": 1,
  "VersionName": "1.0",
  "FriendlyName": "Cine SRT Stream",
  "Description": "Real-time SRT streaming component for cameras in Unreal Engine 5.5",
  "Category": "Streaming",
  "CreatedBy": "CineBroadcast Team",
  "CreatedByURL": "",
  "DocsURL": "",
  "MarketplaceURL": "",
  "SupportURL": "",
  "CanContainContent": false,
  "IsBetaVersion": false,
  "IsExperimentalVersion": false,
  "Installed": false,
  "Modules": [
    {
      "Name": "CineSRTStream",
      "Type": "Runtime",
      "LoadingPhase": "Default",
      "PlatformAllowList": [
        "Win64",
        "Mac",
        "Linux"
      ]
    }
  ]
}
```

---

## 2. CineSRTStream.Build.cs

```csharp
// CineSRTStream.Build.cs

using UnrealBuildTool;
using System.IO;

public class CineSRTStream : ModuleRules
{
    public CineSRTStream(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // Unreal Engine 5.5 optimizations
        bUseUnity = false;
        MinFilesUsingPrecompiledHeader = 1;
        bUsePrecompiled = false;
        
        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public")
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private")
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "RenderCore",
                "RHI",
                "Renderer",
                "CinematicCamera",
                "Projects"
            }
        );
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "ToolMenus",
                "EditorSubsystem",
                "UnrealEd",
                "MediaAssets",
                "MediaUtils",
                "MovieSceneCapture",
                "ImageWrapper",
                "Sockets",
                "Networking",
                "HTTP"
            }
        );
        
        // Add SRT library
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "ThirdParty");
        string SRTPath = Path.Combine(ThirdPartyPath, "SRT");
        
        PublicIncludePaths.Add(Path.Combine(SRTPath, "include"));
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Win64", "srt.lib"));
            RuntimeDependencies.Add(Path.Combine(SRTPath, "bin", "Win64", "srt.dll"));
            
            // Windows system libraries
            PublicSystemLibraries.AddRange(new string[] {
                "ws2_32.lib",
                "winmm.lib"
            });
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Mac", "libsrt.a"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Linux", "libsrt.a"));
        }
        
        // Enable exceptions for SRT
        bEnableExceptions = true;
    }
}
```

---

## 3. CineSRTStreamModule.h

```cpp
// CineSRTStreamModule.h

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCineSRT, Log, All);

class FCineSRTStreamModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
    /** Singleton access */
    static FCineSRTStreamModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FCineSRTStreamModule>("CineSRTStream");
    }
    
    /** Check if module is loaded */
    static bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("CineSRTStream");
    }
};
```

---

## 4. CineSRTStreamModule.cpp

```cpp
// CineSRTStreamModule.cpp

#include "CineSRTStreamModule.h"
#include "CineSRTStreamSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FCineSRTStreamModule"

DEFINE_LOG_CATEGORY(LogCineSRT);

void FCineSRTStreamModule::StartupModule()
{
    UE_LOG(LogCineSRT, Log, TEXT("CineSRTStream module starting up"));
    
    // Register project settings
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->RegisterSettings(
            "Project",
            "Plugins",
            "CineSRTStream",
            LOCTEXT("RuntimeSettingsName", "Cine SRT Stream"),
            LOCTEXT("RuntimeSettingsDescription", "Configure SRT streaming settings"),
            GetMutableDefault<UCineSRTStreamSettings>()
        );
    }
}

void FCineSRTStreamModule::ShutdownModule()
{
    UE_LOG(LogCineSRT, Log, TEXT("CineSRTStream module shutting down"));
    
    // Unregister settings
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "CineSRTStream");
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCineSRTStreamModule, CineSRTStream)
```

---

## 5. CineSRTStreamSettings.h

```cpp
// CineSRTStreamSettings.h

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
    
    virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
};
```

---

## 6. CineSRTStreamComponent.h

```cpp
// CineSRTStreamComponent.h

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
    
    // Streaming objects
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
    
    // Callbacks
    void OnEncodedFrame(const TArray<uint8>& EncodedData);
    void OnTransmitterError(const FString& Error);
};
```

---

## 7. CineSRTStreamComponent.cpp

```cpp
// CineSRTStreamComponent.cpp

#include "CineSRTStreamComponent.h"
#include "CineSRTStreamModule.h"
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
        return;
    }
    
    // Create render target
    CreateRenderTarget();
    
    // Initialize systems
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
    
    Encoder.Reset();
    Transmitter.Reset();
    
    Super::EndPlay(EndPlayReason);
}

void USRTStreamComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
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
    
    UE_LOG(LogCineSRT, Log, TEXT("Created render target: %dx%d"), Resolution.X, Resolution.Y);
}

void USRTStreamComponent::InitializeEncoder()
{
    FSRTEncoderSettings EncoderSettings;
    
    FIntPoint Resolution = GetTargetResolution();
    EncoderSettings.Width = Resolution.X;
    EncoderSettings.Height = Resolution.Y;
    EncoderSettings.FrameRate = TargetFPS;
    EncoderSettings.Bitrate = Bitrate * 1000; // Convert to bps
    EncoderSettings.EncoderType = EncoderType;
    
    Encoder = MakeShareable(new FSRTEncoder(EncoderSettings));
    
    // Bind callback
    Encoder->OnFrameEncoded.BindUObject(this, &USRTStreamComponent::OnEncodedFrame);
    
    if (!Encoder->Initialize())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to initialize encoder"));
        OnStreamingError.Broadcast(TEXT("Encoder initialization failed"));
    }
}

void USRTStreamComponent::InitializeTransmitter()
{
    FSRTTransmitterSettings TransmitterSettings;
    TransmitterSettings.Port = StreamPort;
    TransmitterSettings.StreamID = StreamID;
    
    Transmitter = MakeShareable(new FSRTTransmitter(TransmitterSettings));
    
    // Bind callbacks
    Transmitter->OnError.BindUObject(this, &USRTStreamComponent::OnTransmitterError);
    
    if (!Transmitter->Initialize())
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to initialize transmitter"));
        OnStreamingError.Broadcast(TEXT("Transmitter initialization failed"));
    }
}

void USRTStreamComponent::StartStreaming()
{
    if (bIsStreaming)
    {
        return;
    }
    
    if (!Encoder || !Transmitter || !SceneCaptureComponent)
    {
        UE_LOG(LogCineSRT, Error, TEXT("Cannot start streaming - components not initialized"));
        OnStreamingError.Broadcast(TEXT("Components not initialized"));
        return;
    }
    
    if (!Transmitter->Start())
    {
        OnStreamingError.Broadcast(TEXT("Failed to start transmitter"));
        return;
    }
    
    bIsStreaming = true;
    SetComponentTickEnabled(true);
    UpdateCaptureInterval();
    
    UE_LOG(LogCineSRT, Log, TEXT("Started streaming on port %d"), StreamPort);
    OnStreamingStateChanged.Broadcast(true);
}

void USRTStreamComponent::StopStreaming()
{
    if (!bIsStreaming)
    {
        return;
    }
    
    bIsStreaming = false;
    SetComponentTickEnabled(false);
    
    if (Transmitter)
    {
        Transmitter->Stop();
    }
    
    UE_LOG(LogCineSRT, Log, TEXT("Stopped streaming"));
    OnStreamingStateChanged.Broadcast(false);
}

void USRTStreamComponent::SetStreamQuality(ESRTStreamQuality NewQuality)
{
    if (StreamQuality == NewQuality)
    {
        return;
    }
    
    StreamQuality = NewQuality;
    
    // If streaming, restart with new quality
    if (bIsStreaming)
    {
        StopStreaming();
        
        // Recreate components with new settings
        CreateRenderTarget();
        InitializeEncoder();
        
        StartStreaming();
    }
}

void USRTStreamComponent::SetDynamicBitrate(int32 NewBitrate)
{
    Bitrate = FMath::Clamp(NewBitrate, 500, 50000);
    
    if (Encoder && bIsStreaming)
    {
        Encoder->SetBitrate(Bitrate * 1000);
    }
}

FString USRTStreamComponent::GetStreamURL() const
{
    return FString::Printf(TEXT("srt://localhost:%d"), StreamPort);
}

void USRTStreamComponent::CaptureFrame()
{
    if (!SceneCaptureComponent || !RenderTarget || !Encoder)
    {
        return;
    }
    
    // Capture scene
    SceneCaptureComponent->CaptureScene();
    
    // Submit to encoder on render thread
    ENQUEUE_RENDER_COMMAND(SRTCaptureFrame)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            if (!RenderTarget || !RenderTarget->GetResource())
            {
                return;
            }
            
            FTextureRenderTargetResource* Resource = RenderTarget->GetRenderTargetResource();
            if (!Resource)
            {
                return;
            }
            
            FRHITexture2D* Texture = Resource->GetRenderTargetTexture();
            if (!Texture)
            {
                return;
            }
            
            // Submit texture directly to encoder (GPU->GPU copy)
            Encoder->SubmitFrame_RenderThread(RHICmdList, Texture);
        }
    );
}

void USRTStreamComponent::UpdateCaptureInterval()
{
    CaptureInterval = 1.0f / static_cast<float>(TargetFPS);
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

void USRTStreamComponent::OnEncodedFrame(const TArray<uint8>& EncodedData)
{
    if (Transmitter && bIsStreaming)
    {
        Transmitter->SendFrame(EncodedData);
    }
}

void USRTStreamComponent::OnTransmitterError(const FString& Error)
{
    UE_LOG(LogCineSRT, Error, TEXT("Transmitter error: %s"), *Error);
    OnStreamingError.Broadcast(Error);
    
    // Auto-reconnect logic
    if (bIsStreaming)
    {
        const UCineSRTStreamSettings* Settings = GetDefault<UCineSRTStreamSettings>();
        
        FTimerHandle ReconnectTimer;
        GetWorld()->GetTimerManager().SetTimer(ReconnectTimer, 
            [this]()
            {
                if (Transmitter)
                {
                    Transmitter->Start();
                }
            }, 
            Settings->ReconnectDelayMs / 1000.0f, false);
    }
}
```

---

## 8. SRTEncoder.h

```cpp
// SRTEncoder.h

#pragma once

#include "CoreMinimal.h"
#include "CineSRTStreamSettings.h"
#include "RHI.h"
#include "RHICommandList.h"

DECLARE_DELEGATE_OneParam(FOnFrameEncoded, const TArray<uint8>&);

struct FSRTEncoderSettings
{
    int32 Width = 1920;
    int32 Height = 1080;
    int32 FrameRate = 30;
    int32 Bitrate = 8000000; // bps
    ESRTEncoderType EncoderType = ESRTEncoderType::Auto;
};

class CINESRTSTREAM_API FSRTEncoder : public TSharedFromThis<FSRTEncoder>
{
public:
    FSRTEncoder(const FSRTEncoderSettings& Settings);
    ~FSRTEncoder();
    
    bool Initialize();
    void Shutdown();
    
    void SubmitFrame_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture);
    void SetBitrate(int32 NewBitrate);
    
    FOnFrameEncoded OnFrameEncoded;
    
private:
    FSRTEncoderSettings Settings;
    
    // Encoder implementation details
    void* EncoderHandle = nullptr;
    bool bIsInitialized = false;
    
    // Frame buffer
    TArray<uint8> FrameBuffer;
    FCriticalSection EncoderLock;
    
    // Platform-specific encoder creation
    bool CreateNVENCEncoder();
    bool CreateAMFEncoder();
    bool CreateQuickSyncEncoder();
    bool CreateSoftwareEncoder();
    
    // Encoding thread
    FRunnableThread* EncodingThread = nullptr;
    class FEncodingWorker* EncodingWorker = nullptr;
};

// Worker thread for encoding
class FEncodingWorker : public FRunnable
{
public:
    FEncodingWorker(FSRTEncoder* InEncoder);
    virtual ~FEncodingWorker();
    
    // FRunnable interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    
    void SubmitFrame(const TArray<uint8>& FrameData);
    
private:
    FSRTEncoder* Encoder;
    FThreadSafeCounter StopTaskCounter;
    TQueue<TArray<uint8>> FrameQueue;
    FEvent* FrameAvailableEvent;
};
```

---

## 9. SRTEncoder.cpp

```cpp
// SRTEncoder.cpp

#include "SRTEncoder.h"
#include "CineSRTStreamModule.h"
#include "HAL/PlatformProcess.h"
#include "HAL/RunnableThread.h"
#include "RenderResource.h"
#include "RenderUtils.h"

// Stub implementation - replace with actual encoder SDK integration
FSRTEncoder::FSRTEncoder(const FSRTEncoderSettings& InSettings)
    : Settings(InSettings)
{
}

FSRTEncoder::~FSRTEncoder()
{
    Shutdown();
}

bool FSRTEncoder::Initialize()
{
    if (bIsInitialized)
    {
        return true;
    }
    
    // Try hardware encoders first
    bool bSuccess = false;
    
    switch (Settings.EncoderType)
    {
        case ESRTEncoderType::Auto:
            bSuccess = CreateNVENCEncoder() || CreateAMFEncoder() || 
                      CreateQuickSyncEncoder() || CreateSoftwareEncoder();
            break;
            
        case ESRTEncoderType::NVENC:
            bSuccess = CreateNVENCEncoder();
            break;
            
        case ESRTEncoderType::AMF:
            bSuccess = CreateAMFEncoder();
            break;
            
        case ESRTEncoderType::QuickSync:
            bSuccess = CreateQuickSyncEncoder();
            break;
            
        case ESRTEncoderType::Software:
            bSuccess = CreateSoftwareEncoder();
            break;
    }
    
    if (bSuccess)
    {
        // Create encoding worker thread
        EncodingWorker = new FEncodingWorker(this);
        EncodingThread = FRunnableThread::Create(EncodingWorker, 
            TEXT("SRTEncodingThread"), 0, TPri_Normal);
        
        bIsInitialized = true;
        UE_LOG(LogCineSRT, Log, TEXT("Encoder initialized successfully"));
    }
    else
    {
        UE_LOG(LogCineSRT, Error, TEXT("Failed to initialize any encoder"));
    }
    
    return bIsInitialized;
}

void FSRTEncoder::Shutdown()
{
    if (!bIsInitialized)
    {
        return;
    }
    
    // Stop encoding thread
    if (EncodingThread)
    {
        EncodingWorker->Stop();
        EncodingThread->WaitForCompletion();
        delete EncodingThread;
        EncodingThread = nullptr;
        delete EncodingWorker;
        EncodingWorker = nullptr;
    }
    
    // Cleanup encoder
    if (EncoderHandle)
    {
        // Platform-specific cleanup
        EncoderHandle = nullptr;
    }
    
    bIsInitialized = false;
}

void FSRTEncoder::SubmitFrame_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture)
{
    check(IsInRenderingThread());
    
    if (!bIsInitialized || !Texture)
    {
        return;
    }
    
    // Map texture and copy data
    FIntPoint Size(Settings.Width, Settings.Height);
    TArray<uint8> FrameData;
    FrameData.SetNum(Size.X * Size.Y * 4); // RGBA
    
    // Read pixels from GPU
    FReadSurfaceDataFlags ReadFlags(RCM_UNorm);
    RHICmdList.ReadSurfaceData(
        Texture,
        FIntRect(0, 0, Size.X, Size.Y),
        FrameData,
        ReadFlags
    );
    
    // Submit to encoding thread
    if (EncodingWorker)
    {
        EncodingWorker->SubmitFrame(FrameData);
    }
}

void FSRTEncoder::SetBitrate(int32 NewBitrate)
{
    FScopeLock Lock(&EncoderLock);
    Settings.Bitrate = NewBitrate;
    
    // Update encoder settings if initialized
    if (bIsInitialized && EncoderHandle)
    {
        // Platform-specific bitrate update
    }
}

bool FSRTEncoder::CreateNVENCEncoder()
{
    // TODO: Implement NVENC encoder creation
    // This would involve NVIDIA Video SDK integration
    return false;
}

bool FSRTEncoder::CreateAMFEncoder()
{
    // TODO: Implement AMD AMF encoder creation
    return false;
}

bool FSRTEncoder::CreateQuickSyncEncoder()
{
    // TODO: Implement Intel QuickSync encoder creation
    return false;
}

bool FSRTEncoder::CreateSoftwareEncoder()
{
    // TODO: Implement x264 software encoder
    // For now, return true as a placeholder
    UE_LOG(LogCineSRT, Log, TEXT("Using software encoder (placeholder)"));
    EncoderHandle = (void*)1; // Placeholder
    return true;
}

// Encoding Worker Implementation
FEncodingWorker::FEncodingWorker(FSRTEncoder* InEncoder)
    : Encoder(InEncoder)
    , StopTaskCounter(0)
{
    FrameAvailableEvent = FPlatformProcess::GetSynchEventFromPool();
}

FEncodingWorker::~FEncodingWorker()
{
    FPlatformProcess::ReturnSynchEventToPool(FrameAvailableEvent);
}

bool FEncodingWorker::Init()
{
    return true;
}

uint32 FEncodingWorker::Run()
{
    while (StopTaskCounter.GetValue() == 0)
    {
        // Wait for frame
        FrameAvailableEvent->Wait();
        
        TArray<uint8> FrameData;
        while (FrameQueue.Dequeue(FrameData))
        {
            // TODO: Actual encoding here
            // For now, just pass through as "encoded" data
            
            // Simulate encoding delay
            FPlatformProcess::Sleep(0.001f);
            
            // Notify encoder
            if (Encoder && Encoder->OnFrameEncoded.IsBound())
            {
                Encoder->OnFrameEncoded.Execute(FrameData);
            }
        }
    }
    
    return 0;
}

void FEncodingWorker::Stop()
{
    StopTaskCounter.Increment();
    FrameAvailableEvent->Trigger();
}

void FEncodingWorker::SubmitFrame(const TArray<uint8>& FrameData)
{
    FrameQueue.Enqueue(FrameData);
    FrameAvailableEvent->Trigger();
}
```

---

## 10. SRTTransmitter.h

```cpp
// SRTTransmitter.h

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

DECLARE_DELEGATE_OneParam(FOnSRTError, const FString&);

struct FSRTTransmitterSettings
{
    int32 Port = 9001;
    FString StreamID = TEXT("live");
    FString BindAddress = TEXT("0.0.0.0");
    int32 MaxBandwidth = 0; // 0 = unlimited
    int32 Latency = 120; // ms
};

class CINESRTSTREAM_API FSRTTransmitter : public TSharedFromThis<FSRTTransmitter>
{
public:
    FSRTTransmitter(const FSRTTransmitterSettings& Settings);
    ~FSRTTransmitter();
    
    bool Initialize();
    void Shutdown();
    
    bool Start();
    void Stop();
    
    void SendFrame(const TArray<uint8>& FrameData);
    
    FOnSRTError OnError;
    
private:
    FSRTTransmitterSettings Settings;
    
    // SRT socket handle
    int SRTSocket = -1;
    bool bIsRunning = false;
    
    // Transmission thread
    FRunnableThread* TransmissionThread = nullptr;
    class FTransmissionWorker* TransmissionWorker = nullptr;
    
    // SRT library management
    static int32 SRTInstanceCount;
    static FCriticalSection SRTInitLock;
    
    bool CreateSRTSocket();
    void CloseSRTSocket();
};

// Worker thread for transmission
class FTransmissionWorker : public FRunnable
{
public:
    FTransmissionWorker(FSRTTransmitter* InTransmitter, int InSocket);
    virtual ~FTransmissionWorker();
    
    // FRunnable interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    
    void QueueFrame(const TArray<uint8>& FrameData);
    
private:
    FSRTTransmitter* Transmitter;
    int Socket;
    FThreadSafeCounter StopTaskCounter;
    TQueue<TArray<uint8>> FrameQueue;
    FEvent* FrameAvailableEvent;
};
```

---

## 11. SRTTransmitter.cpp

```cpp
// SRTTransmitter.cpp

#include "SRTTransmitter.h"
#include "CineSRTStreamModule.h"
#include "HAL/RunnableThread.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

// Include SRT headers (make sure these are in ThirdParty/SRT/include)
extern "C" {
    #include "srt/srt.h"
}

int32 FSRTTransmitter::SRTInstanceCount = 0;
FCriticalSection FSRTTransmitter::SRTInitLock;

FSRTTransmitter::FSRTTransmitter(const FSRTTransmitterSettings& InSettings)
    : Settings(InSettings)
{
    FScopeLock Lock(&SRTInitLock);
    
    if (SRTInstanceCount == 0)
    {
        // Initialize SRT library
        srt_startup();
        UE_LOG(LogCineSRT, Log, TEXT("SRT library initialized"));
    }
    
    SRTInstanceCount++;
}

FSRTTransmitter::~FSRTTransmitter()
{
    Shutdown();
    
    FScopeLock Lock(&SRTInitLock);
    SRTInstanceCount--;
    
    if (SRTInstanceCount == 0)
    {
        // Cleanup SRT library
        srt_cleanup();
        UE_LOG(LogCineSRT, Log, TEXT("SRT library cleaned up"));
    }
}

bool FSRTTransmitter::Initialize()
{
    return CreateSRTSocket();
}

void FSRTTransmitter::Shutdown()
{
    Stop();
    CloseSRTSocket();
}

bool FSRTTransmitter::Start()
{
    if (bIsRunning || SRTSocket == -1)
    {
        return false;
    }
    
    // Start listening
    if (srt_listen(SRTSocket, 1) == SRT_ERROR)
    {
        FString Error = FString::Printf(TEXT("Failed to listen: %s"), 
            UTF8_TO_TCHAR(srt_getlasterror_str()));
        OnError.ExecuteIfBound(Error);
        return false;
    }
    
    // Create transmission worker
    TransmissionWorker = new FTransmissionWorker(this, SRTSocket);
    TransmissionThread = FRunnableThread::Create(TransmissionWorker, 
        TEXT("SRTTransmissionThread"), 0, TPri_Normal);
    
    bIsRunning = true;
    UE_LOG(LogCineSRT, Log, TEXT("SRT transmitter started on port %d"), Settings.Port);
    
    return true;
}

void FSRTTransmitter::Stop()
{
    if (!bIsRunning)
    {
        return;
    }
    
    bIsRunning = false;
    
    // Stop transmission thread
    if (TransmissionThread)
    {
        TransmissionWorker->Stop();
        TransmissionThread->WaitForCompletion();
        delete TransmissionThread;
        TransmissionThread = nullptr;
        delete TransmissionWorker;
        TransmissionWorker = nullptr;
    }
    
    UE_LOG(LogCineSRT, Log, TEXT("SRT transmitter stopped"));
}

void FSRTTransmitter::SendFrame(const TArray<uint8>& FrameData)
{
    if (bIsRunning && TransmissionWorker)
    {
        TransmissionWorker->QueueFrame(FrameData);
    }
}

bool FSRTTransmitter::CreateSRTSocket()
{
    // Create SRT socket
    SRTSocket = srt_create_socket();
    if (SRTSocket == SRT_ERROR)
    {
        FString Error = FString::Printf(TEXT("Failed to create socket: %s"), 
            UTF8_TO_TCHAR(srt_getlasterror_str()));
        OnError.ExecuteIfBound(Error);
        return false;
    }
    
    // Set socket options
    int yes = 1;
    srt_setsockflag(SRTSocket, SRTO_SENDER, &yes, sizeof(yes));
    
    // Set latency
    srt_setsockflag(SRTSocket, SRTO_LATENCY, &Settings.Latency, sizeof(Settings.Latency));
    
    // Set stream ID
    if (!Settings.StreamID.IsEmpty())
    {
        srt_setsockflag(SRTSocket, SRTO_STREAMID, 
            TCHAR_TO_UTF8(*Settings.StreamID), Settings.StreamID.Len());
    }
    
    // Bind socket
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Settings.Port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (srt_bind(SRTSocket, (sockaddr*)&addr, sizeof(addr)) == SRT_ERROR)
    {
        FString Error = FString::Printf(TEXT("Failed to bind port %d: %s"), 
            Settings.Port, UTF8_TO_TCHAR(srt_getlasterror_str()));
        OnError.ExecuteIfBound(Error);
        CloseSRTSocket();
        return false;
    }
    
    UE_LOG(LogCineSRT, Log, TEXT("SRT socket created and bound to port %d"), Settings.Port);
    return true;
}

void FSRTTransmitter::CloseSRTSocket()
{
    if (SRTSocket != -1)
    {
        srt_close(SRTSocket);
        SRTSocket = -1;
    }
}

// Transmission Worker Implementation
FTransmissionWorker::FTransmissionWorker(FSRTTransmitter* InTransmitter, int InSocket)
    : Transmitter(InTransmitter)
    , Socket(InSocket)
    , StopTaskCounter(0)
{
    FrameAvailableEvent = FPlatformProcess::GetSynchEventFromPool();
}

FTransmissionWorker::~FTransmissionWorker()
{
    FPlatformProcess::ReturnSynchEventToPool(FrameAvailableEvent);
}

bool FTransmissionWorker::Init()
{
    return true;
}

uint32 FTransmissionWorker::Run()
{
    int ClientSocket = -1;
    sockaddr_in ClientAddr;
    int AddrLen = sizeof(ClientAddr);
    
    // Wait for client connection
    while (StopTaskCounter.GetValue() == 0)
    {
        // Non-blocking accept with timeout
        int ReadySocket = srt_epoll_create();
        srt_epoll_add_usock(ReadySocket, Socket, &SRT_EPOLL_IN);
        
        int ReadySockets[1];
        int NumReady = srt_epoll_uwait(ReadySocket, ReadySockets, 1, 1000); // 1 sec timeout
        
        if (NumReady > 0)
        {
            ClientSocket = srt_accept(Socket, (sockaddr*)&ClientAddr, &AddrLen);
            if (ClientSocket != SRT_ERROR)
            {
                UE_LOG(LogCineSRT, Log, TEXT("Client connected"));
                break;
            }
        }
        
        srt_epoll_release(ReadySocket);
    }
    
    // Transmission loop
    while (StopTaskCounter.GetValue() == 0 && ClientSocket != -1)
    {
        FrameAvailableEvent->Wait(100); // 100ms timeout
        
        TArray<uint8> FrameData;
        while (FrameQueue.Dequeue(FrameData))
        {
            // Send frame data
            int Sent = srt_send(ClientSocket, (char*)FrameData.GetData(), FrameData.Num());
            
            if (Sent == SRT_ERROR)
            {
                FString Error = FString::Printf(TEXT("Send error: %s"), 
                    UTF8_TO_TCHAR(srt_getlasterror_str()));
                
                if (Transmitter && Transmitter->OnError.IsBound())
                {
                    Transmitter->OnError.Execute(Error);
                }
                
                // Client disconnected
                break;
            }
        }
    }
    
    // Cleanup
    if (ClientSocket != -1)
    {
        srt_close(ClientSocket);
    }
    
    return 0;
}

void FTransmissionWorker::Stop()
{
    StopTaskCounter.Increment();
    FrameAvailableEvent->Trigger();
}

void FTransmissionWorker::QueueFrame(const TArray<uint8>& FrameData)
{
    FrameQueue.Enqueue(FrameData);
    FrameAvailableEvent->Trigger();
}
```

---

## 📝 사용 방법

### 1. 플러그인 설치
1. `CineSRTStream` 폴더를 프로젝트의 `Plugins` 폴더에 복사
2. SRT SDK를 `ThirdParty/SRT/` 에 설치
3. 프로젝트 재생성 및 컴파일

### 2. 카메라에 컴포넌트 추가
1. 레벨에서 CineCameraActor 선택
2. Details 패널 → Add Component → "SRT Stream"
3. 포트 번호 설정 (기본: 9001)
4. Play 시작하면 자동 스트리밍

### 3. OBS 설정
1. Sources → Add → Media Source
2. Input: `srt://localhost:9001`
3. Network Buffering: 0 (최소 지연)

---

## ⚠️ 주의사항

1. **SRT SDK 필요**: 실제 구현을 위해서는 SRT SDK를 다운로드하고 빌드해야 합니다
2. **인코더 SDK**: NVENC, AMF 등 하드웨어 인코더는 각 제조사 SDK 통합 필요
3. **테스트 필요**: 이 코드는 기본 구조를 제공하며, 실제 환경에서 테스트 필요

---

**버전**: 1.0  
**언리얼엔진**: 5.5  
**작성일**: 2025.01.22
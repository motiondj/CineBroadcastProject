# SRT SDK 통합 가이드 - 언리얼엔진 5.5
> CineSRTStream 플러그인을 위한 상세 통합 가이드

---

## 📌 개요

SRT (Secure Reliable Transport)는 Haivision에서 개발한 오픈소스 비디오 전송 프로토콜입니다. 이 가이드는 SRT SDK를 언리얼엔진 5.5 프로젝트에 통합하는 완전한 과정을 설명합니다.

---

## 🔧 필요한 도구

### Windows
- **Visual Studio 2022** (v143 toolset)
- **CMake 3.23** 이상
- **Git**
- **NASM** (어셈블러, 선택사항)

### Mac
- **Xcode 14** 이상
- **CMake**
- **Homebrew**

### Linux
- **GCC 11** 이상 또는 **Clang 14** 이상
- **CMake**
- **build-essential**

---

## 📥 Step 1: SRT SDK 다운로드

### 1.1 소스코드 클론
```bash
# 프로젝트의 Plugins/CineSRTStream/Source/ThirdParty 폴더로 이동
cd YourProject/Plugins/CineSRTStream/Source/ThirdParty

# SRT 소스 클론
git clone https://github.com/Haivision/srt.git SRT_Source
cd SRT_Source

# 안정적인 버전으로 체크아웃 (v1.5.3 권장)
git checkout v1.5.3
```

### 1.2 폴더 구조 준비
```bash
cd ..
mkdir -p SRT/include
mkdir -p SRT/lib/Win64
mkdir -p SRT/lib/Mac
mkdir -p SRT/lib/Linux
mkdir -p SRT/bin/Win64
```

---

## 🏗️ Step 2: Windows 빌드

### 2.1 CMake 구성
```batch
cd SRT_Source
mkdir build_win64
cd build_win64

# Visual Studio 2022용 프로젝트 생성
cmake -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DENABLE_SHARED=OFF ^
  -DENABLE_STATIC=ON ^
  -DENABLE_ENCRYPTION=ON ^
  -DENABLE_CXX11=ON ^
  -DUSE_STATIC_LIBSTDCXX=ON ^
  ..
```

### 2.2 빌드
```batch
# Release 빌드
cmake --build . --config Release

# 또는 Visual Studio에서 직접 열어서 빌드
# SRT.sln 파일 열기 → Release 선택 → Build Solution
```

### 2.3 파일 복사
```batch
# 헤더 파일 복사
xcopy /E /I ..\srtcore\*.h ..\..\SRT\include\srt\
xcopy /E /I Release\*.h ..\..\SRT\include\srt\

# 라이브러리 복사
copy Release\srt_static.lib ..\..\SRT\lib\Win64\srt.lib

# OpenSSL 종속성 (필요한 경우)
copy %OPENSSL_ROOT_DIR%\lib\libcrypto.lib ..\..\SRT\lib\Win64\
copy %OPENSSL_ROOT_DIR%\lib\libssl.lib ..\..\SRT\lib\Win64\
```

---

## 🍎 Step 3: Mac 빌드

### 3.1 의존성 설치
```bash
# Homebrew로 필요한 패키지 설치
brew install cmake
brew install openssl@3
```

### 3.2 빌드
```bash
cd SRT_Source
mkdir build_mac
cd build_mac

# CMake 구성
cmake -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_SHARED=OFF \
  -DENABLE_STATIC=ON \
  -DENABLE_ENCRYPTION=ON \
  -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl@3 \
  -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
  ..

# 빌드
make -j8

# 파일 복사
cp ../srtcore/*.h ../../SRT/include/srt/
cp libsrt.a ../../SRT/lib/Mac/
```

---

## 🐧 Step 4: Linux 빌드

### 4.1 의존성 설치
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y cmake build-essential libssl-dev

# CentOS/RHEL
sudo yum install -y cmake3 gcc-c++ openssl-devel
```

### 4.2 빌드
```bash
cd SRT_Source
mkdir build_linux
cd build_linux

# CMake 구성
cmake -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_SHARED=OFF \
  -DENABLE_STATIC=ON \
  -DENABLE_ENCRYPTION=ON \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  ..

# 빌드
make -j$(nproc)

# 파일 복사
cp ../srtcore/*.h ../../SRT/include/srt/
cp libsrt.a ../../SRT/lib/Linux/
```

---

## 📁 Step 5: 최종 폴더 구조 확인

```
ThirdParty/
└── SRT/
    ├── include/
    │   └── srt/
    │       ├── srt.h
    │       ├── srt_compat.h
    │       ├── logging_api.h
    │       └── ... (기타 헤더들)
    ├── lib/
    │   ├── Win64/
    │   │   ├── srt.lib
    │   │   ├── libcrypto.lib (선택)
    │   │   └── libssl.lib (선택)
    │   ├── Mac/
    │   │   └── libsrt.a
    │   └── Linux/
    │       └── libsrt.a
    └── bin/
        └── Win64/
            └── srt.dll (동적 링크 사용시)
```

---

## ⚙️ Step 6: Build.cs 설정 최적화

### 6.1 완전한 Build.cs 설정
```csharp
// CineSRTStream.Build.cs

using UnrealBuildTool;
using System.IO;
using System;

public class CineSRTStream : ModuleRules
{
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/")); }
    }

    public CineSRTStream(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // ... 기존 설정 ...
        
        // SRT SDK 통합
        LoadSRT(Target);
    }
    
    private void LoadSRT(ReadOnlyTargetRules Target)
    {
        string SRTPath = Path.Combine(ThirdPartyPath, "SRT");
        
        // 헤더 경로 추가
        PublicIncludePaths.Add(Path.Combine(SRTPath, "include"));
        
        // 플랫폼별 라이브러리 설정
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string LibPath = Path.Combine(SRTPath, "lib", "Win64");
            
            // SRT 라이브러리
            PublicAdditionalLibraries.Add(Path.Combine(LibPath, "srt.lib"));
            
            // Windows 시스템 라이브러리
            PublicSystemLibraries.AddRange(new string[] {
                "ws2_32.lib",
                "crypt32.lib",
                "Iphlpapi.lib"
            });
            
            // OpenSSL (암호화 사용시)
            if (File.Exists(Path.Combine(LibPath, "libcrypto.lib")))
            {
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libcrypto.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "libssl.lib"));
            }
            
            // DLL 복사 (동적 링크 사용시)
            string DLLPath = Path.Combine(SRTPath, "bin", "Win64", "srt.dll");
            if (File.Exists(DLLPath))
            {
                RuntimeDependencies.Add(DLLPath);
                PublicDelayLoadDLLs.Add("srt.dll");
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Mac", "libsrt.a"));
            
            // Mac 시스템 프레임워크
            PublicFrameworks.AddRange(new string[] {
                "CoreFoundation",
                "Security"
            });
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Linux", "libsrt.a"));
            
            // Linux 시스템 라이브러리
            PublicSystemLibraries.AddRange(new string[] {
                "pthread",
                "crypto",
                "ssl"
            });
        }
        
        // SRT 매크로 정의
        PublicDefinitions.Add("WITH_SRT=1");
        PublicDefinitions.Add("SRT_ENABLE_ENCRYPTION=1");
        
        // C++ 예외 활성화 (SRT에서 필요)
        bEnableExceptions = true;
    }
}
```

---

## 🔍 Step 7: 통합 검증

### 7.1 테스트 코드
```cpp
// TestSRT.cpp

#include "CoreMinimal.h"
#include "srt/srt.h"

void TestSRTIntegration()
{
    // SRT 라이브러리 초기화
    if (srt_startup() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("SRT library initialized successfully"));
        
        // 버전 확인
        UE_LOG(LogTemp, Log, TEXT("SRT Version: %d.%d.%d"), 
            SRT_VERSION_MAJOR, SRT_VERSION_MINOR, SRT_VERSION_PATCH);
        
        // 간단한 소켓 생성 테스트
        SRTSOCKET sock = srt_create_socket();
        if (sock != SRT_INVALID_SOCK)
        {
            UE_LOG(LogTemp, Log, TEXT("SRT socket created successfully"));
            srt_close(sock);
        }
        
        // 정리
        srt_cleanup();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize SRT library"));
    }
}
```

---

## ❗ 일반적인 문제 해결

### 문제 1: 링크 에러
```
LNK2019: unresolved external symbol srt_startup
```
**해결책**:
- Build.cs에서 라이브러리 경로 확인
- srt.lib 파일이 실제로 있는지 확인
- 32비트/64비트 불일치 확인

### 문제 2: 헤더 파일 찾을 수 없음
```
fatal error: srt/srt.h: No such file or directory
```
**해결책**:
- PublicIncludePaths 경로 확인
- include/srt/ 폴더에 헤더 파일 있는지 확인

### 문제 3: DLL 로드 실패
```
Failed to load 'srt.dll'
```
**해결책**:
- RuntimeDependencies에 DLL 경로 추가
- 실행 파일과 같은 폴더에 DLL 복사
- Visual C++ 재배포 가능 패키지 설치

### 문제 4: OpenSSL 종속성
```
LNK2019: unresolved external symbol EVP_CipherInit
```
**해결책**:
- OpenSSL 라이브러리 추가
- 또는 -DENABLE_ENCRYPTION=OFF로 재빌드

---

## 🚀 Step 8: 고급 설정

### 8.1 커스텀 빌드 옵션
```cmake
# 디버그 정보 포함
-DCMAKE_BUILD_TYPE=RelWithDebInfo

# 특정 C++ 표준 사용
-DCMAKE_CXX_STANDARD=17

# 커스텀 OpenSSL 경로
-DOPENSSL_ROOT_DIR="C:/OpenSSL-Win64"

# 스레드 라이브러리 선택
-DENABLE_PTHREAD_WIN32=OFF
```

### 8.2 성능 최적화
```cmake
# CPU 최적화
-DCMAKE_CXX_FLAGS="-O3 -march=native"

# Link Time Optimization
-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

---

## ✅ 최종 체크리스트

- [ ] SRT 소스코드 다운로드 완료
- [ ] 플랫폼별 빌드 완료
- [ ] 헤더 파일 복사 완료
- [ ] 라이브러리 파일 복사 완료
- [ ] Build.cs 설정 완료
- [ ] 컴파일 에러 없음
- [ ] 테스트 코드 실행 성공

---

## 📚 참고 자료

- [SRT 공식 문서](https://github.com/Haivision/srt)
- [SRT API 문서](https://github.com/Haivision/srt/blob/master/docs/API.md)
- [SRT 빌드 가이드](https://github.com/Haivision/srt/blob/master/docs/build/build.md)

---

**작성일**: 2025.01.22  
**SRT 버전**: 1.5.3  
**언리얼엔진**: 5.5
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class CineSRTStream : ModuleRules
{
    public CineSRTStream(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // Unreal Engine 5.5 optimizations
        bUseUnity = false;
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
                "Projects",
                "DeveloperSettings"
            }
        );
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "MediaAssets",
                "MediaUtils",
                "MovieSceneCapture",
                "ImageWrapper",
                "Sockets",
                "Networking",
                "HTTP"
            }
        );
        
        // Editor-only dependencies
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "ToolMenus",
                    "EditorSubsystem",
                    "UnrealEd"
                }
            );
        }
        
        // Add SRT library
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "ThirdParty");
        string SRTPath = Path.Combine(ThirdPartyPath, "SRT");
        
        PublicIncludePaths.Add(Path.Combine(SRTPath, "include"));
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // SRT 라이브러리 경로 (빌드 완료 후 활성화)
            PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Win64", "srt.lib"));
            
            // Windows system libraries
            PublicSystemLibraries.AddRange(new string[] {
                "ws2_32.lib",
                "winmm.lib"
            });
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            // PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Mac", "libsrt.a"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            // PublicAdditionalLibraries.Add(Path.Combine(SRTPath, "lib", "Linux", "libsrt.a"));
        }
        
        // Enable exceptions for SRT
        bEnableExceptions = true;
        
        // SRT 매크로 정의 (암호화 비활성화)
        PublicDefinitions.Add("WITH_SRT=1");
        PublicDefinitions.Add("SRT_ENABLE_ENCRYPTION=0");
    }
}

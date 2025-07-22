// Copyright Epic Games, Inc. All Rights Reserved.

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

// Copyright Epic Games, Inc. All Rights Reserved.

#include "CineSRTStream.h"
#include "CineSRTStreamSettings.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

#define LOCTEXT_NAMESPACE "FCineSRTStreamModule"

DEFINE_LOG_CATEGORY(LogCineSRT);

void FCineSRTStreamModule::StartupModule()
{
    UE_LOG(LogCineSRT, Log, TEXT("CineSRTStream module starting up"));
    
}

void FCineSRTStreamModule::ShutdownModule()
{
    UE_LOG(LogCineSRT, Log, TEXT("CineSRTStream module shutting down"));
    
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCineSRTStreamModule, CineSRTStream)
#include "framework.h"
#include "Hooks.h"

#include "FortGameModeAthena.h"
#include "FortPlayerControllerAthena.h"
#include "Net.h"
#include "FortAbilitySystemComponent.h"
#include "Looting.h"
#include "FortPlayerPawn.h"
#include "FortAIBotControllerAthena.h"

void InitConsole() {
    AllocConsole();
    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w+", stdout);
    SetConsoleTitleA("Spectra 10.40 | Starting...");
    Log("Spectra 10.40, Not for public distribution!");
}

void LoadWorld() {
    Log("Loading World!");
    if (!Globals::bCreativeEnabled && !Globals::bSTWEnabled) {
        UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Athena_Terrain", nullptr);
    }
    else if (Globals::bCreativeEnabled) {
        // Dont know what this is on 10.40
        //UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"", nullptr);
    }
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);
}

void Hook() {
    FortGameModeAthena::HookAll();
    Net::HookAll();
    FortPlayerControllerAthena::HookAll();
    FortAbilitySystemComponent::HookAll();
    Looting::HookAll();
    FortPlayerPawn::HookAll();
    Hooks::HookAll();
    FortAIBotControllerAthena::HookAll();
    MH_EnableHook(MH_ALL_HOOKS);
}

static void WaitForLogin() {
    Log("Waiting for login!");

    FName Frontend = UKismetStringLibrary::Conv_StringToName(L"Frontend");
    FName MatchState = UKismetStringLibrary::Conv_StringToName(L"InProgress");

    while (true) {
        UWorld* CurrentWorld = ((UWorld*)UWorld::GetWorld());
        if (CurrentWorld) {
            if (CurrentWorld->Name == Frontend) {
                auto GameMode = (AGameMode*)CurrentWorld->AuthorityGameMode;
                if (GameMode->GetMatchState() == MatchState) {
                    break;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 1));
    Log("Logged in!");
}

DWORD Main(LPVOID) {
    InitConsole();
    MH_Initialize();
    Log("MinHook Initialised!");

    while (UEngine::GetEngine() == 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    WaitForLogin();

    Hook();

    *(bool*)(ImageBase + 0x637925B) = false; //GIsClient
    *(bool*)(ImageBase + 0x637925C) = true; //GIsServer

    while (UWorld::GetWorld() == nullptr)
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Num() == 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Sleep(1000);
    LoadWorld();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

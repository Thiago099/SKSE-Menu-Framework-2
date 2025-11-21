#include "Hooks.h"
#include "Config.h"
#include "Logger.h"
#include "UI.h"
#include "SKSEMenuFramework.h"

SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    Config::Init();
    WindowManager::MainInterface = AddWindow(UI::RenderMenuWindow);
    Hooks::Install();
    return true;
}

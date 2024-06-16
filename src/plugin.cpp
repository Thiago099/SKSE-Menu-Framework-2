#include "Plugin.h"


SKSEPluginLoad(const SKSE::LoadInterface *skse) {

    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    Config::Init();
    auto builder = UI::Renderer::GetBuilder();
    builder->Install();
    delete builder;
    UI::MainInterface = AddWindow(UI::RenderMenuWindow);
    return true;
}

#include "Renderer.h"

std::vector<UI::Window*> UI::Windows;

GameLock::State GameLock::lastState = GameLock::State::None;

void GameLock::SetState(State currentState) {
    if (lastState == currentState) {
        return;
    }
    lastState = currentState;
    if (Config::FreezeTimeOnMenu) {
        const auto main = RE::Main::GetSingleton();
        if (currentState == State::Locked) {
            main->freezeTime = true;
        } else {
            main->freezeTime = false;
        }
    }

    if (Config::BlurBackgroundOnMenu) {
        if (currentState == State::Locked) {
            RE::UIBlurManager::GetSingleton()->IncrementBlurCount();
        } else {
            RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
        }
    }

    if (currentState == State::Unlocked) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
}

LRESULT UI::WndProcHook::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KILLFOCUS) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
    return func(hWnd, uMsg, wParam, lParam);
}

void UI::D3DInitHook::thunk() {
    logger::debug("[D3DInitHook] START");

    originalFunction();

    const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
    if (!renderer) {
        SKSE::log::error("couldn't find renderer");
        return;
    }

    const auto swapChain = (IDXGISwapChain*)renderer->data.renderWindows[0].swapChain;
    if (!swapChain) {
        SKSE::log::error("couldn't find swapChain");
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
        SKSE::log::error("IDXGISwapChain::GetDesc failed.");
        return;
    }
    const auto device = (ID3D11Device*)renderer->data.forwarder;
    const auto context = (ID3D11DeviceContext*)renderer->data.context;

    SKSE::log::info("Initializing ImGui...");

    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;
    io.MouseDrawCursor = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
        SKSE::log::error("ImGui initialization failed (Win32)");
        return;
    }

    if (!ImGui_ImplDX11_Init(device, context)) {
        SKSE::log::error("ImGui initialization failed (DX11)");
        return;
    }

    Renderer::initialized.store(true);

    SKSE::log::info("ImGui initialized.");

    WndProcHook::func = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrA(desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook::thunk)));
    if (!WndProcHook::func) {
        SKSE::log::error("SetWindowLongPtrA failed!");
    }

    if (Config::MenuStyle == MenuStyle::Skyrim) {
        TransparentStyle();
    } else if (Config::MenuStyle == MenuStyle::Modern) {
        ModernStyle();
    }

    auto regular = LoadFontAwesome(io, 32.0f);
    io.FontDefault = regular.defaultFont;

    fontSizes["Big"] = LoadFontAwesome(io, 64.0f);
    fontSizes["Small"] = LoadFontAwesome(io, 16.0f);
    fontSizes["Default"] = regular; 

    io.Fonts->Build();


    logger::debug("[D3DInitHook] FINISH");
}
void UI::DXGIPresentHook::thunk(std::uint32_t a_timer) {


    originalFunction(a_timer);

    if (!Renderer::initialized.load()) {
        return;
    }

    if (!IsAnyWindowOpen()) {
        GameLock::SetState(GameLock::State::Unlocked);
        return;
    }
    GameLock::SetState(GameLock::State::Locked);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    {
        // trick imgui into rendering at game's real resolution (ie. if upscaled with Display Tweaks)
        static const auto screenSize = RE::BSGraphics::Renderer::GetScreenSize();

        auto& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(screenSize.width);
        io.DisplaySize.y = static_cast<float>(screenSize.height);
    }
    ImGui::NewFrame();
    Renderer::RenderWindows();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    CleanFont();
}

bool ProcessOpenClose(RE::InputEvent* const* evns) {
    if (!*evns) return false;

    for (RE::InputEvent* e = *evns; e; e = e->next) {
        if (e->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) continue;
        const RE::ButtonEvent* a_event = e->AsButtonEvent();
        const auto temp_device = a_event->GetDevice();
        if (!IsSupportedDevice(temp_device)) continue;
        const auto temp_toggleKey = temp_device == RE::INPUT_DEVICE::kKeyboard ? Config::ToggleKey : Config::ToggleKeyGamePad;
        if (a_event->GetIDCode() == temp_toggleKey) {

            if (UI::MainInterface->IsOpen.load() && a_event->IsDown()) {
                UI::MainInterface->IsOpen = false;
            } else {

                if (temp_device == RE::INPUT_DEVICE::kKeyboard) {
                    if (a_event->IsDown()) DoublePressDetectorKeyboard.press();

                    if (Config::ToggleMode == 0 && a_event->IsDown() ||
                        Config::ToggleMode == 1 && a_event->HeldDuration() > 0.2f && a_event->IsUp() ||
                        Config::ToggleMode == 2 && DoublePressDetectorKeyboard && a_event->IsDown()) {
                        UI::MainInterface->IsOpen = true;
                        return true;
                    };
                } else {
                    if (a_event->IsDown()) DoublePressDetectorGamepad.press();
                    if (Config::ToggleModeGamePad == 0 && a_event->IsDown() ||
                        Config::ToggleModeGamePad == 1 && a_event->HeldDuration() > 0.2f && a_event->IsUp() ||
                        Config::ToggleModeGamePad == 2 && DoublePressDetectorGamepad && a_event->IsDown()) {
                        UI::MainInterface->IsOpen = true;
                        return true;
                    };
                }

            }
        }
        if (a_event->GetIDCode() == REX::W32::DIK_ESCAPE && temp_device == RE::INPUT_DEVICE::kKeyboard) {
            bool hasChanged = UI::MainInterface->IsOpen.load();
            UI::MainInterface->IsOpen = false;
            return hasChanged;
        }
    }
    return false;
}

void UI::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                      RE::InputEvent* const* a_event) {
    bool isInputCapturedByOpenClose = ProcessOpenClose(a_event);
    if (isInputCapturedByOpenClose) {
        constexpr RE::InputEvent* const dummy[] = {nullptr};
        originalFunction(a_dispatcher, dummy);
    } else {
        if (IsAnyWindowOpen()) {
            constexpr RE::InputEvent* const dummy[] = {nullptr};
            originalFunction(a_dispatcher, dummy);
            TranslateInputEvent(a_event);
        } else {
            originalFunction(a_dispatcher, a_event);
        }
    }

}

HookBuilder* UI::Renderer::GetBuilder() {
    auto builder = new HookBuilder();
    builder->AddCall<D3DInitHook, 5, 14>(75595, 0x9, 77226, 0x275, 0xDC5530, 0x9);
    builder->AddCall<DXGIPresentHook, 5, 14>(75461, 0x9, 77246, 0x9, 0xDBBDD0, 0x15);
    builder->AddCall<ProcessInputQueueHook, 5, 14>(67315, 0x7B, 68617, 0x7B, 0xC519E0, 0x81);
    return builder;
}

void UI::Renderer::RenderWindows() {
    for (const auto window : Windows) {
        if (window->Interface->IsOpen) {
            window->Render();
        }
    }
}

bool UI::IsAnyWindowOpen() {
    auto it = std::find_if(Windows.begin(), Windows.end(), [](UI::Window* x) { return x->Interface->IsOpen.load(); });
    return it != Windows.end();
}

ImFont* GetFont(ImGuiIO& io, std::string name, float size,
                const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL) {
    std::string path = "Data/SKSE/Plugins/Fonts/" + name;
    if (std::filesystem::exists(path)) {
        return io.Fonts->AddFontFromFileTTF(path.c_str(), size, font_cfg, glyph_ranges);
    }
    return nullptr;
}

#define ICON_MIN_FA 0xe005
#define ICON_MAX_FA 0xf8ff

UI::FontContainer UI::LoadFontAwesome(ImGuiIO& io, float size) {

    auto result = FontContainer();

    if (auto skyrimFont = GetFont(io, "SkyrimMenuFont.ttf", size)) {
        result.defaultFont = skyrimFont;
    }

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;

    if (auto regularFont = GetFont(io, "SkyrimMenuFont.ttf", size, NULL, io.Fonts->GetGlyphRangesDefault())) {
        if (GetFont(io, "fa-regular-400.ttf", size, &icons_config, icons_ranges)) {
            result.faRegular = regularFont;
        }
    }
    if (auto solidFont = GetFont(io, "SkyrimMenuFont.ttf", size, NULL, io.Fonts->GetGlyphRangesDefault())) {
        if (GetFont(io, "fa-solid-900.ttf", size, &icons_config, icons_ranges)) {
            result.faSolid = solidFont;
        }
    }
    if (auto brandsFont = GetFont(io, "SkyrimMenuFont.ttf", size, NULL, io.Fonts->GetGlyphRangesDefault())) {
        if (GetFont(io, "fa-brands-400.ttf", size, &icons_config, icons_ranges)) {
            result.faBrands = brandsFont;
        }
    }

    return result;
}
void UI::CleanFontStack() {
    auto ctx = ImGui::GetCurrentContext();
    while (ctx->FontStack.size() > 0) {
        ImGui::PopFont();
    }
}
void UI::CleanFont() {
    CleanFontStack();
    UI::currentFont = UI::Font::fontSizeDefault;
}

void UI::SetFont(UI::Font font) {
    UI::currentFont = font;
    ProcessFont();
}

void UI::ProcessFont() {
    UI::FontContainer container;
    if (UI::currentFont & UI::Font::fontSizeSmall) 
    {
        container = UI::fontSizes["Small"];
    } 
    else if (UI::currentFont & UI::Font::fontSizeBig) 
    {
        container = UI::fontSizes["Big"];
    } 
    else if (UI::currentFont & UI::Font::fontSizeDefault) 
    {
        container = UI::fontSizes["Default"];
    }
    if (UI::currentFont & UI::Font::faSolid) 
    {
        if (container.faSolid) 
        {
            ImGui::PushFont(container.faSolid);
        }
    } 
    else if (UI::currentFont & UI::Font::faRegular) 
    {
        if (container.faRegular) 
        {
            ImGui::PushFont(container.faRegular);
        }
    } 
    else if (UI::currentFont & UI::Font::faBrands) 
    {
        if (container.faBrands) 
        {
            ImGui::PushFont(container.faBrands);
        }
    }
}

void UI::ModernStyle() {
    // Assuming you have set up Dear ImGui properly in your application
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;


    style.WindowRounding = 0.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.ChildRounding = 2.0f;
    style.PopupRounding = 2.0f;

    style.WindowBorderSize = 3.0f;
    style.ChildBorderSize = 3.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 3.0f;
    style.TabBarBorderSize = 3.0f;
    style.TabBorderSize = 3.0f;

    style.FramePadding = ImVec2(10.0f, 5.0f);

    colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.94f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 0.75f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.75f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void UI::TransparentStyle() {
    // Assuming you have set up Dear ImGui properly in your application
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;


    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;

    style.WindowBorderSize = 3.0f;
    style.ChildBorderSize = 3.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 3.0f;
    style.TabBarBorderSize = 3.0f;
    style.TabBorderSize = 3.0f;


    style.FramePadding = ImVec2(10.0f, 5.0f);

    colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_TableHeaderBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.0f,0.0f, 0.0f, 0.6f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);

    colors[ImGuiCol_Button] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);
    
    colors[ImGuiCol_Header] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
    colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);

    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_BorderShadow] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);

    colors[ImGuiCol_Separator] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_Tab] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_TabActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_TabUnfocused] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 1.0f, 1.0f, 8.0f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);

    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
}

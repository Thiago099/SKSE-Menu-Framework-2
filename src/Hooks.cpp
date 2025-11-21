#include "Hooks.h"
#include "Renderer.h"
#include "FontManager.h"
#include "StyleManager.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "WindowManager.h"

void Hooks::Install() {
    D3DInitHook::install();
    DXGIPresentHook::install();
    ProcessInputQueueHook::install();
}

void Hooks::D3DInitHook::install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(
        REL::RelocationID(75595, 77226, 0xDC5530).address() + REL::Relocate(0x9, 0x275, 0x9), thunk);
}

void Hooks::DXGIPresentHook::install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(
        REL::RelocationID(75461, 77246, 0xDBBDD0).address() + REL::Relocate(0x9, 0x9, 0x15), thunk);
}

void Hooks::ProcessInputQueueHook::install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(
        REL::RelocationID(67315, 68617, 0xC519E0).address() + REL::Relocate(0x7B, 0x7B, 0x81), thunk);
}

void Hooks::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                      RE::InputEvent* const* a_event) {
    bool isInputCapturedByOpenClose = UI::Renderer::ProcessOpenClose(a_event);
    if (isInputCapturedByOpenClose) {
        constexpr RE::InputEvent* const dummy[] = {nullptr};
        originalFunction(a_dispatcher, dummy);
    } else {
        if (WindowManager::IsAnyWindowOpen()) {
            constexpr RE::InputEvent* const dummy[] = {nullptr};
            originalFunction(a_dispatcher, dummy);
            UI::TranslateInputEvent(a_event);
        } else {
            originalFunction(a_dispatcher, a_event);
        }
    }
}

LRESULT Hooks::WndProcHook::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KILLFOCUS) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
    return func(hWnd, uMsg, wParam, lParam);
}

void Hooks::D3DInitHook::thunk() {
    logger::debug("[D3DInitHook] START");

    originalFunction();

    const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
    if (!renderer) {
        SKSE::log::error("couldn't find renderer");
        return;
    }
    auto data = renderer->GetRuntimeData();
    const auto swapChain = reinterpret_cast<IDXGISwapChain*>(data.renderWindows[0].swapChain);
    if (!swapChain) {
        SKSE::log::error("couldn't find swapChain");
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
        SKSE::log::error("IDXGISwapChain::GetDesc failed.");
        return;
    }
    const auto device = reinterpret_cast<ID3D11Device*>(data.forwarder);
    const auto context = reinterpret_cast<ID3D11DeviceContext*>(data.context);

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

    UI::Renderer::initialized.store(true);

    SKSE::log::info("ImGui initialized.");

    WndProcHook::func = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrA(desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook::thunk)));
    if (!WndProcHook::func) {
        SKSE::log::error("SetWindowLongPtrA failed!");
    }

    if (Config::MenuStyle == MenuStyle::Skyrim) {
        StyleManager::TransparentStyle();
    } else if (Config::MenuStyle == MenuStyle::Modern) {
        StyleManager::ModernStyle();
    }

    auto regular = FontManager::LoadFonts(io, 32.0f);
    io.FontDefault = regular.defaultFont;

    FontManager::fontSizes["Big"] = FontManager::LoadFonts(io, 64.0f);
    FontManager::fontSizes["Small"] = FontManager::LoadFonts(io, 16.0f);
    FontManager::fontSizes["Default"] = regular;

    io.Fonts->Build();

    logger::debug("[D3DInitHook] FINISH");
}

void Hooks::DXGIPresentHook::thunk(std::uint32_t a_timer) {
    originalFunction(a_timer);

    if (!UI::Renderer::initialized.load()) {
        return;
    }

    if (!WindowManager::IsAnyWindowOpen()) {
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
    UI::Renderer::RenderWindows();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    FontManager::CleanFont();
}
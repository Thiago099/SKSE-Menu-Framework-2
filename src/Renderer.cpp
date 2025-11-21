#include "Renderer.h"
#include "WindowManager.h"
#include "Config.h"
#include "Input.h"
#include "imgui_impl_dx11.h"
#include "Application.h"
#include "Input.h"
#include "Model.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "dxgi.h"

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


bool UI::Renderer::ProcessOpenClose(RE::InputEvent* const* evns) {
    if (!*evns) return false;

    for (RE::InputEvent* e = *evns; e; e = e->next) {
        if (e->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) continue;
        const RE::ButtonEvent* a_event = e->AsButtonEvent();
        const auto temp_device = a_event->GetDevice();
        if (!IsSupportedDevice(temp_device)) continue;
        const auto temp_toggleKey = temp_device == RE::INPUT_DEVICE::kKeyboard ? Config::ToggleKey : Config::ToggleKeyGamePad;
        if (a_event->GetIDCode() == temp_toggleKey) {

            if (WindowManager::MainInterface->IsOpen.load() && a_event->IsDown()) {
                WindowManager::MainInterface->IsOpen = false;
            } else {

                if (temp_device == RE::INPUT_DEVICE::kKeyboard) {
                    if (a_event->IsDown()) DoublePressDetectorKeyboard.press();

                    if (Config::ToggleMode == 0 && a_event->IsDown() ||
                        Config::ToggleMode == 1 && a_event->HeldDuration() > 0.4f||
                        Config::ToggleMode == 2 && DoublePressDetectorKeyboard && a_event->IsDown()) {
                        WindowManager::MainInterface->IsOpen = true;
                        return true;
                    };
                } else {
                    if (a_event->IsDown()) DoublePressDetectorGamepad.press();
                    if (Config::ToggleModeGamePad == 0 && a_event->IsDown() ||
                        Config::ToggleModeGamePad == 1 && a_event->HeldDuration() > 0.4f ||
                        Config::ToggleModeGamePad == 2 && DoublePressDetectorGamepad && a_event->IsDown()) {
                        WindowManager::MainInterface->IsOpen = true;
                        return true;
                    };
                }

            }
        }
        if (a_event->GetIDCode() == REX::W32::DIK_ESCAPE && temp_device == RE::INPUT_DEVICE::kKeyboard) {
            bool hasChanged = WindowManager::MainInterface->IsOpen.load();
            WindowManager::MainInterface->IsOpen = false;
            return hasChanged;
        }
    }
    return false;
}



void UI::Renderer::RenderWindows() {
    for (const auto window : WindowManager::Windows) {
        if (window->Interface->IsOpen) {
            window->Render();
        }
    }
}

void UI::Renderer::install() {}


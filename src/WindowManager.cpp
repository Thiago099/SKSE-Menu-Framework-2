#include "WindowManager.h"

Window::Window() {
	Interface = new WindowInterface;
    Render = nullptr;
}


bool WindowManager::IsAnyWindowOpen() {
    auto it = std::find_if(WindowManager::Windows.begin(), WindowManager::Windows.end(),
                           [](Window* x) { return x->Interface->IsOpen.load(); });
    return it != WindowManager::Windows.end();
}
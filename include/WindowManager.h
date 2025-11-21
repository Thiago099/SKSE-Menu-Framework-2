#pragma once


typedef void(__stdcall* RenderFunction)();

class WindowInterface {
public:
    std::atomic<bool> IsOpen{false};
};

class Window {
public:
    Window();
    WindowInterface* Interface;
    RenderFunction Render;
};


class WindowManager {
public:
    static inline std::vector<Window*> Windows;
    static inline WindowInterface* MainInterface;
    static bool IsAnyWindowOpen();
};
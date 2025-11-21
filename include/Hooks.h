#pragma once
namespace Hooks {

    void Install();

    struct WndProcHook {
        static LRESULT thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static inline WNDPROC func;
    };

    struct D3DInitHook {
        static void thunk();
        static void install();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct DXGIPresentHook {
        static void thunk(std::uint32_t a_timer);
        static void install();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct ProcessInputQueueHook {
        static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);
        static void install();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

}

#pragma once
#include "Config.h"
#include "Input.h"


namespace GameLock {
    enum State { None, Locked, Unlocked };
    extern State lastState;
    void SetState(State currentState);
}

namespace UI {

    class Renderer {
    public:
        static void RenderWindows();
        static void install();
        static bool ProcessOpenClose(RE::InputEvent* const* evns);
        static inline std::atomic<bool> initialized{false};
    };



}




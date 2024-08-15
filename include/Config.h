#pragma once
#include "Application.h"

enum MenuStyle {
    Skyrim,
    Modern,
    Classic
};

class Config {
    public:
    static void Init();
    static unsigned int ToggleKey;
    static unsigned int ToggleKeyGP;  // Gamepad
    static unsigned int ToggleMode;  // 0 = Toggle, 1 = Hold, 2 = Double Press
    static bool FreezeTimeOnMenu;
    static bool BlurBackgroundOnMenu;
    static MenuStyle MenuStyle;
};
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
    static uint8_t ToggleMode;
    static unsigned int ToggleKeyGamePad;
    static uint8_t ToggleModeGamePad;
    static bool FreezeTimeOnMenu;
    static bool BlurBackgroundOnMenu;
    static MenuStyle MenuStyle;
};
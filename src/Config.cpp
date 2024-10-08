#include "Config.h"

unsigned int Config::ToggleKey = 0x3B;
uint8_t Config::ToggleMode = 0;
unsigned int Config::ToggleKeyGamePad = 0;
uint8_t Config::ToggleModeGamePad = 0;
bool Config::FreezeTimeOnMenu = true;
MenuStyle Config::MenuStyle = MenuStyle::Skyrim;
bool Config::BlurBackgroundOnMenu = true;

char* toUpperCase(const char* str) {
    if (str == nullptr) {
        return nullptr;
    }

    char* upper_str = new char[strlen(str) + 1];
    if (upper_str == nullptr) {
        return nullptr; 
    }
    for (int i = 0; str[i] != '\0'; i++) {
        upper_str[i] = std::toupper((unsigned char)str[i]);
    }
    upper_str[strlen(str)] = '\0';

    return upper_str;
}

void Config::Init() {
	const auto ini = new Utils::Ini("SKSEMenuFramework.ini");
    ini->SetSection("General");

    ToggleKey = GetKeyBinding(ini->GetString("ToggleKey", "f1"));
    ToggleMode = GetToggleMode(ini->GetString("ToggleMode", "SinglePress"));

    ToggleKeyGamePad = GetKeyBinding(ini->GetString("ToggleKeyGamePad", ""),RE::INPUT_DEVICE::kGamepad);
    ToggleModeGamePad = GetToggleMode(ini->GetString("ToggleModeGamePad", "DoublePress"));

    FreezeTimeOnMenu = ini->GetBool("FreezeTimeOnMenu", true);
    BlurBackgroundOnMenu = ini->GetBool("BlurBackgroundOnMenu", true);
    auto menuStyleStr = toUpperCase(ini->GetString("MenuStyle", "SKYRIM"));

    if (strcmp(menuStyleStr, "MODERN") == 0) {
        Config::MenuStyle = MenuStyle::Modern;
    } else if(strcmp(menuStyleStr, "DEFAULT") == 0) {
        Config::MenuStyle = MenuStyle::Classic;
    }

    delete ini;
    delete menuStyleStr;
}
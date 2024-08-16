#pragma once
#include "Model.h"
#include "SimpleIni.h"
#include "combaseapi.h"
#include "Renderer.h"



std::vector<std::string> SplitString(const std::string& input, char delimiter);

uint8_t GetToggleMode(std::string input);

int GetKeyBinding(std::string input, RE::INPUT_DEVICE device = RE::INPUT_DEVICE::kKeyboard);
namespace Utils {
    class Ini {
    private:
        CSimpleIniA _ini;
        const char* _section;
        bool opened;
    public:
        Ini(const char* filaname) {
            _ini.SetUnicode();
            opened = _ini.LoadFile(("Data/SKSE/Plugins/" + std::string(filaname)).c_str()) >= 0;
        }
        void SetSection(const char* section) { _section = section; }
        const char* GetString(const char* key, const char* def = "") const { 
            if (!opened) return def;
            return _ini.GetValue(_section, key, def); 
        }
        float GetFloat(const char* key, float def = 0.0f) const {
            if (!opened) return def;
            return static_cast<float>(_ini.GetDoubleValue(_section, key, def));
        }
        int GetInt(const char* key, int def = 0) const { 
            if (!opened) return def;
            return static_cast<int>(_ini.GetLongValue(_section, key, def));
        }
        bool GetBool(const char* key, bool def = false) const {
            if (!opened) return def;
            return _ini.GetBoolValue(_section, key, def); 
        }
    };
}

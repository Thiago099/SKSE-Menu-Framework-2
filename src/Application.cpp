#include "Application.h"


std::vector<std::string> SplitString(const std::string& input, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(input);
    std::string part;

    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}

int GetKeyBinding(std::string input) {

    std::transform(input.begin(), input.end(), input.begin(), [](char c) { return static_cast<char>(std::toupper(c)); });

    const std::unordered_map<std::string, int> keymap = {
        {"NONE", 0x00},
        {"ESCAPE", 0x01},
        {"1", 0x02},
        {"2", 0x03},
        {"3", 0x04},
        {"4", 0x05},
        {"5", 0x06},
        {"6", 0x07},
        {"7", 0x08},
        {"8", 0x09},
        {"9", 0x0A},
        {"0", 0x0B},
        {"MINUS", 0x0C},
        {"EQUALS", 0x0D},
        {"BACKSPACE", 0x0E},
        {"TAB", 0x0F},
        {"Q", 0x10},
        {"W", 0x11},
        {"E", 0x12},
        {"R", 0x13},
        {"T", 0x14},
        {"Y", 0x15},
        {"U", 0x16},
        {"I", 0x17},
        {"O", 0x18},
        {"P", 0x19},
        {"BRACKETLEFT", 0x1A},
        {"BRACKETRIGHT", 0x1B},
        {"ENTER", 0x1C},
        {"LEFTCONTROL", 0x1D},
        {"A", 0x1E},
        {"S", 0x1F},
        {"D", 0x20},
        {"F", 0x21},
        {"G", 0x22},
        {"H", 0x23},
        {"J", 0x24},
        {"K", 0x25},
        {"L", 0x26},
        {"SEMICOLON", 0x27},
        {"APOSTROPHE", 0x28},
        {"TILDE", 0x29},
        {"LEFTSHIFT", 0x2A},
        {"BACKSLASH", 0x2B},
        {"Z", 0x2C},
        {"X", 0x2D},
        {"C", 0x2E},
        {"V", 0x2F},
        {"B", 0x30},
        {"N", 0x31},
        {"M", 0x32},
        {"COMMA", 0x33},
        {"PERIOD", 0x34},
        {"SLASH", 0x35},
        {"RIGHTSHIFT", 0x36},
        {"KP_MULTIPLY", 0x37},
        {"LEFTALT", 0x38},
        {"SPACEBAR", 0x39},
        {"CAPSLOCK", 0x3A},
        {"F1", 0x3B},
        {"F2", 0x3C},
        {"F3", 0x3D},
        {"F4", 0x3E},
        {"F5", 0x3F},
        {"F6", 0x40},
        {"F7", 0x41},
        {"F8", 0x42},
        {"F9", 0x43},
        {"F10", 0x44},
        {"NUMLOCK", 0x45},
        {"SCROLLLOCK", 0x46},
        {"KP_7", 0x47},
        {"KP_8", 0x48},
        {"KP_9", 0x49},
        {"KP_SUBTRACT", 0x4A},
        {"KP_4", 0x4B},
        {"KP_5", 0x4C},
        {"KP_6", 0x4D},
        {"KP_PLUS", 0x4E},
        {"KP_1", 0x4F},
        {"KP_2", 0x50},
        {"KP_3", 0x51},
        {"KP_0", 0x52},
        {"KP_DECIMAL", 0x53},
        {"F11", 0x57},
        {"F12", 0x58},
        {"KP_ENTER", 0x9C},
        {"RIGHTCONTROL", 0x9D},
        {"KP_DIVIDE", 0xB5},
        {"PRINTSCREEN", 0xB7},
        {"RIGHTALT", 0xB8},
        {"PAUSE", 0xC5},
        {"HOME", 0xC7},
        {"UP", 0xC8},
        {"PAGEUP", 0xC9},
        {"LEFT", 0xCB},
        {"RIGHT", 0xCD},
        {"END", 0xCF},
        {"DOWN", 0xD0},
        {"PAGEDOWN", 0xD1},
        {"INSERT", 0xD2},
        {"DELETE", 0xD3},
        {"LEFTWIN", 0xDB},
        {"RIGHTWIN", 0xDC},
    };

    auto it = keymap.find(input);
    if (it != keymap.end()) {
        return it->second;
    } else {
        return 0x0;
    }
}

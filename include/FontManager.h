#pragma once

#include "imgui_internal.h"

enum Font {
    none = 0,
    faSolid = 1 << 0,
    faRegular = 1 << 1,
    faBrands = 1 << 2,
    fontSizeSmall = 1 << 3,
    fontSizeDefault = 1 << 4,
    fontSizeBig = 1 << 5,
};

struct FontContainer {
    ImFont* faSolid;
    ImFont* faRegular;
    ImFont* faBrands;
    ImFont* defaultFont;
};

class FontManager {
public:
    static void ProcessFont();
    static ImFont* GetFont(ImGuiIO& io, std::string name, float size, const ImFontConfig* font_cfg,
                    const ImWchar* glyph_ranges);
    static void SetFont(Font font);
    static FontContainer LoadFonts(ImGuiIO& io, float size);
    static void CleanFontStack();
    static void CleanFont();
    static inline std::map<std::string, FontContainer> fontSizes;
    static inline Font currentFont = Font::fontSizeDefault;
    static inline std::map<float, ImVector<ImWchar>> persistentGlyphRanges;
};


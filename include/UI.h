#pragma once

#include "WindowManager.h"

namespace UI {
    class MenuTree {
    public:
        std::map<std::string, MenuTree*> Children;
        std::vector<std::pair<const std::string, MenuTree*>> SortedChildren;
        RenderFunction Render;
        std::string Title;
    };
    extern UI::MenuTree* RootMenu;
    void __stdcall RenderMenuWindow();
    void AddToTree(UI::MenuTree* node, std::vector<std::string>& path, RenderFunction render, std::string title);
}
#include "SKSEMenuFramework.h"
#include "FontManager.h"
#include <imgui.h>
#include "Application.h"
#include "Renderer.h"
#include "UI.h"
void AddSectionItem(const char* path, RenderFunction rendererFunction) { 
    auto pathSplit = SplitString(path, '/');
    AddToTree(UI::RootMenu, pathSplit, rendererFunction, pathSplit.back());
}

WindowInterface* AddWindow(RenderFunction rendererFunction) { 

    auto newWindow = new Window();

    newWindow->Render = rendererFunction;

    WindowManager::Windows.push_back(newWindow);

    return newWindow->Interface;

}

void PushDefault() 
{
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::fontSizeBig);
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::fontSizeSmall);
    FontManager::currentFont = (Font)(FontManager::currentFont | Font::fontSizeDefault);
    FontManager::ProcessFont();
}
void PushBig() 
{
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::fontSizeDefault);
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::fontSizeSmall);
    FontManager::currentFont = (Font)(FontManager::currentFont | Font::fontSizeBig);
    FontManager::ProcessFont();
}
void PushSmall() 
{
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::fontSizeDefault);
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::fontSizeBig);
    FontManager::currentFont = (Font)(FontManager::currentFont | Font::fontSizeSmall);
    FontManager::ProcessFont();
}

void PushSolid() 
{
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::faBrands);
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::faRegular);
    FontManager::currentFont = (Font)(FontManager::currentFont | Font::faSolid);
    FontManager::ProcessFont();
}

void PushRegular() 
{
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::faSolid);
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::faBrands);
    FontManager::currentFont = (Font)(FontManager::currentFont | Font::faRegular);
    FontManager::ProcessFont();
}

void PushBrands() 
{
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::faSolid);
    FontManager::currentFont = (Font)(FontManager::currentFont & ~Font::faRegular);
    FontManager::currentFont = (Font)(FontManager::currentFont | Font::faBrands);
    FontManager::ProcessFont();
}

void Pop() { FontManager::CleanFont(); }
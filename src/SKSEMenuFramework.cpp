#include "SKSEMenuFramework.h"


void AddSectionItem(const char* path, UI::RenderFunction rendererFunction) { 
    auto pathSplit = SplitString(path, '/');
    AddToTree(UI::RootMenu, pathSplit, rendererFunction, pathSplit.back());
}

UI::WindowInterface* AddWindow(UI::RenderFunction rendererFunction) { 

    auto newWindow = new UI::Window();

    newWindow->Render = rendererFunction;

    UI::Windows.push_back(newWindow);

    return newWindow->Interface;

}

void ProcessFont() {
    UI::FontContainer container;
    bool regular = false;
    if (UI::currentFont & UI::Font::fontSizeSmall) 
    {
        container = UI::fontSizes["Small"];
    } 
    else if (UI::currentFont & UI::Font::fontSizeBig) 
    {
        container = UI::fontSizes["Big"];
    } 
    else if (UI::currentFont & UI::Font::fontSizeDefault) 
    {
        regular = true;
        container = UI::fontSizes["Default"];
    }
    if (UI::currentFont & UI::Font::faSolid) 
    {

        if (container.faSolid){
            ImGui::PushFont(container.faSolid);
        }
    } 
    else if (UI::currentFont & UI::Font::faRegular) 
    {
        if (container.faRegular) {
            ImGui::PushFont(container.faRegular);
        }
    }
    else if (UI::currentFont & UI::Font::faBrands) 
    {
        if (container.faBrands) {
            ImGui::PushFont(container.faBrands);
        }
    } 
    else 
    {
        if (regular) {
            Pop();
        }
        else if (container.defaultFont) {
            ImGui::PushFont(container.defaultFont);
        }
    }
}

void PushDefault() {
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeBig);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeSmall);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::fontSizeDefault);
    ProcessFont();
}
void PushBig() {
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeDefault);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeSmall);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::fontSizeBig);
    ProcessFont();
}
void PushSmall() {
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeDefault);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeBig);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::fontSizeSmall);
    ProcessFont();
}

void PushSolid() {
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faBrands);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faRegular);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::faSolid);
    ProcessFont();
}

void PushRegular() { 
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faSolid);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faBrands);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::faRegular);
    ProcessFont();
}

void PushBrands() {
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faSolid);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faRegular);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::faBrands);
    ProcessFont();
}

void Pop() {
    auto ctx = ImGui::GetCurrentContext();
    ctx->FontStack.clear();
    UI::currentFont = UI::Font::none;
    ImGui::SetCurrentFont(ImGui::GetDefaultFont());
}
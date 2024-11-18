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

unsigned long WindowAddOpenCloseEvent(UI::WindowInterface* window, UI::OpenCloseCallback callback) { 
    for (auto win : UI::Windows) {
        if (win->Interface == window) {
            auto id = win->open_close_event_id++;
            win->OpenCloseEvent[id] = callback;
            return id;
        }
    }
    return 0;
}

void WindowRemoveOpenCloseEvent(UI::WindowInterface* window, long id) {
    for (auto win : UI::Windows) {
        if (win->Interface == window) {
            auto it = win->OpenCloseEvent.find(id);

            if (it != win->OpenCloseEvent.end()) {
                win->OpenCloseEvent.erase(it);
            }

            return;
        }
    }
}

void WindowOpen(UI::WindowInterface* window) {
    for (auto win : UI::Windows) {
        if (win->Interface == window) {
            win->Open();
            return;
        }
    }
}

void WindowClose(UI::WindowInterface* window) {
    for (auto win : UI::Windows) {
        if (win->Interface == window) {
            win->Close();
            return;
        }
    }
}



void PushDefault() 
{
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeBig);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeSmall);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::fontSizeDefault);
    UI::ProcessFont();
}
void PushBig() 
{
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeDefault);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeSmall);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::fontSizeBig);
    UI::ProcessFont();
}
void PushSmall() 
{
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeDefault);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::fontSizeBig);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::fontSizeSmall);
    UI::ProcessFont();
}

void PushSolid() 
{
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faBrands);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faRegular);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::faSolid);
    UI::ProcessFont();
}

void PushRegular() 
{
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faSolid);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faBrands);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::faRegular);
    UI::ProcessFont();
}

void PushBrands() 
{
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faSolid);
    UI::currentFont = (UI::Font)(UI::currentFont & ~UI::Font::faRegular);
    UI::currentFont = (UI::Font)(UI::currentFont | UI::Font::faBrands);
    UI::ProcessFont();
}

void Pop() { UI::CleanFont(); }
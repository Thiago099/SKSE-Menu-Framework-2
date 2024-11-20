#pragma once
#include <imgui.h>

#include "Application.h"
#include "Renderer.h"
#include "UI.h"
#define FUNCTION_PREFIX extern "C" [[maybe_unused]] __declspec(dllexport)

FUNCTION_PREFIX void AddSectionItem(const char* path, UI::RenderFunction rendererFunction);
FUNCTION_PREFIX UI::WindowInterface* AddWindow(UI::RenderFunction rendererFunction);
FUNCTION_PREFIX unsigned long WindowAddOpenCloseEvent(UI::WindowInterface* window, UI::OpenCloseCallback callback);
FUNCTION_PREFIX void WindowRemoveOpenCloseEvent(UI::WindowInterface* window, long id);

FUNCTION_PREFIX ImTextureID LoadTextureFromDDSFile(const wchar_t* path);
FUNCTION_PREFIX ImTextureID LoadTextureFromWICFile(const wchar_t* path);
FUNCTION_PREFIX void WindowOpen(UI::WindowInterface* window);
FUNCTION_PREFIX void WindowClose(UI::WindowInterface* window);
FUNCTION_PREFIX void PushBig();
FUNCTION_PREFIX void PushDefault();
FUNCTION_PREFIX void PushSmall();
FUNCTION_PREFIX void PushSolid();
FUNCTION_PREFIX void PushRegular();
FUNCTION_PREFIX void PushBrands();
FUNCTION_PREFIX void Pop();
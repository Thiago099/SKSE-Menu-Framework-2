#pragma once
#include <imgui.h>

#include "Application.h"
#include "Renderer.h"
#include "UI.h"

#define FUNCTION_PREFIX extern "C" [[maybe_unused]] __declspec(dllexport)

FUNCTION_PREFIX void AddSectionItem(const char* path, UI::RenderFunction rendererFunction);
FUNCTION_PREFIX UI::WindowInterface* AddWindow(UI::RenderFunction rendererFunction);
FUNCTION_PREFIX void PushSolid();
FUNCTION_PREFIX void PushRegular();
FUNCTION_PREFIX void PushBrands();
FUNCTION_PREFIX void Pop();
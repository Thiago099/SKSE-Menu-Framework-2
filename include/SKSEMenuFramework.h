#pragma once

#include "WindowManager.h"
#define FUNCTION_PREFIX extern "C" [[maybe_unused]] __declspec(dllexport)

FUNCTION_PREFIX void AddSectionItem(const char* path, RenderFunction rendererFunction);
FUNCTION_PREFIX WindowInterface* AddWindow(RenderFunction rendererFunction);
FUNCTION_PREFIX void PushBig();
FUNCTION_PREFIX void PushDefault();
FUNCTION_PREFIX void PushSmall();
FUNCTION_PREFIX void PushSolid();
FUNCTION_PREFIX void PushRegular();
FUNCTION_PREFIX void PushBrands();
FUNCTION_PREFIX void Pop();
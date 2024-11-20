#pragma once

#include <nlohmann/json.hpp>
#include "imgui.h"
#include <vector>
#include "Renderer.h"
#include "File.h"

using json = nlohmann::json;


struct Asset {
    float u0, v0, u1, v1;
};

struct Texture {
    ImTextureID texture;
    std::vector<Asset*> Assets;
};


namespace Textures {
    inline std::map<unsigned long, Texture*> textures;
    void Render(unsigned long idTexture, unsigned long idAsset, ImVec2 size, int color = 0xffffff, float alpha = 1);
    unsigned long LoadDDS(const wchar_t* imagePath, const wchar_t* uvwPath);
    unsigned long LoadWIC(const wchar_t* imagePath, const wchar_t* uvwPath);
}
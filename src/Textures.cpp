#include "Textures.h"


void LoadJsonFile(const wchar_t* uvwPath, Texture* texture) {
    if (!File::Exists(uvwPath)) {
        return;
    }
    std::ifstream file(uvwPath);
    if (!file.is_open()) {
        return; 
    }
    json js = json::parse(file);
    if (js.is_discarded()) {
        return;
    }
    for (auto& item : js) {

        if (!item.contains("u1") || !item.contains("v1") || !item.contains("u2") || !item.contains("v2")) {
            continue; 
        }

        auto asset = new Asset();

        asset->u0 = item["u1"].get<float>();
        asset->v0 = item["v1"].get<float>();
        asset->u1 = item["u2"].get<float>();
        asset->v1 = item["v2"].get<float>();

        texture->Assets.push_back(asset);
    }
}

unsigned long CreateTexture(ImTextureID texture, const wchar_t* uvwPath) {
    auto id = Textures::textures.size();
    auto result = new Texture();
    result->texture = texture;
    LoadJsonFile(uvwPath, result);
    Textures::textures[id] = result;
    return id;
}


ImVec4 ConvertHexToColor(uint32_t hexColor) {
    ImVec4 color;
    color.x = ((hexColor >> 16) & 0xFF) / 255.0f;  // Extract red
    color.y = ((hexColor >> 8) & 0xFF) / 255.0f;   // Extract green
    color.z = (hexColor & 0xFF) / 255.0f;          // Extract blue
    return color;
}
void Textures::Render(unsigned long idTexture, unsigned long idAsset, ImVec2 size, int color, float alpha) {
    if (idTexture >= textures.size()) {
        return;
    }

    auto texture = textures[idTexture];

    if (texture->texture == NULL) {
        return;
    }

    if (idAsset >= texture->Assets.size()) {
        return;
    }

    auto asset = texture->Assets[idAsset];

    ImVec2 uv0(asset->u0, asset->v0);
    ImVec2 uv1(asset->u1, asset->v1);
    auto c = ConvertHexToColor(color);
    c.w = alpha;
    ImGui::Image(texture->texture, size, uv0, uv1, c);
}

unsigned long Textures::LoadDDS(const wchar_t* imagePath, const wchar_t* uvwPath) {
    auto textureId = UI::D3DInitHook::LoadTextureFromDDSFile(imagePath);
    return CreateTexture(textureId, uvwPath);
}

unsigned long Textures::LoadWIC(const wchar_t* imagePath, const wchar_t* uvwPath) {
    auto textureId = UI::D3DInitHook::LoadTextureFromWICFile(imagePath);
    return CreateTexture(textureId, uvwPath);
}

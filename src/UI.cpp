#include "UI.h"

static ImGuiTextFilter filter;

UI::MenuTree* UI::RootMenu = new UI::MenuTree();


int frame = 0;

size_t item_current_idx = 0;
size_t node_id = 0;
UI::MenuTree* display_node;


static ImGuiTreeNodeFlags base_flags =
    ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
static int selection_mask = (1 << 2);

void DummyRenderer(std::pair<const std::string, UI::MenuTree*>& node) {
    ++node_id;
    for (auto& item : node.second->Children) {
        DummyRenderer(item);
    }
}

void RenderNode(std::pair<const std::string, UI::MenuTree*>& node) {
    ++node_id;
    ImGuiTreeNodeFlags node_flags = base_flags;
    // const bool is_selected = item_current_idx == i;
    if (item_current_idx == node_id) node_flags |= ImGuiTreeNodeFlags_Selected;

    if (node.second->Children.size() == 0) {
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)node_id, node_flags, node.first.c_str(), node_id);


    bool itemClicked = ImGui::IsItemClicked();
    bool itemToggledOpen = ImGui::IsItemToggledOpen();
    bool gamepadButtonPressed = ImGui::IsKeyPressed(ImGuiKey_GamepadFaceDown);  // Typically A button
    bool itemIsFocused = ImGui::IsItemFocused();  // Check if the item is focused/highlighted by gamepad navigation

    if ((itemClicked || (gamepadButtonPressed && itemIsFocused)) && !itemToggledOpen) {
        if (node.second->Render) {
            item_current_idx = node_id;
            display_node = node.second;
        }
    }
    if (node_open && node.second->Children.size() != 0) {
        for (auto& item : node.second->SortedChildren) {
            RenderNode(item);
        }
        ImGui::TreePop();
    } else {
        for (auto& item : node.second->Children) {
            DummyRenderer(item);
        }
    }
}

void __stdcall UI::RenderMenuWindow() {
    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
    ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.8f, viewport->Size.y * 0.8f}, ImGuiCond_Appearing);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Mod Control Panel", nullptr, window_flags);

    float offset1 = 20.0f;
    float offset2 = 20.0f;
    float offset3 = 0.0f;

    if (Config::MenuStyle != MenuStyle::Classic) {
        offset1 = 50.0f;
        offset2 = 41.0f;
        offset3 = 5.0f;
    }


    ImGui::BeginChild("TreeView2", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, offset1), ImGuiChildFlags_None,
                      window_flags);
    filter.Draw("##SKSEModControlPanelMenuFilter", -FLT_MIN);
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("SKSEModControlPanelModMenuHeader", ImVec2(0, offset2), ImGuiChildFlags_None, window_flags);
    if (display_node) {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(display_node->Title.c_str()).x;
        float offsetX = (windowWidth - textWidth) * 0.5f;
        ImGui::SetCursorPosX(offsetX);
        ImGui::SetCursorPosY(offset3);
        ImGui::Text(display_node->Title.c_str());
    } else {
        // TODO: Default title
    }
    ImGui::EndChild();

    ImGui::BeginChild("SKSEModControlPanelTreeView", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, -FLT_MIN),
                      ImGuiChildFlags_Border, window_flags);
    node_id = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
    for (const auto& item : RootMenu->Children) {
        if (filter.PassFilter(item.first.c_str()) &&
            (ImGui::CollapsingHeader(std::format("{}##{}", item.first, node_id).c_str()))) {
            for (auto node : item.second->SortedChildren) {
                RenderNode(node);

            }
        } else {
            for (auto node : item.second->Children) {
                DummyRenderer(node);
            }
        }
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("SKSEModControlPanelMenuNode", ImVec2(0, -FLT_MIN), ImGuiChildFlags_Border, window_flags);



    ImGui::EndChild();
    ImGui::End();
}

void UI::AddToTree(UI::MenuTree* node, std::vector<std::string>& path, UI::RenderFunction render, std::string title) {
    if (!path.empty()) {
        auto currentName = path.front();
        path.erase(path.begin());

        auto foundItem = node->Children.find(currentName);
        if (foundItem != node->Children.end()) {
            AddToTree(foundItem->second, path, render, title);
        } else {
            auto newItem = new UI::MenuTree();
            node->Children[currentName] = newItem;
            node->SortedChildren.push_back(std::pair<const std::string, UI::MenuTree*>(currentName, newItem));
            AddToTree(newItem, path, render, title);
        }
    } else {
        node->Render = render;
        node->Title = title;
    }
}


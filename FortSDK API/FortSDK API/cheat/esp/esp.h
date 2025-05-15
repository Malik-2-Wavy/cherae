#pragma once 
#include "../../util/util.h"
#include "../sdk/sdk.h"
#include "../sdk/offsets.hpp"
#include "drawing.hpp"

//Cracky

class visuals_class {
public:

	bool FullBox = true;

	bool FilledBox = false;

    bool WeaponName = false;

    bool userDistance = false;

    int renderDist = 250;

}; inline visuals_class visual;

class aimbot_class {
public:

    bool enable = true;

    bool showFOV = false;

    float fovvalue = 250;

    float smoothness = 5;

}; inline aimbot_class aimbot;

class entity_class {
public:

    std::string PlayerName;
    std::string WeaponName;

};

void ActorLoop() {


    cache::Gworld = read<uint64_t>(virtualaddy + Offsets::UWorld);
    cache::GameInstance = read<uint64_t>(cache::Gworld + Offsets::OwningGameInstance);
    cache::LocalPlayers = read<uint64_t>(read<uint64_t>(cache::GameInstance + Offsets::LocalPlayers));
    cache::PlayerController = read<uint64_t>(cache::LocalPlayers + Offsets::PlayerController);
    cache::LocalPawn = read<uint64_t>(cache::PlayerController + Offsets::AcknowledgedPawn);
    cache::RootComponent = read<uint64_t>(cache::LocalPawn + Offsets::RootComponent);
    cache::GameState = read<uintptr_t>(cache::Gworld + Offsets::GameState);
    cache::PlayerArray = read<uintptr_t>(cache::GameState + Offsets::PlayerArray);
    cache::PlayerArraySize = read<int>(cache::GameState + (Offsets::PlayerArray + sizeof(uintptr_t)));

    float closestDistance = FLT_MAX;
    Vector2 targetScreen{};
    Vector3 targetWorld{};

    for (int i = 0; i < cache::PlayerArraySize; i++) {

        uintptr_t PlayerState = read<uintptr_t>(cache::PlayerArray + (i * sizeof(uintptr_t)));
        if (!PlayerState) continue;

        uintptr_t Player = read<uintptr_t>(PlayerState + Offsets::PawnPrivate);
        if (!Player || Player == cache::LocalPawn) continue;

        uintptr_t Mesh = read<uintptr_t>(Player + Offsets::Mesh);
        if (!Mesh) continue;

        entity_class caching{};
        caching.PlayerName = PINFO.getPlayerName(PlayerState);
        caching.WeaponName = PINFO.getWeaponName(Player);

        Vector3 Head3D = GetBoneWithRotation(Mesh, 110);
        Vector2 Head2D = ProjectWorldToScreen({ Head3D.x, Head3D.y, Head3D.z + 20 });

        Vector3 Bottom3D = GetBoneWithRotation(Mesh, 0);
        Vector2 Bottom2D = ProjectWorldToScreen(Bottom3D);

        float BoxHeight = abs(Head2D.y - Bottom2D.y);
        float BoxWidth = BoxHeight * 0.35f;

        float X = Head2D.x - BoxWidth / 2;
        float Y = Head2D.y;

        const float distance = vCamera.Location.Distance(Bottom3D) / 100;
        if (distance > visual.renderDist && cache::LocalPawn) continue;

        bool isEnemyVis = utility.IsEnemyVisible(Mesh);
        ImColor espColor = isEnemyVis ? ImColor(46, 125, 50) : ImColor(198, 40, 40);

        if (visual.FullBox) {
            draw.DrawBox(X - 1, Y - 1, BoxWidth + 2, BoxHeight + 2, ImColor(0, 0, 0), 3.0f);
            draw.DrawBox(X + 1, Y + 1, BoxWidth - 2, BoxHeight - 2, espColor, 3.0f);

            if (visual.FilledBox) {
                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(X, Y),
                    ImVec2(X + BoxWidth, Y + BoxHeight),
                    ImColor(0, 0, 0, 65));
            }
        }

        if (visual.userDistance)
        {
            char display_name[64];
            snprintf(display_name, sizeof(display_name), "%s (%.1fm)", caching.PlayerName.c_str(), distance);

            ImVec2 text_size = ImGui::CalcTextSize(display_name);
            ImVec2 pos = ImVec2(Head2D.x - text_size.x / 2.0f, Head2D.y - 20.0f);

            ImDrawList* draw = ImGui::GetOverlayDrawList();

            draw->AddText(ImVec2(pos.x - 1, pos.y - 1), ImColor(0, 0, 0), display_name);
            draw->AddText(ImVec2(pos.x + 1, pos.y - 1), ImColor(0, 0, 0), display_name);
            draw->AddText(ImVec2(pos.x - 1, pos.y + 1), ImColor(0, 0, 0), display_name);
            draw->AddText(ImVec2(pos.x + 1, pos.y + 1), ImColor(0, 0, 0), display_name);

            draw->AddText(pos, ImColor(255, 255, 255), display_name);
        }

        if (visual.WeaponName)
        {
            std::string weapon;

            if (caching.WeaponName.empty())
            {
                weapon = "Building Plan";
            }
            else
            {
                weapon = caching.WeaponName;
            }

            ImVec2 text_size = ImGui::CalcTextSize(weapon.c_str());
            ImVec2 pos = ImVec2(Bottom2D.x - text_size.x / 2.0f, Bottom2D.y + 20.0f);

            ImDrawList* draw = ImGui::GetOverlayDrawList();

            draw->AddText(ImVec2(pos.x - 1, pos.y - 1), ImColor(0, 0, 0), weapon.c_str());
            draw->AddText(ImVec2(pos.x + 1, pos.y - 1), ImColor(0, 0, 0), weapon.c_str());
            draw->AddText(ImVec2(pos.x - 1, pos.y + 1), ImColor(0, 0, 0), weapon.c_str());
            draw->AddText(ImVec2(pos.x + 1, pos.y + 1), ImColor(0, 0, 0), weapon.c_str());

            draw->AddText(pos, ImColor(255, 255, 255), weapon.c_str());

        }

        if (isEnemyVis)
        {
            float dx = Head2D.x - (Width / 2.0f);
            float dy = Head2D.y - (Height / 2.0f);
            float distance2D = sqrtf(dx * dx + dy * dy);

            if (distance2D < closestDistance && distance2D < aimbot.fovvalue)
            {
                closestDistance = distance2D;
                targetScreen = Head2D;
                targetWorld = Head3D;
            }
        }
    }
    if (targetScreen.x > 0 && targetScreen.y > 0)
    {
        if (aimbot.enable && GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            float deltaX = targetScreen.x - (Width / 2.0f);
            float deltaY = targetScreen.y - (Height / 2.0f);

            float smooth = aimbot.smoothness;


            //mem::MoveMouse(static_cast<long>(deltaX / smooth), static_cast<long>(deltaY / smooth));
            //Use your own Aimbot method here watch my other Tutorial for that
        }
    }
}
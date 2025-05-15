#include "../../render/ImGui/imgui.h"
#include "drawing.hpp"

//Cracky

void drawing_class::DrawBox(int X, int Y, int W, int H, const ImColor color, float thickness) {

	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), color, 2.0f, thickness);
}
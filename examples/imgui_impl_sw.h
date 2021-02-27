
#pragma once

#include "../app/ImGuiApp.hpp"

IMGUI_IMPL_API bool     ImGui_ImplSW_Init(/*ID3D10Device* device*/);
IMGUI_IMPL_API void     ImGui_ImplSW_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplSW_NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_IMPL_API void     ImGui_ImplSW_InvalidateDeviceObjects();
IMGUI_IMPL_API bool     ImGui_ImplSW_CreateDeviceObjects();

IMGUI_IMPL_API void     ImGui_ImplSW_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplSW_DestroyFontTexture();

IMGUI_IMPL_API void     ImGui_ImplSW_BlockFinishDrawResources();


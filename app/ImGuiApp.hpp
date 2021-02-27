/// =============================================================================
/// LICENSE - MIT (https://opensource.org/licenses/MIT)
///
/// imgui (https://github.com/ocornut/imgui)
/// 
/// Copyright (c) 2014-2020 Omar Cornut
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// =============================================================================

#pragma once

//System Includes
#include <chrono>

#include "../../handycpp/Handy.hpp"
#include "../../imgui/imgui.h"
#include "../../imgui/misc/cpp/imgui_stdlib.h"
#include "ImExt.hpp"
#include "IconsFontAwesome5.h"
#include "IconsMaterialDesignIcons.h"

enum class RenderingAPI
{
    INVALID,
    Ask,

    GLFW,
    DX10,
    DX10W,
    DX11,
    DX11W,
    Emscripten,
    SoftwareRasterizer
};

enum class FrameSyncType
{
    INVALID,
    Dynamic,
    VSync,
    NoSync
};

inline std::string RenderingAPIToLabel(RenderingAPI value)
{
    switch (value)
    {
        case RenderingAPI::DX11:    return "DirectX11";
        case RenderingAPI::DX11W:   return "DirectX11 WARP";
        case RenderingAPI::DX10:    return "DirectX10";
        case RenderingAPI::DX10W:   return "DirectX10 WARP";
        case RenderingAPI::GLFW:    return "OpenGL";
        case RenderingAPI::SoftwareRasterizer: return "Software Rasterization";
        case RenderingAPI::Ask:     return "ASK";
        default:
        case RenderingAPI::INVALID: return "INVALID";
    }
}

inline std::string FrameSyncTypeToLabel(FrameSyncType value)
{
    switch (value)
    {
        case FrameSyncType::Dynamic: return "Dynamic";
        case FrameSyncType::VSync:   return "VSync";
        case FrameSyncType::NoSync:  return "No Sync";
        default:
        case FrameSyncType::INVALID: return "INVALID";
    }
}

/// Only used for Emscripten
struct AImageAsyncURLRequest
{
    std::string URL;

    virtual void OnSuccess(ImTextureID textureID) = 0;
    virtual void OnError() = 0;

    virtual ~AImageAsyncURLRequest() { };
};

class AppUI
{
public:
    bool DrawLoopEnabled = true;

    AppUI() = default;
    virtual ~AppUI() = default;

    virtual void Preframe()  = 0;//  { }
    virtual void Draw()      = 0;// {}
    virtual void Postframe() = 0;// {}

};



class Fonts
{
public:
    static ImFont * Tiny;
    static ImFont * Small;
    static ImFont * Normal;
    static ImFont * NormalBold;
    static ImFont * Large;
    static ImFont * Huge;

    static ImFont * Mono;
    //static ImFont * MonoUTF8;

    static ImFont * Default;

    static float Scaling;
    static float FontScaling;

    static void Build(float scaling, float fontScaling);
};

class ImGuiApp
{
    ImGuiApp() = default; // private ctor; access through Instance() method!

    RenderingAPI  m_api       = RenderingAPI ::INVALID;
    FrameSyncType m_frameSync = FrameSyncType::INVALID;
    bool          m_showMainWindow = true;

    AppUI *       m_ui = nullptr;

    bool          m_updateStyleForScale = true;
    float         m_scaling     = 1.0f;
    float         m_scalingFont = 1.0f;
	float         m_scalingDef  = 1.0f;

    void          initDX10();
    void          initDX11();
    void          initGL  ();
    void          initSW  ();

    void          deinitDX10();
    void          deinitDX11();
    void          deinitGL  ();
    void          deinitSW  ();

    void          mainDX10();
    void          mainDX11();
    void          mainGL  ();
    void          mainSW  ();

    bool          shouldVSync();

    int           swapInterval();

    void          draw();

    void          Style(float scaling = 1.0f);

    //Thread for asynchronous texture deletion
    std::thread              m_garbageBurner;
    std::mutex               m_garbageToBurnMutex;
    std::vector<std::tuple<ImTextureID, std::chrono::time_point<std::chrono::steady_clock>>> m_garbageToBurn; //<TexID, AllowedToDestroyAfterTimepoint>
    std::atomic<bool>        m_garbageBurnerExit = false;
    void                     GarbageBurnerMain();

public:



    static ImGuiApp & Instance() { static ImGuiApp app; return app; }
    COPY_ASSIGN_MOVE_CTOR(ImGuiApp, delete, delete, delete);

    ImVec4 ClearColor = ImVec4(0.440f, 0.440f, 0.440f, 1.000f);

    RenderingAPI  API()               const { return m_api; }
    bool          API_IsWarp()        const { return m_api == RenderingAPI::DX10W || m_api == RenderingAPI::DX11W; }
    FrameSyncType FrameSyncStrategy() const { return m_frameSync; }

    void   Init(RenderingAPI api, FrameSyncType syncStrategy, bool showMainWindow = true);
    void   Main(AppUI * ui);
    void   Deinit();

    void   SetMainWindowSize (ImVec2 size)        { (*ImGui::GetPlatformIO().Platform_SetWindowSize )(ImGui::GetMainViewport(), size);  }
    void   SetMainWindowPos  (ImVec2 pos)         { (*ImGui::GetPlatformIO().Platform_SetWindowPos  )(ImGui::GetMainViewport(), pos);   }
    void   SetMainWindowTitle(char const * title) { (*ImGui::GetPlatformIO().Platform_SetWindowTitle)(ImGui::GetMainViewport(), title); }
    void   SetMainWindowAlpha(float alpha)        { (*ImGui::GetPlatformIO().Platform_SetWindowAlpha)(ImGui::GetMainViewport(), alpha); }
    void   SetMainWindowFocus()                   { (*ImGui::GetPlatformIO().Platform_SetWindowFocus)(ImGui::GetMainViewport());        }

    ImVec2 GetMainWindowSize()      { return (*ImGui::GetPlatformIO().Platform_GetWindowSize)      (ImGui::GetMainViewport()); }
    ImVec2 GetMainWindowPos()       { return (*ImGui::GetPlatformIO().Platform_GetWindowPos)       (ImGui::GetMainViewport()); }
    bool   GetMainWindowMinimized() { return (*ImGui::GetPlatformIO().Platform_GetWindowMinimized) (ImGui::GetMainViewport()); }
    bool   GetMainWindowFocus()     { return (*ImGui::GetPlatformIO().Platform_GetWindowFocus)     (ImGui::GetMainViewport()); }

    float GetScaling()        const { return m_scaling; }
    float GetFontScaling()    const { return m_scalingFont; }
	float GetDefaultScaling() const { return m_scalingDef; }

    void SetScaling(float scaling = 1.0f)
    {
        if (Handy::WithinEpsilon(scaling, m_scaling))
            return;

        m_scaling = scaling;
        m_updateStyleForScale = true;
    }

    void SetFontScaling(float fontScaling = 1.0f)
    {
        if (Handy::WithinEpsilon(fontScaling, m_scalingFont))
            return;

        m_scalingFont = fontScaling;
        m_updateStyleForScale = true;
    }

	void SetDefaultScaling(float scaling = 1.0f)
	{
		if (Handy::WithinEpsilon(scaling, m_scalingDef))
			return;

		m_scalingDef = scaling;
	}

    ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height);
    ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height);
    void        DeleteImage(ImTextureID img);
    void        DeleteImageAsync(ImTextureID img);
    void        DeleteImagesAsync(std::vector<ImTextureID> const & imgs);
    void        DeleteImageAsyncWithDelay(ImTextureID img, double DelayInSeconds);
    void        DeleteImagesAsyncWithDelay(std::vector<ImTextureID> const & imgs, double DelayInSeconds);
    

    /// --- INTERNAL ---

    uint32_t      monitorRefreshRate();

    inline bool & zInternal_WasResized()
    {
        static bool instance = false;
        return instance;
    }

    static std::mutex    ContextMutex;
    static bool          ContextValid; // Safety for texture functions to early-out after the context is gone
};

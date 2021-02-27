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

#include "ImGuiApp.hpp"

namespace ImGuiTextures {

    namespace GLFW {
        void        Init();
        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height);
        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height);
        void        DeleteImage(ImTextureID img);
    }

    namespace EM {
        void        CreateImageAsyncURL(AImageAsyncURLRequest * req);
        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height);
        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height);
        void        DeleteImage(ImTextureID img);
    }

    namespace DX10 {
        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height);
        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height);
        void        DeleteImage(ImTextureID img);
    }

    namespace DX11 {
        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height);
        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height);
        void        DeleteImage(ImTextureID img);
    }

    namespace SW {
        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height);
        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height);
        void        DeleteImage(ImTextureID img);
    }
    
    #if !defined _WIN32
	    namespace DX10
	    {
	        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        void DeleteImage(ImTextureID img) { }
	    }
	    namespace DX11
	    {
	        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        void DeleteImage(ImTextureID img) { }
	    }
	    #if defined EMSCRIPTEN
		    namespace GLFW
		    {
		        void Init() { }
		        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
		        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
		        void DeleteImage(ImTextureID img) { }
		    }
		#else
			namespace EM
		    {
		        void        CreateImageAsyncURL(AImageAsyncURLRequest * req) { }
		        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
		        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
		        void DeleteImage(ImTextureID img) { }
		    }
	    #endif
	    namespace SW
	    {
	        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        void DeleteImage(ImTextureID img) { }
	    }
    #else
	    namespace EM
	    {
	        void        CreateImageAsyncURL(AImageAsyncURLRequest * req) { }
	        ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height) { return 0; }
	        void DeleteImage(ImTextureID img) { }
	    }
    #endif
}

#if !defined _WIN32
	void ImGuiApp::mainDX11() { }
	void ImGuiApp::mainDX10() { }
	void ImGuiApp::mainSW()   { }

    void ImGuiApp::initDX11() { }
    void ImGuiApp::initDX10() { }
    void ImGuiApp::initSW()   { }

	void ImGuiApp::deinitDX11() { }
	void ImGuiApp::deinitDX10() { }
	void ImGuiApp::deinitSW()   { }
#endif

/*static*/ std::mutex    ImGuiApp::ContextMutex;
/*static*/ bool          ImGuiApp::ContextValid = false; // Safety for texture functions to early-out after the context is gone


int ImGuiApp::swapInterval()
{
    switch (m_frameSync)
    {
        case FrameSyncType::VSync:   return  1;
        case FrameSyncType::NoSync:  return  0;
        default:
        case FrameSyncType::Dynamic: return -1;
    }
    return -1;
}

void ImGuiApp::Init(RenderingAPI api, FrameSyncType syncStrategy, bool showMainWindow)
{
    m_api            = api;
    m_frameSync      = syncStrategy;
    m_showMainWindow = showMainWindow;

    switch (m_api)
    {
        case RenderingAPI::DX10:
        case RenderingAPI::DX10W:              initDX10(); break;
        case RenderingAPI::DX11:
        case RenderingAPI::DX11W:              initDX11(); break;
        case RenderingAPI::GLFW:               initGL();   break;
        case RenderingAPI::SoftwareRasterizer: initSW();   break;
        case RenderingAPI::Emscripten:         throw std::runtime_error("Emscripten not implemented");
        default:
        case RenderingAPI::Ask:
        case RenderingAPI::INVALID:            throw std::runtime_error("Invalid RenderingAPI");
    }

	m_garbageBurner = std::thread(&ImGuiApp::GarbageBurnerMain, this);
}

void ImGuiApp::Deinit()
{
    m_garbageBurnerExit = true;
    m_garbageBurner.join();

	switch (m_api)
	{
		case RenderingAPI::DX10:
		case RenderingAPI::DX10W:              deinitDX10(); break;
		case RenderingAPI::DX11:
		case RenderingAPI::DX11W:              deinitDX11(); break;
		case RenderingAPI::GLFW:               deinitGL();   break;
		case RenderingAPI::SoftwareRasterizer: deinitSW();   break;
		case RenderingAPI::Emscripten:         throw std::runtime_error("Emscripten not implemented");
		default:
		case RenderingAPI::Ask:
		case RenderingAPI::INVALID:            throw std::runtime_error("Invalid RenderingAPI");
	}
}

void ImGuiApp::draw()
{
    if (m_updateStyleForScale)
    {
        m_updateStyleForScale = false;
        Style(m_scaling);
    }
}

ImTextureID ImGuiApp::CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height)
{
    switch (API())
    {
        case RenderingAPI::GLFW:                return ImGuiTextures::GLFW::CreateImageRGBA8888(pixels, width, height);
        case RenderingAPI::DX10:
        case RenderingAPI::DX10W:               return ImGuiTextures::DX10::CreateImageRGBA8888(pixels, width, height);
        case RenderingAPI::DX11:
        case RenderingAPI::DX11W:               return ImGuiTextures::DX11::CreateImageRGBA8888(pixels, width, height);
        case RenderingAPI::Emscripten:          return ImGuiTextures::EM  ::CreateImageRGBA8888(pixels, width, height);
        case RenderingAPI::SoftwareRasterizer:  return ImGuiTextures::SW  ::CreateImageRGBA8888(pixels, width, height);
        default:
        case RenderingAPI::Ask:
        case RenderingAPI::INVALID:             throw std::runtime_error("Invalid RenderingAPI"); return 0;
    }

    return 0;
}

ImTextureID ImGuiApp::CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height)
{
    ImTextureID ret = 0;

    switch (API())
    {
        case RenderingAPI::GLFW:               ret = ImGuiTextures::GLFW::CreateImageBGRA8888(pixels, width, height); break;
        case RenderingAPI::DX10:
        case RenderingAPI::DX10W:              ret = ImGuiTextures::DX10::CreateImageBGRA8888(pixels, width, height); break;
        case RenderingAPI::DX11:
        case RenderingAPI::DX11W:              ret = ImGuiTextures::DX11::CreateImageBGRA8888(pixels, width, height); break;
        case RenderingAPI::SoftwareRasterizer: ret = ImGuiTextures::SW  ::CreateImageBGRA8888(pixels, width, height); break;
        case RenderingAPI::Emscripten:         ret = ImGuiTextures::EM  ::CreateImageBGRA8888(pixels, width, height); break;
        default:
        case RenderingAPI::Ask:
        case RenderingAPI::INVALID:            throw std::runtime_error("Invalid RenderingAPI"); break;
    }

    return ret;
}


void ImGuiApp::DeleteImage(ImTextureID img)
{
    switch (API())
    {
        case RenderingAPI::GLFW:               ImGuiTextures::GLFW::DeleteImage(img); break;
        case RenderingAPI::DX10:
        case RenderingAPI::DX10W:              ImGuiTextures::DX10::DeleteImage(img); break;
        case RenderingAPI::DX11:
        case RenderingAPI::DX11W:              ImGuiTextures::DX11::DeleteImage(img); break;
        case RenderingAPI::Emscripten:         ImGuiTextures::EM  ::DeleteImage(img); break;
        case RenderingAPI::SoftwareRasterizer: ImGuiTextures::SW  ::DeleteImage(img); break;
        default:
        case RenderingAPI::Ask:
        case RenderingAPI::INVALID:
            throw std::runtime_error("Invalid RenderingAPI"); break;
    }
}

void ImGuiApp::GarbageBurnerMain() {
	//Note: If this is running after the ImGui and/or graphics context is invalidated, then it is important that DeleteImage()
	//not fail in any catastrophic ways if called when the context is invalid.
	//Another note: We use a std::thread instead of a thread_pool to avoid lambda overhead - otherwise we could hold up a draw
	//thread when destroying lots of textures at once. To avoid holding a lock on m_garbageToBurnMutex for too long (which could
	//also hold up a draw thread) we try to burn garbage in bursts instead of all at once.
	while (! m_garbageBurnerExit) {
		{
			std::scoped_lock lock(m_garbageToBurnMutex);
			std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
			size_t queueSize = m_garbageToBurn.size();
			size_t numItemsToDestroy = queueSize / 5U; //Try to destroy 20% of garbage (Clears 90% of garbage after 1 second)
			if (queueSize > 0U)
				numItemsToDestroy = std::max(numItemsToDestroy, size_t(1)); //Make sure we can empty the queue (delete at least 1 item each pass)
			
			if (numItemsToDestroy > 0U) {
				std::vector<std::tuple<ImTextureID, std::chrono::time_point<std::chrono::steady_clock>>> newQueue;
				std::vector<ImTextureID> toBurn;
				newQueue.reserve(queueSize);
				toBurn.reserve(numItemsToDestroy);
				
				for (auto const & item : m_garbageToBurn) {
					auto tex = std::get<0>(item);
					auto TP  = std::get<1>(item);
					if ((toBurn.size() < numItemsToDestroy) && (TP <= now))
						toBurn.push_back(tex);
					else
						newQueue.push_back(item);
				}
				for (auto tex : toBurn)
					DeleteImage(tex);
				m_garbageToBurn.swap(newQueue);
				//std::cerr << "Destroyed " << toBurn.size() << " textures\r\n";
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void ImGuiApp::DeleteImageAsync(ImTextureID img)
{
	if (img == 0)
		return;

	std::scoped_lock lock(m_garbageToBurnMutex);
	m_garbageToBurn.push_back(std::make_tuple(img, std::chrono::steady_clock::now()));
}

void ImGuiApp::DeleteImagesAsync(std::vector<ImTextureID> const & imgs)
{
	std::scoped_lock lock(m_garbageToBurnMutex);
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	for (ImTextureID img : imgs)
	{
		if (img != 0)
			m_garbageToBurn.push_back(std::make_tuple(img, now));
	}
}

void ImGuiApp::DeleteImageAsyncWithDelay(ImTextureID img, double DelayInSeconds)
{
	if (img == 0)
		return;
		
	std::scoped_lock lock(m_garbageToBurnMutex);
	std::chrono::time_point<std::chrono::steady_clock> TP = std::chrono::steady_clock::now() + std::chrono::milliseconds((int) std::round(DelayInSeconds*1000.0));
	m_garbageToBurn.push_back(std::make_tuple(img, TP));
}

void ImGuiApp::DeleteImagesAsyncWithDelay(std::vector<ImTextureID> const & imgs, double DelayInSeconds)
{
	std::scoped_lock lock(m_garbageToBurnMutex);
	std::chrono::time_point<std::chrono::steady_clock> TP = std::chrono::steady_clock::now() + std::chrono::milliseconds((int) std::round(DelayInSeconds*1000.0));
	for (ImTextureID img : imgs)
	{
		if (img != 0)
			m_garbageToBurn.push_back(std::make_tuple(img, TP));
	}
}

void ImGuiApp::Main(AppUI * ui)
{
    m_ui = ui;

    switch (m_api)
    {
        case RenderingAPI::DX10:
		case RenderingAPI::DX10W:              mainDX10(); break;
        case RenderingAPI::DX11:
		case RenderingAPI::DX11W:              mainDX11(); break;
		case RenderingAPI::GLFW:               mainGL();   break;
		case RenderingAPI::SoftwareRasterizer: mainSW();   break;
		case RenderingAPI::Emscripten:         throw std::runtime_error("Emscripten not implemented"); break;
        default:
        case RenderingAPI::Ask:
		case RenderingAPI::INVALID:            throw std::runtime_error("Invalid RenderingAPI");       break;
    }
}

bool ImGuiApp::shouldVSync()
{
    ImGuiIO & io = ImGui::GetIO();

    static bool mouseDown0Prev = io.MouseDown[0];
    static bool mouseDown1Prev = io.MouseDown[1];
    static bool mouseDown2Prev = io.MouseDown[2];
    static bool mouseDown3Prev = io.MouseDown[3];
    static bool mouseDown4Prev = io.MouseDown[4];
    static Math::Vector2 mousePosPrev = io.MousePos;

    bool mouseDownChanged =
        (io.MouseDown[0] != mouseDown0Prev) ||
        (io.MouseDown[1] != mouseDown1Prev) ||
        (io.MouseDown[2] != mouseDown2Prev) ||
        (io.MouseDown[3] != mouseDown3Prev) ||
        (io.MouseDown[4] != mouseDown4Prev);
    bool mouseMoved = io.MousePos != mousePosPrev;

    bool mouseIsDown =
        io.MouseDown[0] ||
        io.MouseDown[1] ||
        io.MouseDown[2] ||
        io.MouseDown[3] ||
        io.MouseDown[4];

    mousePosPrev   = io.MousePos;
    mouseDown0Prev = io.MouseDown[0];
    mouseDown1Prev = io.MouseDown[1];
    mouseDown2Prev = io.MouseDown[2];
    mouseDown3Prev = io.MouseDown[3];
    mouseDown4Prev = io.MouseDown[4];

    bool mouseWheelChanged = io.MouseWheel != 0.0f || io.MouseWheelH != 0.0f;

    return !(mouseIsDown || mouseDownChanged || mouseMoved || mouseWheelChanged);
}

void ImGuiApp::Style(float scaling)
{
    ImGuiStyle & style = ImGui::GetStyle();
    ImVec4 * colors = style.Colors;

    /// 0 = FLAT APPEARENCE
    /// 1 = More "3D" look
    int is3D = 0;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
    colors[ImGuiCol_Separator]              = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

    style.PopupRounding = std::round(3.0f * scaling);

    style.WindowPadding = (Math::Vector2(4, 4) * scaling).Round_Near().Max(1.0f);
    style.FramePadding  = (Math::Vector2(6, 4) * scaling).Round_Near().Max(1.0f);
    style.ItemSpacing   = (Math::Vector2(6, 2) * scaling).Round_Near().Max(1.0f);

    style.ScrollbarSize = std::max(std::round(18 * scaling), 1.0f);

    style.WindowBorderSize = std::max(std::round(1 * scaling), 1.0f);
    style.ChildBorderSize  = std::max(std::round(1 * scaling), 1.0f);
    style.PopupBorderSize  = std::max(std::round(1 * scaling), 1.0f);
    style.FrameBorderSize  = std::max(std::round(is3D * scaling), is3D == 1 ? 1.0f : 0.0f);

    style.WindowRounding    = std::max(std::round(3 * scaling), 1.0f);
    style.ChildRounding     = std::max(std::round(3 * scaling), 1.0f);
    style.FrameRounding     = std::max(std::round(3 * scaling), 1.0f);
    style.ScrollbarRounding = std::max(std::round(2 * scaling), 1.0f);
    style.GrabRounding      = std::max(std::round(3 * scaling), 1.0f);

    #ifdef IMGUI_HAS_DOCK 
    style.TabBorderSize = std::max(std::round(is3D * scaling), is3D == 1 ? 1.0f : 0.0f);
    style.TabRounding   = std::max(std::round(3 * scaling), 1.0f);

    colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_Tab]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_TabActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_TabUnfocused]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_DockingPreview]     = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        //style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    #endif

    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.00f, 0.69f, 1.00f, 0.58f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.00f, 0.69f, 1.00f, 0.58f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.00f, 0.50f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.00f, 0.69f, 1.00f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.74f, 0.98f, 0.26f, 1.00f);

    ClearColor = ImVec4(0.440f, 0.440f, 0.440f, 1.000f);
}

ImFont * Fonts::Tiny       = nullptr;
ImFont * Fonts::Small      = nullptr;
ImFont * Fonts::Normal     = nullptr;
ImFont * Fonts::NormalBold = nullptr;
ImFont * Fonts::Large      = nullptr;
ImFont * Fonts::Huge       = nullptr;
ImFont * Fonts::Mono       = nullptr;
ImFont * Fonts::Default    = nullptr;
//ImFont * Fonts::MonoUTF8   = nullptr;

float Fonts::Scaling = -1.0f;
float Fonts::FontScaling = -1.0f;


// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar * GlyphRanges()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2500, 0x25B0,
        0,
    };
    return &ranges[0];
}

void Fonts::Build(float scaling, float fontScaling)
{
    Scaling = scaling;
    FontScaling = fontScaling;

    ImGuiIO & io = ImGui::GetIO();


    if (Normal != nullptr)
    {
        //io.Fonts->ClearFonts();
        io.Fonts->Clear();

        Fonts::Tiny       = nullptr;
        Fonts::Small      = nullptr;
        Fonts::Normal     = nullptr;
        Fonts::NormalBold = nullptr;
        Fonts::Large      = nullptr;
        Fonts::Huge       = nullptr;
        Fonts::Mono       = nullptr;
        //Fonts::MonoUTF8   = nullptr;
    }

    #if defined IMGUIAPP_USE_FAS
    	static const ImWchar icons_ranges[] = { (ImWchar) ICON_MIN_FA,  (ImWchar) ICON_MAX_FA,  0 };
    #else
	    static const ImWchar icons_ranges[] = { (ImWchar) ICON_MIN_MDI, (ImWchar) ICON_MAX_MDI, 0 };
    #endif
	    
    ImFontConfig icons_config; 
    icons_config.MergeMode  = true; 
    icons_config.PixelSnapH = true;

    auto CalcScaledFontSize = [scaling, fontScaling](float unscaledSize) { return std::round(unscaledSize * scaling * fontScaling); };

    auto exedir = Handy::Paths::ThisExecutableDirectory();
    auto fontsdir = exedir / "Fonts";

    std::filesystem::path ttfPath = 
        fontsdir /

    #if defined IMGUIAPP_USE_FAS
    	FONT_ICON_FILE_NAME_FAS;
    #else
        FONT_ICON_FILE_NAME_MDI;
    #endif

    Tiny          = io.Fonts->AddFontFromFileTTF((fontsdir / "DejaVuSansCondensed.ttf")     .u8string().c_str(), CalcScaledFontSize(10));
		io.Fonts->AddFontFromFileTTF(ttfPath.u8string().c_str(),                                                 CalcScaledFontSize(10), &icons_config, icons_ranges); // Only the "Normal" font will have the font images.
	Small         = io.Fonts->AddFontFromFileTTF((fontsdir / "DejaVuSansCondensed.ttf")     .u8string().c_str(), CalcScaledFontSize(12));
    Normal        = io.Fonts->AddFontFromFileTTF((fontsdir / "DejaVuSansCondensed.ttf")     .u8string().c_str(), CalcScaledFontSize(15));
		io.Fonts->AddFontFromFileTTF(ttfPath.u8string().c_str(),                                                 CalcScaledFontSize(15), &icons_config, icons_ranges); // Only the "Normal" font will have the font images.
    NormalBold    = io.Fonts->AddFontFromFileTTF((fontsdir / "DejaVuSansCondensed-Bold.ttf").u8string().c_str(), CalcScaledFontSize(15));
    Large         = io.Fonts->AddFontFromFileTTF((fontsdir / "DejaVuSansCondensed.ttf")     .u8string().c_str(), CalcScaledFontSize(18));
		io.Fonts->AddFontFromFileTTF(ttfPath.u8string().c_str(),                                                 CalcScaledFontSize(18), &icons_config, icons_ranges); // Only the "Normal" font will have the font images.
    Huge          = io.Fonts->AddFontFromFileTTF((fontsdir / "DejaVuSansCondensed.ttf")     .u8string().c_str(), CalcScaledFontSize(24));
    Mono          = io.Fonts->AddFontFromFileTTF((fontsdir / "Input-Regular.ttf")           .u8string().c_str(), CalcScaledFontSize(12));
    //MonoUTF8      = io.Fonts->AddFontFromFileTTF((fontsdir / "unifont-10.0.07.ttf").u8string().c_str(),      CalcScaledFontSize(16), nullptr, GlyphRanges());

    ///// Some additional demo fonts
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Roboto-Regular.ttf").u8string().c_str(), CalcScaledFontSize(17));
    //io.Fonts->AddFontFromFileTTF(ttfPath.u8string().c_str(),                           CalcScaledFontSize(17), &icons_config, icons_ranges); // Only the "Normal" font will have the font images.

    //io.Fonts->AddFontFromFileTTF((fontsdir / "segoeui.ttf").u8string().c_str(), CalcScaledFontSize(17));
    //io.Fonts->AddFontFromFileTTF(ttfPath.u8string().c_str(),                    CalcScaledFontSize(17), &icons_config, icons_ranges); // Only the "Normal" font will have the font images.

    //io.Fonts->AddFontFromFileTTF((fontsdir / "SanFranciscoDisplayRegular.ttf").u8string().c_str(), CalcScaledFontSize(15));
    //io.Fonts->AddFontFromFileTTF(ttfPath.u8string().c_str(),                                       CalcScaledFontSize(15), &icons_config, icons_ranges); // Only the "Normal" font will have the font images.

    Default = io.Fonts->AddFontDefault();
    io.FontDefault = Normal;

    /// Use this font for Android, instead of Deja Vu:
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Roboto-Regular.ttf").u8string().c_str(), 17);
    //io.Fonts->AddFontFromFileTTF(ttfPath.u8string().c_str(), 17, &icons_config, icons_ranges );


    //io.Fonts->AddFontFromFileTTF((fontsdir / "dejavu-sans/DejaVuSansCondensed.ttf").u8string().c_str(), 14);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "dejavu-sans/DejaVuSansCondensed.ttf").u8string().c_str(), 16);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "DroidSans-Regular.ttf").u8string().c_str(), 15);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "DroidSans-Regular.ttf").u8string().c_str(), 14);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "DroidSans-Regular.ttf").u8string().c_str(), 16);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "dejavu-sans/DejaVuSans.ttf").u8string().c_str(), 14);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "dejavu-sans/DejaVuSans.ttf").u8string().c_str(), 15);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "dejavu-sans/DejaVuSans.ttf").u8string().c_str(), 16);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "segoeui.ttf").u8string().c_str(), 17);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "segoeui.ttf").u8string().c_str(), 18);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Roboto-Regular.ttf").u8string().c_str(), 18);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Roboto-Medium.ttf").u8string().c_str(), 17);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Roboto-Medium.ttf").u8string().c_str(), 18);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Cousine-Regular.ttf").u8string().c_str(), 14);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Cousine-Regular.ttf").u8string().c_str(), 15);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Cousine-Regular.ttf").u8string().c_str(), 16);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Karla-Regular.ttf").u8string().c_str(), 14);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Karla-Regular.ttf").u8string().c_str(), 15);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Karla-Regular.ttf").u8string().c_str(), 16);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Input-Regular.ttf").u8string().c_str(), 9);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Input-Regular.ttf").u8string().c_str(), 10);
    //io.Fonts->AddFontFromFileTTF((fontsdir / "Input-Regular.ttf").u8string().c_str(), 11);
}



#if defined IS_WINDOWS
    #include <Windows.h>
    //#include <shellscalingapi.h>
#endif

uint32_t ImGuiApp::monitorRefreshRate()
{
    #if defined IS_WINDOWS
        DEVMODE lpDevMode;
        memset(&lpDevMode, 0, sizeof(DEVMODE));

        lpDevMode.dmSize = sizeof(DEVMODE);
        lpDevMode.dmDriverExtra = 0;

        if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode))
            return 60; // default value if cannot retrieve from user settings.

        return lpDevMode.dmDisplayFrequency;
    #else
        /// TODO!!!
        return 60;
    #endif
}

/// If we're running on a system with hybrid graphics, try to force the selection of the high-performance gpu
#if defined(WIN32) || defined(_WIN32)
	extern "C" { __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }
	extern "C" { __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; }
#else 
	extern "C" { int NvOptimusEnablement = 1; }
	extern "C" { int AmdPowerXpressRequestHighPerformance = 1; }
#endif




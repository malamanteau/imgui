
#include <thread>
#include <chrono>

#include <handycpp/Handy.hpp>
#include "libs/ImFastRast/ImFastRast.hpp"

#include "imgui_impl_sw.h"
#include "imgui.h"
#include <Windows.h>
#include "misc/freetype/imgui_freetype.h"

/// Include ImFastRast

static ImFastRast::Texture * g_pFontTexture = nullptr;

#include "../app/ImGuiApp.hpp"

namespace ImGuiTextures {
	namespace SW {

		ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height)
		{
            Locked(ImGuiApp::ContextMutex)
            {
                if (!ImGuiApp::ContextValid)
                    return nullptr;

                auto nuPixels = new uint8[width * height * 4];
                memcpy(nuPixels, pixels, width * height * 4);
                return (ImTextureID) new ImFastRast::Texture{ nuPixels, width, height, false };
            }
		}

		ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height)
		{
            Locked(ImGuiApp::ContextMutex)
            {
                if (!ImGuiApp::ContextValid)
                    return nullptr;

                auto nuPixels = new uint8[width * height * 4];
                memcpy(nuPixels, pixels, width * height * 4);
                return (ImTextureID) new ImFastRast::Texture{ nuPixels, width, height, false };
            }
		}


		void DeleteImage(ImTextureID img)
		{
            Locked(ImGuiApp::ContextMutex)
            {
                if (!ImGuiApp::ContextValid)
                    return;

                ImFastRast::Texture * nu = (ImFastRast::Texture *)img;

                if (nu->Pixels)
                    delete[] nu->Pixels;

                delete nu;

                return;
            }
		}
	}
}

// Forward Declarations
static void ImGui_ImplSW_InitPlatformInterface();
static void ImGui_ImplSW_ShutdownPlatformInterface();


void ImGui_ImplSW_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO & io = ImGui::GetIO();
	uint8_t * pixels;
	int width, height;

	ImGuiFreeType::BuildFontAtlas(io.Fonts, ImGuiFreeType::RasterizerFlags::ForceAutoHint);

	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	g_pFontTexture = new ImFastRast::Texture{ pixels, width, height, true };
	io.Fonts->TexID = g_pFontTexture;
}

void ImGui_ImplSW_DestroyFontTexture()
{
	ImGuiIO & io = ImGui::GetIO();

	if (g_pFontTexture)
	{
		io.Fonts->TexID = 0;
		delete (ImFastRast::Texture *)g_pFontTexture;
		g_pFontTexture = nullptr;
	}
}

bool ImGui_ImplSW_CreateDeviceObjects()
{
	ImGui_ImplSW_CreateFontsTexture();

	return true;
}

void ImGui_ImplSW_InvalidateDeviceObjects()
{

}

bool ImGui_ImplSW_Init(/*ID3D10Device* device*/)
{
	// Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports; // We can create multi-viewports on the Renderer side (optional)
	io.BackendRendererName = "imgui_impl_sw";

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		ImGui_ImplSW_InitPlatformInterface();
	return true;
}

void ImGui_ImplSW_Shutdown()
{
	ImGui_ImplSW_ShutdownPlatformInterface();
	ImGui_ImplSW_InvalidateDeviceObjects();

	ImGui_ImplSW_DestroyFontTexture();
}

void ImGui_ImplSW_NewFrame()
{
	if (!g_pFontTexture)
		ImGui_ImplSW_CreateFontsTexture();
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

struct ImGuiViewportDataSW
{
	std::atomic_bool SwapAlreadyEnqueued = false;

	std::mutex       SwapWaitLock;
	std::atomic_bool SwapWaitReady = false;
	Math::Vector2i   SwapWaitDimensions;

	std::vector<uint32_t> BufferDrawing;
	std::vector<uint32_t> BufferSwapWait;
	std::vector<uint32_t> BufferSwapping;
	
	ImGuiViewportDataSW()
	{ }
};

static Handy::ThreadPool DrawPool(std::max(2_szt, (size_t)std::thread::hardware_concurrency() / 4_szt - 1_szt));
static Handy::ThreadPool SwapPool(std::max(2_szt, (size_t)std::thread::hardware_concurrency() / 4_szt - 1_szt));

static void ImGui_ImplSW_CreateWindow(ImGuiViewport* viewport)
{
	ImGuiViewportDataSW* data = IM_NEW(ImGuiViewportDataSW)();

	// Make it double the size of the window so we have room to grow w/o reallocating.
	data->BufferDrawing .resize(viewport->Size.x * viewport->Size.y * 2, 0x00000000); 
	data->BufferSwapWait.resize(viewport->Size.x * viewport->Size.y * 2, 0x00000000); 
	data->BufferSwapping.resize(viewport->Size.x * viewport->Size.y * 2, 0x00000000); 

	viewport->RendererUserData = data;

	HWND hwnd = (HWND)viewport->PlatformHandle;
	IM_ASSERT(hwnd != 0);
}

static void ImGui_ImplSW_DestroyWindow(ImGuiViewport* viewport)
{
	DrawPool.Wait();
	SwapPool.Wait();

	// The main viewport (owned by the application) will always have RendererUserData == NULL here since we didn't create the data for it.
	if (ImGuiViewportDataSW* data = (ImGuiViewportDataSW*)viewport->RendererUserData)
	{
		data->BufferDrawing .resize(0);
		data->BufferSwapWait.resize(0);
		data->BufferSwapping.resize(0);
		IM_DELETE(data);
	}
	viewport->RendererUserData = NULL;
}

static void ImGui_ImplSW_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	ImGuiViewportDataSW* data = (ImGuiViewportDataSW*)viewport->RendererUserData;

	auto area = viewport->Size.x * viewport->Size.y;

	ImGuiApp::Instance().zInternal_WasResized() = true;

	DrawPool.Wait();
	SwapPool.Wait();

	if (data->BufferDrawing.size() >= area)
		return;
	   
	// Make it double the size of the window so we have room to grow w/o reallocating.
	data->BufferDrawing .resize(area * 2, 0x00000000);
	data->BufferSwapWait.resize(area * 2, 0x00000000);
	data->BufferSwapping.resize(area * 2, 0x00000000);
}

static void ImGui_ImplSW_RenderViewport(ImGuiViewport* viewport, void *)
{
	using XXHash64 = Handy::Hash::XXHash64;
	static Handy::StopWatch sw;

	uint64 lastHValue = 0;
	if (viewport->UserData)
	{
		lastHValue = ((XXHash64*)viewport->UserData)->Get();

		delete (XXHash64*)viewport->UserData;
		viewport->UserData = nullptr;
	}

	viewport->UserData = new XXHash64(1105121757519812621_u64);
	XXHash64 * h = (XXHash64 *)viewport->UserData;

	ImGuiIO & io = ImGui::GetIO();

	const float width_points  = io.DisplaySize.x;
	const float height_points = io.DisplaySize.y;

	ImDrawData * draw_data = viewport->DrawData;

	const int  width_pixels = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
	const int height_pixels = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);

	h->Add(&width_pixels,               4_u64);
	h->Add(&height_pixels,              4_u64);
	h->Add(&(draw_data->CmdListsCount), 4_u64);

	for (int i = 0; i < draw_data->CmdListsCount; i++)
	{
		h->Add(&(draw_data->CmdLists[i]->VtxBuffer[0]), draw_data->CmdLists[i]->VtxBuffer.size() * sizeof(ImDrawVert));
		h->Add(&(draw_data->CmdLists[i]->IdxBuffer[0]), draw_data->CmdLists[i]->IdxBuffer.size() * sizeof(ImDrawIdx));
		h->Add(&(draw_data->CmdLists[i]->CmdBuffer[0]), draw_data->CmdLists[i]->CmdBuffer.size() * sizeof(ImDrawCmd));
	}

	uint64_t time = sw.Seconds();
	h->Add(&time, 8);

	uint64 nuHValue = h->Get();

	if (lastHValue == nuHValue)
	{
		viewport->NeedSwap = false;
		return;
	}
	else 
		viewport->NeedSwap = true;

	ImGuiViewportDataSW* data = (ImGuiViewportDataSW*)viewport->RendererUserData;

	///Probably should clear the buffer, but this prototype doesn't use the background... so...
	//ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	//ImFastRast::Fill(&data->BufferA[0], width_pixels, height_pixels, 0x00000000);

	Math::Vector2 dispOff = draw_data->DisplayPos;

	for (size_t i = 0; i < draw_data->CmdListsCount; i++)
	{
		auto * commandList = draw_data->CmdLists[i];

		for (int cmd_i = 0; cmd_i < commandList->CmdBuffer.size(); cmd_i++)
		{
			ImDrawCmd & pcmd = commandList->CmdBuffer[cmd_i];

			pcmd.ClipRect.x -= dispOff.X;
			pcmd.ClipRect.y -= dispOff.Y;
			pcmd.ClipRect.z -= dispOff.X;
			pcmd.ClipRect.w -= dispOff.Y;
		}

		for (size_t v = 0; v < commandList->VtxBuffer.size(); v++)
		{
			ImVec2 & p = commandList->VtxBuffer[v].pos;
			p = p - dispOff; //(mvp * Math::Vector4(p.x, p.y, 0.0f, 1.0f)).XY();
		}
	}

	auto ptr = &data->BufferDrawing[0];
	DrawPool.AddJob([=]
	{
		ImFastRast::Paint(draw_data, ptr, width_pixels, height_pixels);
		std::scoped_lock sLock(data->SwapWaitLock);
		std::swap(data->BufferDrawing, data->BufferSwapWait);
		data->SwapWaitDimensions = Math::Vector2i(width_pixels, height_pixels);
		data->SwapWaitReady = true;
	});
}

static void w32swap(HWND wnd, int x, int y, int w, int h, void * data)
{
	BITMAPINFOHEADER hdr = { 0 };
	hdr.biSize         = sizeof(hdr);
	hdr.biWidth        = w;
	hdr.biHeight       = -h;
	hdr.biPlanes       = 1;
	hdr.biBitCount     = 32;
	hdr.biCompression  = BI_RGB;
	hdr.biSizeImage    = w * h * 4;
	hdr.biClrUsed      = 0;
	hdr.biClrImportant = 0;
	HDC dc = GetDC(wnd);
	SetDIBitsToDevice(dc, x, y, w, h, 0, 0, 0, h, data, (BITMAPINFO *)&hdr, DIB_RGB_COLORS);
	ReleaseDC(wnd, dc);
}

void ImGui_ImplSW_BlockFinishDrawResources()
{
	DrawPool.Wait();
}

static void ImGui_ImplSW_SwapBuffers(ImGuiViewport* viewport, void*)
{
	/*if (!viewport->NeedSwap)
		return;*/

	//DrawPool.Wait();

	ImGuiViewportDataSW* data = (ImGuiViewportDataSW*)viewport->RendererUserData;
	//data->SwapChain->Present(0, 0); // Present without vsync

	HWND hwnd = (HWND)viewport->PlatformHandle;
	IM_ASSERT(hwnd != 0);

	RECT cr;
	GetClientRect(hwnd, &cr);
	int x = cr.left, y = cr.top/*, w = cr.right - cr.left, h = cr.bottom - cr.top*/;
	//if (w * h > data->BufferDraw.size())
	//	data->BufferDraw.resize(w * h * 1.5, 0x00000000);


	//w = viewport->Size.x;
	//h = viewport->Size.y;

	bool alreadyEnqueued = data->SwapAlreadyEnqueued.exchange(true);
	if (!alreadyEnqueued)
		SwapPool.AddJob([=]
		{
			bool ready = false;
			Math::Vector2i sDim;
			{
				std::scoped_lock sLock(data->SwapWaitLock);
				ready = data->SwapWaitReady.exchange(false);
				if (ready)
				{
					std::swap(data->BufferSwapWait, data->BufferSwapping);
					sDim = data->SwapWaitDimensions;
				}
			}

			if (ready)
				w32swap(hwnd, x, y, sDim.X, sDim.Y, &data->BufferSwapping[0]);

			data->SwapAlreadyEnqueued = false;
		});
}

void ImGui_ImplSW_InitPlatformInterface()
{
	ImGuiPlatformIO & platform_io = ImGui::GetPlatformIO();
	platform_io.Renderer_CreateWindow  = ImGui_ImplSW_CreateWindow;
	platform_io.Renderer_DestroyWindow = ImGui_ImplSW_DestroyWindow;
	platform_io.Renderer_SetWindowSize = ImGui_ImplSW_SetWindowSize;
	platform_io.Renderer_RenderWindow  = ImGui_ImplSW_RenderViewport;
	platform_io.Renderer_SwapBuffers   = ImGui_ImplSW_SwapBuffers;
}

void ImGui_ImplSW_ShutdownPlatformInterface()
{
	ImGui::DestroyPlatformWindows();
}


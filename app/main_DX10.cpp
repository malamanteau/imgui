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

// dear imgui: standalone example application for DirectX 10
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "ImGuiApp.hpp"

#include "imgui.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx10.h"

#include <d3d10_1.h>
#include <d3d10.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

// Data
static ID3D10Device*            g_pd3dDevice = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D10RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
static bool CreateDeviceD3D(HWND hWnd);
static void CleanupDeviceD3D();
static void CreateRenderTarget();
static void CleanupRenderTarget();
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND hwnd = nullptr;
static WNDCLASSEX wc;

void ImGuiApp::initDX10()
{


	ImGui_ImplWin32_EnableDpiAwareness();

	const POINT ptZero = { 0, 0 };
	auto defScaling = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY));
	ImGuiApp::Instance().SetScaling(defScaling);
	ImGuiApp::Instance().SetDefaultScaling(defScaling);

	// Create application window
	ZeroMemory(&wc, sizeof(wc));
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	::RegisterClassEx(&wc);
	hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX10 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 256, 256, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		throw std::runtime_error("Cannot Initialize DX10");
	}

	// Show the window
	if (m_showMainWindow)
		::ShowWindow(hwnd, SW_SHOWDEFAULT);
	else
		::ShowWindow(hwnd, SW_HIDE);

	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
																//io.ConfigViewportsNoAutoMerge = true;
																//io.ConfigViewportsNoTaskBarIcon = true;
	io.ConfigDockingWithShift = true;

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX10_Init(g_pd3dDevice);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	Style();

	Fonts::Build(1.0f, 1.0f);

	Locked(ImGuiApp::ContextMutex)
		ImGuiApp::ContextValid = true;
}

static bool isFirstFrame = true;

// Main code
void ImGuiApp::mainDX10()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	// Main loop
	while (msg.message != WM_QUIT && m_ui->DrawLoopEnabled)
	{
		Handy::StopWatch sw;

		// Poll and handle messages (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		if (!isFirstFrame)
		{
			if (m_scaling != Fonts::Scaling || m_scalingFont != Fonts::FontScaling)
			{
				ImGui_ImplDX10_DestroyFontTexture();
				Fonts::Build(m_scaling, m_scalingFont);
				ImGui_ImplDX10_CreateFontsTexture();
	
				ImGui::SetCurrentFont(Fonts::Normal);
			}
		}
		else
			isFirstFrame = false;

		// Start the Dear ImGui frame
		ImGui_ImplDX10_NewFrame();
		ImGui_ImplWin32_NewFrame();

		m_ui->Preframe();

		ImGui::NewFrame();

		/// By disabling rounding when running DX WARP, we get a much improved framerate.
		Scoped(ImExt::Style fastStyle)
		{
			if (API_IsWarp())
			{
				fastStyle(StyleVar::ChildRounding,     0.0f);
				fastStyle(StyleVar::FrameRounding,     0.0f);
				fastStyle(StyleVar::GrabRounding,      0.0f);
				fastStyle(StyleVar::PopupRounding,     0.0f);
				fastStyle(StyleVar::ScrollbarRounding, 0.0f);
				fastStyle(StyleVar::TabRounding,       0.0f);
				fastStyle(StyleVar::WindowRounding,    0.0f);
			}

			m_ui->Draw();
		}

		/// ---- Rendering ---- /// 
		bool asvs = shouldVSync(); // Must be before ImGui::Render()

		ImGui::Render();

		g_pd3dDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDevice->ClearRenderTargetView(g_mainRenderTargetView, (float*)&ClearColor.x);
		ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
		
		if (zInternal_WasResized()) // Must be after ImGui::UpdatePlatformWindows()
			asvs = true;

		if (swapInterval() == -1)
			g_pSwapChain->Present(asvs ? 1 : 0, 0); // Present with vsync
		else
			g_pSwapChain->Present(swapInterval(), 0); // Present with vsync

		zInternal_WasResized() = false;

		m_ui->Postframe();

		int32 rr = 1000_i32 / (int32)(monitorRefreshRate() * 4);
		int32 timeDiff = rr - (int32)(sw.Milliseconds());

		if (timeDiff > 0)// && asvs)
			std::this_thread::sleep_for(std::chrono::milliseconds(timeDiff));
	}
}

void ImGuiApp::deinitDX10()
{
	Locked(ImGuiApp::ContextMutex)
		ImGuiApp::ContextValid = false;

	// Cleanup
	ImGui_ImplDX10_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	UINT numRR = 60;

	HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
	if (hMon)
	{
		MONITORINFOEXW monInfo = { 0 };
		monInfo.cbSize = sizeof(MONITORINFOEX);

		if (GetMonitorInfoW(hMon, &monInfo))
		{
			DEVMODEW devMode = { 0 };
			devMode.dmSize = sizeof(DEVMODEW);

			if (EnumDisplaySettingsW(monInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode)
				&& devMode.dmDisplayFrequency >= 1 
				&& devMode.dmDisplayFrequency < 1024)
			{
				numRR = devMode.dmDisplayFrequency;
			}
		}
	}

	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = numRR;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow       = hWnd;
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed           = TRUE;
	sd.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
	if (D3D10CreateDeviceAndSwapChain(NULL, ImGuiApp::Instance().API_IsWarp() ? D3D10_DRIVER_TYPE_WARP : D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
	ID3D10Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


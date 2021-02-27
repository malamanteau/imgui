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

#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_sw.h"

#include <dinput.h>
#include <tchar.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
		case WM_ERASEBKGND:
		case WM_PAINT:
			//mark the window as validated without actually doing anything, it will get repainted in the main loop
			ValidateRect(hWnd, nullptr);
			return 0;
		case WM_SIZE:
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

static HWND hwnd = nullptr;
static WNDCLASSEX wc;

void ImGuiApp::initSW()
{
    ImGui_ImplWin32_EnableDpiAwareness();

	const POINT ptZero = { 0, 0 };
	auto defScaling = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY));
	ImGuiApp::Instance().SetScaling(defScaling);
	ImGuiApp::Instance().SetDefaultScaling(defScaling);


    ZeroMemory(&wc, sizeof(wc));
    wc = { 0 };
    {
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.lpfnWndProc   = WndProc;
        wc.hInstance     = GetModuleHandle(nullptr);
        wc.lpszClassName = _T("ImGui Example");


        //////////CS_CLASSDC
    }

    RegisterClassEx(&wc);

    hwnd = CreateWindow(_T("ImGui Example"), _T("Dear ImGui DirectX10 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 256, 256, NULL, NULL, wc.hInstance, NULL);

    // Show the window
    if (m_showMainWindow)
        ShowWindow(hwnd, SW_SHOWDEFAULT);
    else
        ShowWindow(hwnd, SW_HIDE);

    UpdateWindow(hwnd);

    //force resize to the requested size (in case the requested size was bigger than the available monitors)
    //SetWindowPos(hwnd, nullptr, 0, 0, actualWindowWidth, actualWindowHeight, SWP_NOSENDCHANGING);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
                                                                //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcon;
                                                                //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
    io.ConfigDockingWithShift = true;

    ImGui_ImplWin32_Init(hwnd, true);
    ImGui_ImplSW_Init(/*g_pd3dDevice*/);

    Style();

    Fonts::Build(1.0f, 1.0f);

    Locked(ImGuiApp::ContextMutex)
        ImGuiApp::ContextValid = true;
}

static bool isFirstFrame = true;

void ImGuiApp::mainSW()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	// Main loop
	while (msg.message != WM_QUIT && m_ui->DrawLoopEnabled)
	{
		Handy::StopWatch sw;

		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

        if (!isFirstFrame)
        {
			if (m_scaling != Fonts::Scaling || m_scalingFont != Fonts::FontScaling)
			{
				ImGui_ImplSW_DestroyFontTexture();
				Fonts::Build(m_scaling, m_scalingFont);
				ImGui_ImplSW_CreateFontsTexture();
	
				ImGui::SetCurrentFont(Fonts::Normal);
			}
        }
        else
            isFirstFrame = false;

		ImGui_ImplSW_NewFrame();
		ImGui_ImplWin32_NewFrame();
		
		m_ui->Preframe();

		ImGui_ImplSW_BlockFinishDrawResources();

		ImGui::NewFrame();

		/// By disabling rounding we get a much improved framerate.
		Scoped(ImExt::Style fastStyle)
		{
			fastStyle(StyleVar::ChildRounding,     0.0f);
			fastStyle(StyleVar::FrameRounding,     0.0f);
			fastStyle(StyleVar::GrabRounding,      0.0f);
			fastStyle(StyleVar::PopupRounding,     0.0f);
			fastStyle(StyleVar::ScrollbarRounding, 0.0f);
			fastStyle(StyleVar::TabRounding,       0.0f);
			fastStyle(StyleVar::WindowRounding,    0.0f);

			m_ui->Draw();
		}

		/// ---- Rendering ---- /// 
		bool asvs = shouldVSync(); // Must be before ImGui::Render()

		ImGui::Render();

		//g_pd3dDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		//g_pd3dDevice->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
		//ImGui_ImplSW_RenderDrawData(ImGui::GetDrawData(), nullptr);

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
		
		if (zInternal_WasResized()) // Must be after ImGui::UpdatePlatformWindows()
			asvs = true;

		//if (swapInterval() == -1)
		//	g_pSwapChain->Present(asvs ? 1 : 0, 0); // Present with vsync
		//else
		//	g_pSwapChain->Present(swapInterval(), 0); // Present with vsync

		zInternal_WasResized() = false;

        m_ui->Postframe();

		int32 rr = 1000_i32 / (int32)(monitorRefreshRate() * 4);
		int32 timeDiff = rr - (int32)(sw.Milliseconds());

		if (timeDiff > 0)// && asvs)
			std::this_thread::sleep_for(std::chrono::milliseconds(timeDiff));
	}
}

void ImGuiApp::deinitSW()
{
    Locked(ImGuiApp::ContextMutex)
        ImGuiApp::ContextValid = false;

	// Cleanup
	ImGui_ImplSW_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(hwnd);
	UnregisterClass(_T("ImGui Example"), wc.hInstance);
}

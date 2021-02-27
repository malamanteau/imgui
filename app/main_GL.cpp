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

// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

#include <stdio.h>

#include "ImGuiApp.hpp"
#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h> 

namespace ImGuiTextures {
	namespace GLFW {

		static Handy::ThreadPool * g_pool = nullptr;
		static GLFWwindow * OffscreenContext = nullptr;

		void Init()
		{
			Locked(ImGuiApp::ContextMutex)
			{
	
				if (!OffscreenContext)
				{
					glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
					//fprintf(stderr, "Calling glfwCreateWindow() to create offscreen context...\r\n"); fflush(stderr);
					OffscreenContext = glfwCreateWindow(256, 256, "", NULL, NULL);
					//fprintf(stderr, "glfwCreateWindow() returned\r\n"); fflush(stderr);
					glfwMakeContextCurrent(OffscreenContext);
					glfwSwapInterval(0);
	
					glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
				}
	
				if (!OffscreenContext)
				{
					fprintf(stderr, "Failed to create OffscreenContext.\r\n"); fflush(stderr);
				}
			}
		}

		ImTextureID CreateImageRGBA8888(uint8_t const * pixels, int32_t width, int32_t height)
		{
			Locked(ImGuiApp::ContextMutex)
			{
				if (! ImGuiApp::ContextValid)
					return nullptr;

				if (!g_pool)
				{
					if (!OffscreenContext)
						return 0;
	
					g_pool = new Handy::ThreadPool(1);
	
					g_pool->AddJob([]
					{
						glfwMakeContextCurrent(OffscreenContext);
						glfwSwapInterval(0);
					});
				}
	
				g_pool->Wait();
	
				//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				GLuint textureID = 0;
	
				g_pool->AddJob([&textureID, width, height, pixels]
				{
					glEnable(GL_TEXTURE_2D);
					//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
					glGenTextures(1, &textureID);
					glBindTexture(GL_TEXTURE_2D, textureID);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
					if (!textureID)
					{
						fprintf(stderr, "Zero-value texture ID...");
					}
	
					glFinish();
					glFlush();
				});
	
				g_pool->Wait();
	
				if (!textureID)
				{
					fprintf(stderr, " zero value confirmed.\r\n");
				}
	
				return (ImTextureID)(uintptr_t)textureID;
			}
		}

		ImTextureID CreateImageBGRA8888(uint8_t const * pixels, int32_t width, int32_t height)
		{
			Locked(ImGuiApp::ContextMutex)
			{
				if (! ImGuiApp::ContextValid)
					return nullptr;
	
				if (!g_pool)
				{
					if (!OffscreenContext)
						return 0;
	
					g_pool = new Handy::ThreadPool(1);
	
					g_pool->AddJob([]
					{
						glfwMakeContextCurrent(OffscreenContext);
						glfwSwapInterval(0);
					});
				}
	
				g_pool->Wait();
	
				//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				GLuint textureID = 0;
	
				g_pool->AddJob([&textureID, width, height, pixels]
				{
					glEnable(GL_TEXTURE_2D);
					//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
					glGenTextures(1, &textureID);
					glBindTexture(GL_TEXTURE_2D, textureID);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
	
					if (!textureID)
					{
						fprintf(stderr, "Zero-value texture ID...");
					}
	
					glFinish();
					glFlush();
				});
	
				g_pool->Wait();
	
				if (!textureID)
				{
					fprintf(stderr, " zero value confirmed.\r\n");
				}
	
				return (ImTextureID)(uintptr_t)textureID;
			}
		}

		void DeleteImage(ImTextureID img)
		{
			Locked(ImGuiApp::ContextMutex)
			{
				if (! ImGuiApp::ContextValid)
					return;
	
				if (!OffscreenContext)
					return;
	
				g_pool->AddJob([img]
				{
					GLuint id = (GLuint)((uintptr_t)img);
	
					if (id)
						glDeleteTextures(1, &id);
				});
	
				//g_pool->Wait();
			}
		}
	}
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static GLFWwindow * window = nullptr;

void ImGuiApp::initGL()
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		throw std::runtime_error("Cannot Initialize GLFW");

	// Decide GL+GLSL versions
	#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
	#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
	#endif

	ImGuiTextures::GLFW::Init();

	if (m_showMainWindow)
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	else
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	// Create window with graphics context
	window = glfwCreateWindow(300, 300, "Root", NULL, ImGuiTextures::GLFW::OffscreenContext);

	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

	if (window == NULL)
		throw std::runtime_error("Cannot Initialize GLFW");

	glfwMakeContextCurrent(window);
	glfwSwapInterval(swapInterval());

	// Initialize OpenGL loader
	#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
	bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
	bool err = false;
	glbinding::Binding::initialize();
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
	bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
	#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
	#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		throw std::runtime_error("Cannot Initialize GLFW");
	}

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
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup Style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

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

void ImGuiApp::mainGL()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Main loop
	while (!glfwWindowShouldClose(window) && m_ui->DrawLoopEnabled)
	{
		Handy::StopWatch sw;

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		if (!isFirstFrame)
		{
			if (m_scaling != Fonts::Scaling || m_scalingFont != Fonts::FontScaling)
			{
				ImGui_ImplOpenGL3_DestroyFontsTexture();
				Fonts::Build(m_scaling, m_scalingFont);
				ImGui_ImplOpenGL3_CreateFontsTexture();
	
				ImGui::SetCurrentFont(Fonts::Normal);
			}
		}
		else
			isFirstFrame = false;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		m_ui->Preframe();

		ImGui::NewFrame();

		m_ui->Draw();

		/// ---- Rendering ---- /// 
		bool asvs = shouldVSync(); // Must be before ImGui::Render()

		ImGui::Render();

		int display_w, display_h;
		/*glfwMakeContextCurrent(window);*/
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(window);
		}

		/// GL doesn't drop frames on resize, so we don't need to force vsync on resize like we do with DX.
		//if (zInternal_WasResized()) // Must be after ImGui::UpdatePlatformWindows()
		//	asvs = true;

		//glfwMakeContextCurrent(window);
	
		if (swapInterval() == -1)
			glfwSwapInterval(asvs ? 1 : 0);
		else
			glfwSwapInterval(swapInterval());
		
		glfwSwapBuffers(window);

		zInternal_WasResized() = false;

		m_ui->Postframe();

		int32 rr = 1000_i32 / (int32)(monitorRefreshRate() * 4);
		int32 timeDiff = rr - (int32)(sw.Milliseconds());

		if (timeDiff > 0)// && asvs)
			std::this_thread::sleep_for(std::chrono::milliseconds(timeDiff));
	}
}

void ImGuiApp::deinitGL()
{
	Locked(ImGuiApp::ContextMutex)
		ImGuiApp::ContextValid = false;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwDestroyWindow(ImGuiTextures::GLFW::OffscreenContext);
	glfwTerminate();
}

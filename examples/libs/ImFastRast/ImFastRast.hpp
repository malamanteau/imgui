/// =============================================================================
/// LICENSE - Public Domain
///
/// Original:
///     emilk / imgui_software_renderer  (https://github.com/emilk/imgui_software_renderer)
///
/// Changes:
/// 	Optimizations
///
/// This software is dual-licensed to the public domain and under the following 
/// license: you are granted a perpetual, irrevocable license to copy, modify, 
/// publish, and distribute this file as you see fit.
///
/// =============================================================================

#pragma once

#include <cstdint>

#include <handycpp/Handy.hpp>

#include <imgui/imgui.h>

namespace ImFastRast
{

	struct Stats
	{
		int64_t UniformTrianglePixels           = 0;
		int64_t UniformRectanglePixels          = 0;

		int64_t TexturedTrianglePixels          = 0;
		int64_t textured_rectangle_pixels       = 0;

		int64_t GradientTrianglePixels          = 0;
		int64_t GradientRectanglePixels         = 0;
		int64_t GradientTexturedRectanglePixels = 0;

		int64_t FontPixels                      = 0;

		int64_t TriangleOverdraw                = 0;
		int64_t TriangleOverdrawSkipped         = 0;

		double PaintTime = 0;

		Stats() = default;
	};

	struct Texture
	{
		uint8_t const * Pixels; // 8-bit.

		int           Width;
		int           Height;

		bool          Is8Bit;
	};

	struct PaintTarget
	{
		uint32_t *    Pixels;
		int           Width;
		int           Height;
		//Math::Vector2 scale; // Multiply ImGui (point) coordinates with this to get pixel coordinates.
	};
	
	struct Window
	{
		void * windowHandle = nullptr;

		PaintTarget * BufferA;
		PaintTarget * BufferB;
	};

	//Stats Statistics;
	
	void DrawInterface();

	void Fill(uint32_t * pixels, int w, int h, uint32_t fillColor);

	// The buffer is assumed to follow how ImGui packs pixels, i.e. ABGR by default. Change with IMGUI_USE_BGRA_PACKED_COLOR.
	// If width/height differs from ImGui::GetIO().DisplaySize then the function scales the UI to fit the given pixel buffer.
	Stats Paint(ImDrawData * draw_data, uint32_t * pixels, int w, int h);

}


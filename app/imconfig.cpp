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

#include "../imgui.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
	#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "../imgui_internal.h"

bool isAcute(ImVec2 const * pointsABC)
{
	ImVec2 A = pointsABC[0] - pointsABC[1];
	ImVec2 B = pointsABC[2] - pointsABC[1];

	ImVec2 BmA = B - A;

	float a2b2 = A.x * A.x + A.y * A.y + B.x * B.x + B.y * B.y;
	float c2 = BmA.x * BmA.x + BmA.y * BmA.y;

	return c2 < a2b2;
}

void ImDrawList::AddPolylinePenupOnAcute(const ImVec2* points, const int points_count, ImU32 col, bool closed, float thickness)
{
	if (points_count <= 2)
	{
		AddPolyline(points, points_count, col, closed, thickness);
		return;
	}

	for (size_t i = 0; i < points_count - 2; ++i)
	{
		if (isAcute(points + i))
		{
			AddPolyline(points, i + 2, col, closed, thickness);
			AddPolylinePenupOnAcute(points + i + 1, points_count - i - 1, col, closed, thickness);
			return;
		}
	}

	AddPolyline(points, points_count, col, closed, thickness);
}

/// If there are ever changes/improvements to ImDrawList::PathArcToFast make the same changes here!
void ImDrawList::PathArcToFastCCW(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12)
{
	if (radius == 0.0f || a_min_of_12 > a_max_of_12)
	{
		_Path.push_back(center);
		return;
	}

	// For legacy reason the PathArcToFast() always takes angles where 2*PI is represented by 12,
	// but it is possible to set IM_DRAWLIST_ARCFAST_TESSELATION_MULTIPLIER to a higher value. This should compile to a no-op otherwise.
#if IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER != 1
	a_min_of_12 *= IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER;
	a_max_of_12 *= IM_DRAWLIST_ARCFAST_TESSELLATION_MULTIPLIER;
#endif

	_Path.reserve(_Path.Size + (a_max_of_12 - a_min_of_12 + 1));
	for (int a = a_max_of_12; a >= a_min_of_12; a--)
	{
		const ImVec2& c = _Data->ArcFastVtx[a % IM_ARRAYSIZE(_Data->ArcFastVtx)];
		_Path.push_back(ImVec2(center.x + c.x * radius, center.y + c.y * radius));
	}
}

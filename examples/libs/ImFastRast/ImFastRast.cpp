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

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <exception>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "ImFastRast.hpp"

struct TriangleInfo
{
	ImFastRast::PaintTarget    target;
	ImFastRast::Texture const * texture;

	Math::Vector4  clip_rect;

	Math::Vector2  v0Pos;
	Math::Vector2  v1Pos;
	Math::Vector2  v2Pos;

	Math::Vector2  v0UV;
	Math::Vector2  v1UV;
	Math::Vector2  v2UV;

	uint32         v0Col;
	uint32         v1Col;
	uint32         v2Col;

	TriangleInfo() = default;
};

//static void * m_threadPool()
//{
//	static void * inst = nullptr;
//	return inst;
//}
//
//ImFastRast::ImFastRast()
//	: m_threadPool(new Handy::ThreadPool(2))
//{
//
//}
//
//ImFastRast::~ImFastRast()
//{
//	Handy::ThreadPool * tp  = (Handy::ThreadPool *)m_threadPool;
//
//	Handy::SafeDelete(tp);
//
//	m_threadPool   = nullptr;
//}

// ----------------------------------------------------------------------------

FORCEINLINE uint32_t ToUint32(Math::Vector4i const & colorInt)
{
	#ifdef USE_AVX2
	Math::Vector4i temp = colorInt << Math::Vector4i(24, 16, 8, 0);
	return temp.X | temp.Y | temp.Z | temp.W;
	#else
	return (colorInt.X << 24u) | (colorInt.Y << 16u) | (colorInt.Z << 8u) | colorInt.W;
	#endif
}

///TODO Can do bitwise and as one instruction.
FORCEINLINE Math::Vector4i ToColorInt(uint32_t x)
{
	#ifdef USE_AVX2
	return 0xFFu & (Math::Vector4i(x) >> Math::Vector4i(IM_COL32_A_SHIFT, IM_COL32_B_SHIFT, IM_COL32_G_SHIFT, IM_COL32_R_SHIFT));
	#else
	return Math::Vector4i 
	(
		(int32_t)(x >> IM_COL32_A_SHIFT),
		(int32_t)(x >> IM_COL32_B_SHIFT),
		(int32_t)(x >> IM_COL32_G_SHIFT),
		(int32_t)(x >> IM_COL32_R_SHIFT)
	) & 0xFFu;
	#endif
}

FORCEINLINE Math::Vector4i blend(Math::Vector4i const & target, Math::Vector4i const & source)
{
	return ((source * source.XXXX() + target * (255 - source.X)) / 255);//.InsertZero<0>();

	//return Math::Vector4i(
	//	0, // Whatever.
	//	(source.Y * source.X + target.Y * (255 - source.X)) / 255,
	//	(source.Z * source.X + target.Z * (255 - source.X)) / 255,
	//	(source.W * source.X + target.W * (255 - source.X)) / 255);
}

FORCEINLINE Math::Vector4i blendI4(Math::Vector4i const & target, Math::Vector4i const & source)
{
	return Math::Vector4i
	(
		(int32_t)ToUint32(blend(ToColorInt((uint32_t)target.X), source)),
		(int32_t)ToUint32(blend(ToColorInt((uint32_t)target.Y), source)),
		(int32_t)ToUint32(blend(ToColorInt((uint32_t)target.Z), source)),
		(int32_t)ToUint32(blend(ToColorInt((uint32_t)target.W), source))
	);
}

constexpr float sdiv = 1.0f / 255.0f;
FORCEINLINE Math::Vector4 ColorConvertU32ToFloat4(ImU32 const & in)
{
	#ifdef USE_AVX2
	return sdiv * (0xFF & (Math::Vector4i(in) >> Math::Vector4i(IM_COL32_R_SHIFT, IM_COL32_G_SHIFT, IM_COL32_B_SHIFT, IM_COL32_A_SHIFT))).ToFloat();
	#else
	return Math::Vector4
	(
		(in >> IM_COL32_R_SHIFT) & 0xFF,
		(in >> IM_COL32_G_SHIFT) & 0xFF,
		(in >> IM_COL32_B_SHIFT) & 0xFF,
		(in >> IM_COL32_A_SHIFT) & 0xFF
	) * sdiv;
	#endif
}

FORCEINLINE ImU32 ColorConvertFloat4ToU32(Math::Vector4 const & in)
{
	Math::Vector4i temp = (in * 255.0f + 0.5f).ToInteger();

	#ifdef USE_AVX2
	temp =  temp << Math::Vector4i(IM_COL32_R_SHIFT, IM_COL32_G_SHIFT, IM_COL32_B_SHIFT, IM_COL32_A_SHIFT);
	return temp.X | temp.Y | temp.Z | temp.W;
	#else
	return
		(temp.X << IM_COL32_R_SHIFT) |
		(temp.Y << IM_COL32_G_SHIFT) |
		(temp.Z << IM_COL32_B_SHIFT) |
		(temp.W << IM_COL32_A_SHIFT);
	#endif
}

// ----------------------------------------------------------------------------
// For fast and subpixel-perfect triangle rendering we used fixed point arithmetic.

constexpr int32_t kFixedBias = 8;
constexpr int32_t kFixedBiasD2 = 4;

constexpr float kUnfixedDivisor = 1.0f / ((float)kFixedBias);

FORCEINLINE int32_t FixedOrient2D(Math::Vector2i const & a, Math::Vector2i const & b, Math::Vector2i const & c)
{
	return (b.X - a.X) * (c.Y - a.Y) - (b.Y - a.Y) * (c.X - a.X);
}

FORCEINLINE int32_t FixedOrient2D_FindX_0(Math::Vector2i const & a, Math::Vector2i const & b, int32_t y)
{
	int32_t num = (b.X - a.X) * (y - a.Y);
	int32_t denom = (b.Y - a.Y);
	if (denom == 0) return INT_MAX;

	return num / denom + a.X;
}


FORCEINLINE int32_t FixedOrient2D_FindX_BIAS(Math::Vector2i const & a, Math::Vector2i const & b, int32_t y, int32_t bias, int32_t sign)
{
	int32_t denom = b.Y - a.Y;

	if (denom == 0)
		return INT_MAX;

	return ((b.X - a.X) * (y - a.Y) + bias * sign) / denom + a.X;
}


//FORCEINLINE int32_t FixedOrient2D_FindX_N(Math::Vector2i const & a, Math::Vector2i const & b, float y, float n)
//{
//	return (b.X - a.X) * (y - a.Y) / (b.Y - a.Y) + a.X;
//}

FORCEINLINE int32_t ToFixed(float v)
{
	return static_cast<int32_t>(std::floor(v * kFixedBias));
}

FORCEINLINE float ToUnfixed(int32_t v)
{
	return static_cast<float>(v) * kUnfixedDivisor;
}

FORCEINLINE Math::Vector2i ToFixed(Math::Vector2 v)
{
	return Math::Vector2i{ToFixed(v.X), ToFixed(v.Y)};
}

FORCEINLINE Math::Vector2 ToUnfixed(Math::Vector2i v)
{
	return Math::Vector2{ToUnfixed(v.X), ToUnfixed(v.Y)};
}


// ----------------------------------------------------------------------------

FORCEINLINE float barycentric(Math::Vector2 const & a, Math::Vector2 const & b, Math::Vector2 const & point)
{
	Math::Vector4 f1342 = b.XY00().XYAB(point.XY00()) - a.XY00().XYXY(); /// This looks awful, but it inlines down to just 3 instructions.
	return f1342.X * f1342.W - f1342.Y * f1342.Z;
}

FORCEINLINE uint8_t sample_texture(ImFastRast::Texture const & texture, ImVec2 const & uv)
{
	Math::Vector2i dimensions(texture.Width, texture.Height);

	Math::Vector2i t_xy = (uv * (dimensions.ToFloat() - 1.0f) + 0.5f).ToInteger().Clamp(Math::Vector2i::Zero(), dimensions - 1);
	return texture.Pixels[t_xy.Y * dimensions.X + t_xy.X];
}

FORCEINLINE uint32_t sample_texture32(ImFastRast::Texture const & texture, ImVec2 const & uv)
{
	Math::Vector2i dimensions(texture.Width, texture.Height);

	Math::Vector2i t_xy = (uv * (dimensions.ToFloat() - 1.0f) + 0.5f).ToInteger().Clamp(Math::Vector2i::Zero(), dimensions - 1);
	return ((uint32_t *)texture.Pixels)[t_xy.Y * dimensions.X + t_xy.X];
}

//#define RENDER_SINGLE
FORCEINLINE void PaintUniformRectangle(
	ImFastRast::PaintTarget const & target,
	Math::Vector2  const & min_f,
	Math::Vector2  const & max_f,
	Math::Vector4i const & color,
	ImFastRast::Stats    & stats)
{
	// Integer bounding box [min, max):
	Math::AABB2i aabbI(((Math::Vector2)min_f + 0.5f).ToInteger(), ((Math::Vector2)max_f + 0.5f).ToInteger());

	// Clamp to render target:
	Math::AABB2i aabbRT(Math::Vector2i::Zero(), Math::Vector2i(target.Width, target.Height));
	aabbI = aabbI.Intersection(aabbRT);

	stats.UniformRectanglePixels += aabbI.Area();

	uint32_t last_target_pixel = target.Pixels[aabbI.Min.Y * target.Width + aabbI.Min.X];
	uint32_t last_output = ToUint32(blend(ToColorInt(last_target_pixel), color));

	Math::Vector4i last_target_pixel4 = Math::Vector4i(last_target_pixel);
	Math::Vector4i last_output4       = Math::Vector4i(last_output);
	
	#ifdef RENDER_SINGLE // Only paint a single pixel at a time, instead of four at a time.
	for (int y = aabbI.Min.Y; y < aabbI.Max.Y; ++y) 
		for (int x = aabbI.Min.X; x < aabbI.Max.X; ++x) 
		{
			uint32_t & target_pixel = target.Pixels[y * target.Width + x];
			if (target_pixel == last_target_pixel) 
			{
				target_pixel = last_output;
				continue;
			}
			last_target_pixel = target_pixel;
			target_pixel = ToUint32(blend(ToColorInt(target_pixel), color));
			last_output = target_pixel;
		} 
	#else // Paint four pixels at a time (except when we don't line up, then do one at a time.
	for (int y = aabbI.Min.Y; y < aabbI.Max.Y; ++y)
	for (int x = aabbI.Min.X; x < aabbI.Max.X; ++x) 
	{
		uint32_t & target_pixel = target.Pixels[y * target.Width + x];

		if ((uintptr_t)&target_pixel % 16 == 0 && x < (aabbI.Max.X - 3)) // If it is memory aligned and we won't overflow the rect, do it four pixels at a time.
		{
			Math::Vector4i & target_pixel4 = *(Math::Vector4i *)(void *)&target_pixel;

			if (target_pixel4 == last_target_pixel4)
			{
				target_pixel4 = last_output4;
				x += 3;
				continue;
			}

			last_target_pixel4 = target_pixel4;
			target_pixel4 = blendI4(target_pixel4, color);
			last_output4 = target_pixel4;
		}
		else
		{
			if (target_pixel == last_target_pixel) 
			{
				target_pixel = last_output;
				continue;
			}
			last_target_pixel = target_pixel;
			target_pixel = ToUint32(blend(ToColorInt(target_pixel), color));
			last_output = target_pixel;
		}
	} 

	#endif
}

FORCEINLINE void PaintUniformTexturedRectangle(
	ImFastRast::PaintTarget const & target,
	ImFastRast::Texture     const & texture,
	ImVec4                  const & clip_rect,
	ImDrawVert              const & min_v,
	ImDrawVert              const & max_v,
	ImFastRast::Stats             & stats)
{
	//// Find bounding box:
	Math::AABB2 aabb(min_v.pos, max_v.pos);

	// Clip against clip_rect:
	Math::AABB2 aabbClip(
		Math::Vector2(clip_rect.x, clip_rect.y),
		Math::Vector2(clip_rect.z - 0.5f, clip_rect.w - 0.5f));
	Math::AABB2 aabbPostClip = aabb.Intersection(aabbClip);

	// Integer bounding box [min, max):
	Math::AABB2i aabbI(aabbPostClip.Min.ToInteger(), (aabbPostClip.Max + 1.0f).ToInteger());


	// Clip against render target:
	Math::AABB2i aabbRT(Math::Vector2i::Zero(), Math::Vector2i(target.Width, target.Height)); // <-- Render Target bounds
	aabbI = aabbI.Intersection(aabbRT);

	stats.FontPixels += aabbI.Area();

	Math::Vector2 topleft = aabbI.Min.ToFloat() + 0.5f;

	Math::AABB2 aabbUV(min_v.uv, max_v.uv);
	Math::Vector2 delta_uv_per_pixel = aabbUV.Size() / aabb.Size();

	Math::Vector2 uv_topleft = Math::Vector2(min_v.uv) + (topleft - Math::Vector2(min_v.pos)) * delta_uv_per_pixel;

	Math::Vector2 current_uv = uv_topleft;

	auto vColorI  = ToColorInt(min_v.col);

	if (vColorI.X != 0)
	{
		if (texture.Is8Bit)
		{
			for (int y = aabbI.Min.Y; y < aabbI.Max.Y; ++y, current_uv.Y += delta_uv_per_pixel.Y) 
			{
				current_uv.X = uv_topleft.X;
				for (int x = aabbI.Min.X; x < aabbI.Max.X; ++x, current_uv.X += delta_uv_per_pixel.X) 
				{
					uint32_t & target_pixel = target.Pixels[y * target.Width + x];
					uint8_t texel = sample_texture(texture, current_uv);

					// The font texture is all black or all white, so optimize for this:
					if (texel == 0) 
					{ 
						continue; 
					}
					if (texel == 255) 
					{
						target_pixel = min_v.col;
						continue;
					}

					// Other textured rectangles
					target_pixel = ToUint32(blend(ToColorInt(target_pixel), vColorI.InsertX(vColorI.X * texel / 255)));
				}
			}
		}
		else
		{
			for (int y = aabbI.Min.Y; y < aabbI.Max.Y; ++y, current_uv.Y += delta_uv_per_pixel.Y) 
			{
				current_uv.X = uv_topleft.X;
				for (int x = aabbI.Min.X; x < aabbI.Max.X; ++x, current_uv.X += delta_uv_per_pixel.X) 
				{
					uint32_t & target_pixel = target.Pixels[y * target.Width + x];
					uint32_t texel = sample_texture32(texture, current_uv);
					// The font texture is all black or all white, so optimize for this:

					uint32_t alpha = (texel >> IM_COL32_A_SHIFT) & 0xFF;

					// If the texture is fully xparent or opaque, optimize for this.
					if (alpha == 0) 
					{ 
						continue; 
					}

					if (alpha == 255) 
					{
						target_pixel = texel;
						continue;
					}

					// Other textured rectangles
					target_pixel = ToUint32(blend(ToColorInt(target_pixel), ToColorInt(texel)));
				}
			}
		}
	}
}

// When two triangles share an edge, we want to draw the pixels on that edge exactly once.
// The edge will be the same, but the direction will be the opposite
// (assuming the two triangles have the same winding order).
// Which edge wins? This functions decides.
FORCEINLINE bool IsDominantEdge(Math::Vector2 edge)
{
	// return edge.x < 0 || (edge.x == 0 && edge.y > 0);
	return edge.Y > 0 || (edge.Y == 0 && edge.X < 0);
}


// Handles triangles in any winding order (CW/CCW)
FORCEINLINE void PaintTriangle(TriangleInfo const & tInfo, ImFastRast::Stats & stats)
{
	const Math::Vector2 p0 = tInfo.v0Pos;
	const Math::Vector2 p1 = tInfo.v1Pos;
	const Math::Vector2 p2 = tInfo.v2Pos;

	const auto rect_area = barycentric(p0, p1, p2); // Can be positive or negative depending on winding order
	if (rect_area == 0.0f) 
		return; 
	// From original code (NOT AGK) was commented before I started: //  if (rect_area < 0.0f) { return PaintTriangle(target, texture, clip_rect, v0, v2, v1, stats); }

	Math::Vector3 p012_X(p0.X, p1.X, p2.X);
	Math::Vector3 p012_Y(p0.Y, p1.Y, p2.Y);

	Math::AABB2 aabb( // Find bounding box
		Math::Vector2(p012_X.Min(), p012_Y.Min()), 
		Math::Vector2(p012_X.Max(), p012_Y.Max()));

	Math::AABB2 aabbClip(Math::Vector2(tInfo.clip_rect.X, tInfo.clip_rect.Y), Math::Vector2(tInfo.clip_rect.Z, tInfo.clip_rect.W) - 0.5f);

	aabb = aabb.Intersection(aabbClip); // Clip against clip_rect

	Math::AABB2i aabbI(aabb.Min.ToInteger(), (aabb.Max + 1.0f).ToInteger()); // Integer bounding box [min, max)
	Math::AABB2i aabbRT(Math::Vector2i::Zero(), Math::Vector2i(tInfo.target.Width, tInfo.target.Height)); // Clip against render target

	aabbI = aabbI.Intersection(aabbRT);

	// ------------------------------------------------------------------------
	// Set up interpolation of barycentric coordinates:

	const Math::Vector2 topleft = aabbI.Min.ToFloat() + 0.5f;

	const Math::Vector3 w012_topleft(
		barycentric(p1, p2, topleft), 
		barycentric(p2, p0, topleft),
		barycentric(p0, p1, topleft));

	const Math::Vector2 tldx = topleft + Math::Vector2::UnitX();

	Math::Vector3 w012_dx = Math::Vector3(
		barycentric(p1, p2, tldx),
		barycentric(p2, p0, tldx),
		barycentric(p0, p1, tldx)) - w012_topleft;

	const Math::Vector2 tldy = topleft + Math::Vector2::UnitY();

	const Math::Vector3 w012_dy = Math::Vector3(
		barycentric(p1, p2, tldy),
		barycentric(p2, p0, tldy),
		barycentric(p0, p1, tldy)) - w012_topleft;

	const float inv_area = 1.0f / rect_area;
	const Math::Vector3 bary_topleft = inv_area * w012_topleft;
	const Math::Vector3 bary_dx      = inv_area * w012_dx;
	const Math::Vector3 bary_dy      = inv_area * w012_dy;

	Math::Vector3 bary_current_row = bary_topleft;

	// ------------------------------------------------------------------------
	// For pixel-perfect inside/outside testing:

	const int32_t sign = rect_area > 0 ? 1 : -1; // winding order?

	const Math::Vector4i bias012i(
		IsDominantEdge(p2 - p1) ? 0 : -1,
		IsDominantEdge(p0 - p2) ? 0 : -1,
		IsDominantEdge(p1 - p0) ? 0 : -1, 0
	);

	const Math::Vector2i p0i = ToFixed(p0);
	const Math::Vector2i p1i = ToFixed(p1);
	const Math::Vector2i p2i = ToFixed(p2);

	const Math::Vector4i p120i_X(p1i.X, p2i.X, p0i.X, 0);
	const Math::Vector4i p201i_X(p2i.X, p0i.X, p1i.X, 0);

	const Math::Vector4i p120i_Y(p1i.Y, p2i.Y, p0i.Y, 0);
	const Math::Vector4i p201i_Y(p2i.Y, p0i.Y, p1i.Y, 0);

	const Math::Vector4i pXmX = p201i_X - p120i_X;
	const Math::Vector4i pYmY = p201i_Y - p120i_Y;

	const Math::Vector4i pRHS = pYmY * p120i_X - pXmX * p120i_Y;

	// ------------------------------------------------------------------------

	const bool has_uniform_color = (tInfo.v0Col == tInfo.v1Col && tInfo.v0Col == tInfo.v2Col);

	const Math::Vector4 c0 = ColorConvertU32ToFloat4(tInfo.v0Col);
	const Math::Vector4 c1 = ColorConvertU32ToFloat4(tInfo.v1Col);
	const Math::Vector4 c2 = ColorConvertU32ToFloat4(tInfo.v2Col);

	// We often blend the same colors over and over again, so optimize for this (saves 10% total cpu):
	uint32_t last_target_pixel = 0;
	uint32_t last_output = ToUint32(blend(ToColorInt(last_target_pixel), ToColorInt(tInfo.v0Col)));

	for (int32_t y = aabbI.Min.Y; y < aabbI.Max.Y; ++y) 
	{
		int32_t py = y * kFixedBias + kFixedBiasD2;

		Math::Vector3 bary = bary_current_row;

		int32_t xMin = aabbI.Min.X;
		int32_t xMax = aabbI.Max.X;

		const Math::Vector4i pLHS = pXmX * py + pRHS;
		bool has_been_inside_this_row = false;

		// Inside/outside test:
		for (int32_t x = xMin; x <= xMax; ++x)
		{
			int32_t px = x * kFixedBias + kFixedBiasD2;

			const Math::Vector4i k123_ = pLHS - pYmY * px;
			const Math::Vector4i w012_i = sign * k123_ + bias012i.XYZ0();

			//if ((w0i | w1i | w2i) < 0)
			if ((w012_i.X | w012_i.Y | w012_i.Z) < 0)
			{
				if (has_been_inside_this_row) 
				{
					xMax = x;
					break; // Gives a nice 10% speedup
				}
			}
			else
			{
				if (!has_been_inside_this_row)
					xMin = x;

				has_been_inside_this_row = true;
			}
		}

		bary += ((float)(xMin - aabbI.Min.X)) * bary_dx;

		if (has_been_inside_this_row)
			for (int32_t x = xMin; x < xMax; ++x) 
			{
				Math::Vector3 w012 = bary;

				bary += bary_dx;

				uint32_t & target_pixel = tInfo.target.Pixels[int32_t(y) * tInfo.target.Width + int32_t(x)];

				if (has_uniform_color && !tInfo.texture) 
				{
					stats.UniformTrianglePixels += 1;
					if (target_pixel == last_target_pixel) 
					{
						target_pixel = last_output;
						continue;
					}

					last_target_pixel = target_pixel;
					target_pixel = ToUint32(blend(ToColorInt(target_pixel), ToColorInt(tInfo.v0Col)));
					last_output = target_pixel;
					continue;
				}

				Math::Vector4 src_color;

				if (has_uniform_color) 
				{
					src_color = c0;
				} 
				else 
				{
					stats.GradientTrianglePixels += 1;
					src_color = w012.X * c0 + w012.Y * c1 + w012.Z * c2;
				}

				if (tInfo.texture) 
				{
					stats.TexturedTrianglePixels += 1;
					const Math::Vector2 uv = w012.X * (Math::Vector2)tInfo.v0UV + w012.Y * (Math::Vector2)tInfo.v1UV + w012.Z * (Math::Vector2)tInfo.v2UV;
					src_color.W *= sample_texture(*tInfo.texture, uv) * (1/255.0f);
				}

				if (src_color.W <= 0.0f) 
					continue; // Transparent.

				if (src_color.W >= 1.0f) 
				{
					// Opaque, no blending needed:
					target_pixel = ColorConvertFloat4ToU32(src_color);
					continue;
				}

				Math::Vector4 target_color = ColorConvertU32ToFloat4(target_pixel);
				const auto blended_color = src_color.W * src_color + (1.0f - src_color.W) * target_color;
				target_pixel = ColorConvertFloat4ToU32(blended_color);
			}

		bary_current_row += bary_dy;
	}
}

void PaintDrawCommand(
	ImFastRast::PaintTarget const & target,
	ImDrawVert  const * vertices,
	ImDrawIdx   const * idx_buffer,
	ImDrawCmd   const & pcmd,
	ImFastRast::Stats             & stats)
{
	const auto texture = reinterpret_cast<const ImFastRast::Texture*>(pcmd.TextureId);
	assert(texture);

	// ImGui uses the first pixel for "white".
	const Math::Vector2 white_uv = 0.5f / Math::Vector2i(texture->Width, texture->Height).ToFloat();// ImVec2(0.5f / texture->width, 0.5f / texture->height);

	for (int i = 0; i + 3 <= (int)pcmd.ElemCount; ) 
	{
		ImDrawVert const & v0 = vertices[idx_buffer[i + 0]];
		ImDrawVert const & v1 = vertices[idx_buffer[i + 1]];
		ImDrawVert const & v2 = vertices[idx_buffer[i + 2]];

		// Text is common, and is made of textured rectangles. So let's optimize for it.
		// This assumes the ImGui way to layout text does not change.
		if (i + 6 <= (int)pcmd.ElemCount && idx_buffer[i + 3] == idx_buffer[i + 0] && idx_buffer[i + 4] == idx_buffer[i + 2])
		{
			const ImDrawVert& v3 = vertices[idx_buffer[i + 5]];

			if (v0.pos.x == v3.pos.x &&
				v1.pos.x == v2.pos.x &&
				v0.pos.y == v1.pos.y &&
				v2.pos.y == v3.pos.y &&
				v0.uv.x == v3.uv.x &&
				v1.uv.x == v2.uv.x &&
				v0.uv.y == v1.uv.y &&
				v2.uv.y == v3.uv.y)
			{
				const bool has_uniform_color =
					v0.col == v1.col &&
					v0.col == v2.col &&
					v0.col == v3.col;

				const bool has_texture =
					v0.uv != white_uv ||
					v1.uv != white_uv ||
					v2.uv != white_uv ||
					v3.uv != white_uv;

				if (has_uniform_color && has_texture)
				{
					PaintUniformTexturedRectangle(target, *texture, pcmd.ClipRect, v0, v2, stats);
					i += 6;
					continue;
				}
			}
		}

		// A lot of the big stuff are uniformly colored rectangles,
		// so we can save a lot of CPU by detecting them:
		if (i + 6 <= (int)pcmd.ElemCount) 
		{
			const ImDrawVert & v3 = vertices[idx_buffer[i + 3]];
			const ImDrawVert & v4 = vertices[idx_buffer[i + 4]];
			const ImDrawVert & v5 = vertices[idx_buffer[i + 5]];

			Math::Vector2 min, max;
			min.X = Math::Vector3(v0.pos.x, v1.pos.x, v2.pos.x).Min();
			min.Y = Math::Vector3(v0.pos.y, v1.pos.y, v2.pos.y).Min();
			max.X = Math::Vector3(v0.pos.x, v1.pos.x, v2.pos.x).Max();
			max.Y = Math::Vector3(v0.pos.y, v1.pos.y, v2.pos.y).Max();

			// Not the prettiest way to do this, but it catches all cases of a rectangle split into two triangles.
			// TODO: Stop it from also assuming duplicate triangles is one rectangle.
			if ((v0.pos.x == min.X || v0.pos.x == max.X) &&
				(v0.pos.y == min.Y || v0.pos.y == max.Y) &&
				(v1.pos.x == min.X || v1.pos.x == max.X) &&
				(v1.pos.y == min.Y || v1.pos.y == max.Y) &&
				(v2.pos.x == min.X || v2.pos.x == max.X) &&
				(v2.pos.y == min.Y || v2.pos.y == max.Y) &&
				(v3.pos.x == min.X || v3.pos.x == max.X) &&
				(v3.pos.y == min.Y || v3.pos.y == max.Y) &&
				(v4.pos.x == min.X || v4.pos.x == max.X) &&
				(v4.pos.y == min.Y || v4.pos.y == max.Y) &&
				(v5.pos.x == min.X || v5.pos.x == max.X) &&
				(v5.pos.y == min.Y || v5.pos.y == max.Y))
			{
				const bool has_uniform_color =
					v0.col == v1.col &&
					v0.col == v2.col &&
					v0.col == v3.col &&
					v0.col == v4.col &&
					v0.col == v5.col;

				const bool has_texture =
					v0.uv != white_uv ||
					v1.uv != white_uv ||
					v2.uv != white_uv ||
					v3.uv != white_uv ||
					v4.uv != white_uv ||
					v5.uv != white_uv;

				min.X = Handy::FastMax(min.X, pcmd.ClipRect.x);
				min.Y = Handy::FastMax(min.Y, pcmd.ClipRect.y);
				max.X = Handy::FastMin(max.X, pcmd.ClipRect.z - 0.5f);
				max.Y = Handy::FastMin(max.Y, pcmd.ClipRect.w - 0.5f);

				if (max.X < min.X || max.Y < min.Y) { i += 6; continue; } // Completely clipped

				const int64 num_pixels = (int64)((max.X - min.X) * (max.Y - min.Y));

				if (has_uniform_color) 
				{
					if (has_texture) 
					{
						stats.textured_rectangle_pixels += num_pixels;
					} 
					else 
					{
						PaintUniformRectangle(target, min, max, ToColorInt(v0.col), stats);
						i += 6;
						continue;
					}
				} 
				else 
				{
					if (has_texture) 
					{
						// I have never encountered these.
						stats.GradientTexturedRectanglePixels += num_pixels;
					} 
					else 
					{
						// Color picker. TODO: Optimize
						stats.GradientRectanglePixels += num_pixels;
					}
				}
			}
		}

		const bool has_texture = (v0.uv != white_uv || v1.uv != white_uv || v2.uv != white_uv);

		TriangleInfo tInfo;
		tInfo.target = target;
		tInfo.texture = has_texture ? texture : nullptr;

		tInfo.clip_rect = pcmd.ClipRect;

		tInfo.v0Pos = v0.pos;
		tInfo.v1Pos = v1.pos;
		tInfo.v2Pos = v2.pos;

		tInfo.v0UV = v0.uv;
		tInfo.v1UV = v1.uv;
		tInfo.v2UV = v2.uv;

		tInfo.v0Col = v0.col;
		tInfo.v1Col = v1.col;
		tInfo.v2Col = v2.col;

		PaintTriangle(tInfo, stats);
		i += 3;
	}
}

void PaintDrawList(ImFastRast::PaintTarget const & target, ImDrawList const * cmd_list, ImFastRast::Stats & stats, bool unique)
{
	const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer[0];
	const ImDrawVert* vertices = cmd_list->VtxBuffer.Data;

	for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
	{
		const ImDrawCmd& pcmd = cmd_list->CmdBuffer[cmd_i];
		if (pcmd.UserCallback) 
		{
			pcmd.UserCallback(cmd_list, &pcmd);
		} 
		else 
		{
			if (unique)
				PaintDrawCommand(target, vertices, idx_buffer, pcmd, stats);
		}
		idx_buffer += pcmd.ElemCount;
	}
}

void ImFastRast::DrawInterface()
{
	static bool fast_style      = true;
	static bool fast_style_once = true;

	Stats Statistics;

	ImGui::SetNextWindowPos(ImVec2(32.0f, 32.0f), ImGuiCond_Once);
	if (ImGui::Begin("ImFastRast")) 
	{
		if (ImGui::Checkbox("Tweak ImGui style for speed", &fast_style) || fast_style_once) 
		{
			fast_style_once = false;

			auto & style = ImGui::GetStyle();

			static auto windowRounding = style.   WindowRounding;
			static auto  childRounding = style.    ChildRounding;
			static auto scrollRounding = style.ScrollbarRounding;
			static auto   grabRounding = style.     GrabRounding;
			static auto  frameRounding = style.    FrameRounding;
			static auto  popupRounding = style.    PopupRounding;

			static auto aaLines = style.AntiAliasedLines;
			static auto aaFill  = style.AntiAliasedFill;

			if (fast_style) 
			{
				aaLines = style.AntiAliasedLines;
				aaFill  = style.AntiAliasedFill;

				windowRounding = style.   WindowRounding;
				 childRounding = style.    ChildRounding;
				scrollRounding = style.ScrollbarRounding;
				  grabRounding = style.     GrabRounding;
				 frameRounding = style.    FrameRounding;
				 popupRounding = style.    PopupRounding;

				style.AntiAliasedLines = false;
				style.AntiAliasedFill  = false;

				style.   WindowRounding = 0;
				style.    ChildRounding = 0;
				style.ScrollbarRounding = 0;
				style.     GrabRounding = 0;
				style.    FrameRounding = 0;
				style.    PopupRounding = 0;
			}
			else 
			{
				style.AntiAliasedLines = aaLines;
				style.AntiAliasedFill  = aaFill;

				style.   WindowRounding = windowRounding;
				style.    ChildRounding =  childRounding;
				style.ScrollbarRounding = scrollRounding;
				style.     GrabRounding =   grabRounding;
				style.    FrameRounding =  frameRounding;
				style.    PopupRounding =  popupRounding;
			}
		}

		static bool showPerFrame = true;

		ImGui::Checkbox("Show per frame statistics", &showPerFrame);

		if (showPerFrame)
		{
			// NB: Future columns API should allow automatic horizontal borders.
			ImGui::Columns(3, NULL, true);

			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("pixels/frame");        ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("TRIANGLE");  ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("RECTANGLE"); ImGui::NextColumn(); }

			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("UNIFORM");  ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("ut:  %7d", Statistics.UniformTrianglePixels);  ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("ur:  %7d", Statistics.UniformRectanglePixels); ImGui::NextColumn(); }

			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("TEXTURED"); ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("tt:  %7d", Statistics.TexturedTrianglePixels);  ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("tr:  %7d", Statistics.textured_rectangle_pixels); ImGui::NextColumn(); }

			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("GRADIENT"); ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("gt:  %7d", Statistics.GradientTrianglePixels);  ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("gr:  %7d", Statistics.GradientRectanglePixels); ImGui::NextColumn(); }

			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("GRAD+TEX"); ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("N/A");      ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("gtr: %7d", Statistics.GradientTexturedRectanglePixels); ImGui::NextColumn(); }

			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("FONT"); ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("");     ImGui::NextColumn(); }
			{ if (ImGui::GetColumnIndex() == 0) ImGui::Separator(); ImGui::Text("fr:  %7d", Statistics.FontPixels); ImGui::NextColumn(); }


			ImGui::Columns(1);

			ImGui::Separator();

			ImGui::Text("Triangle Overdraw:          %7d",   Statistics.TriangleOverdraw);                 
			ImGui::Text("Triangle Overdraw, Skipped: %7d",   Statistics.TriangleOverdrawSkipped);   

			ImGui::Separator();

			ImGui::Text("Paint time: %.2f ms", 1000 * Statistics.PaintTime);
			ImGui::Text("FPS: %.2f ", ImGui::GetIO().Framerate);
		}
	}
	ImGui::End();
}

static Handy::ThreadPool m_threadPool;

// The buffer is assumed to follow how ImGui packs pixels, i.e. ABGR by default.
// Change with IMGUI_USE_BGRA_PACKED_COLOR.
// If width/height differs from ImGui::GetIO().DisplaySize then
// the function scales the UI to fit the given pixel buffer.

void ImFastRast::Fill(uint32_t * pixels, int w, int h, uint32_t fillColor)
{
	memset(pixels, fillColor, w * h);
}

ImFastRast::Stats ImFastRast::Paint(ImDrawData * draw_data, uint32_t * pixels, int w, int h)
{
	//Handy::ThreadPool * tp = (Handy::ThreadPool *)&m_threadPool;

	//tp->AddJob([pixels, fillColor] 
	//{
		Handy::StopWatch sw;

		ImFastRast::Stats Statistics;

		ImGuiIO & io = ImGui::GetIO();
	   
		ImFastRast::PaintTarget target { pixels, w, h };

		if (w <= 0 || h <= 0)
			return Statistics;

		/*draw_data->ScaleClipRects(io.DisplayFramebufferScale);*/

		for (int i = 0; i < draw_data->CmdListsCount; ++i) 
			PaintDrawList(target, draw_data->CmdLists[i], Statistics, true);

		Statistics.PaintTime = sw.SecondsD();

		return Statistics;
	//});
}

//void ImFastRast::PaintBlockFinish()
//{
//	Handy::ThreadPool * tp = (Handy::ThreadPool *)&m_threadPool;
//	tp->Wait();
//}



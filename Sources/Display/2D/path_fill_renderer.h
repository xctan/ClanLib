/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    Mark Page
*/

#pragma once

#include <vector>
#include "API/Display/2D/canvas.h"
#include "API/Display/2D/path.h"
#include "API/Display/2D/brush.h"
#include "API/Display/Render/blend_state_description.h"
#include "API/Display/Render/blend_state.h"
#include "API/Display/Render/shader_object.h"
#include "API/Display/Render/texture_2d.h"
#include "API/Display/Render/transfer_texture.h"
#include "API/Display/Image/pixel_buffer.h"
#include "API/Display/Render/program_object.h"
#include "render_batch_buffer.h"
#include "path_renderer.h"

namespace clan
{
	enum class PathFillMode;
	class Brush;

	class PathScanlineEdge
	{
	public:
		PathScanlineEdge() { }
		PathScanlineEdge(float x, bool up_direction) : x(x), up_direction(up_direction) { }

		float x = 0.0f;
		bool up_direction = false;
	};

	class PathScanline
	{
	public:
		std::vector<PathScanlineEdge> edges;
		std::vector<unsigned char> pixels;
	};

	class PathFillRenderer : public PathRenderer
	{
	public:
		PathFillRenderer(GraphicContext &gc);

		void set_size(Canvas &canvas, int width, int height);
		void clear();

		void line(float x, float y) override;
		void end(bool close) override;

		void fill(RenderBatchBuffer *batch_buffer, Canvas &canvas, PathFillMode mode, const Brush &brush, const Mat4f &transform);

	private:
		struct Vertex
		{
			Vertex() { }
			Vertex(const Vec4f &position, const Vec4f &brush_data1, const Vec4f &brush_data2, const Vec2f &texcoord, int mode) :
				Position(position), BrushData1(brush_data1), BrushData2(brush_data2), TexCoord0(texcoord), Mode(mode) { }

			Vec4f Position;
			Vec4f BrushData1;
			Vec4f BrushData2;
			Vec2f TexCoord0;
			int Mode;
		};

		Pointf transform_point(Pointf point, const Mat3f &brush_transform, const Mat4f &fill_transform) const;

		int width = 0;
		int height = 0;
		std::vector<PathScanline> scanlines;
		TransferTexture mask_buffer;
		Texture2D mask_texture;
		PrimitivesArray prim_array[RenderBatchBuffer::num_vertex_buffers];
		BlendState blend_state;
	};

	class PathRasterRange
	{
	public:
		PathRasterRange(const PathScanline &scanline, PathFillMode mode);

		void next();

		bool found = false;
		float x0 = 0.0f;
		float x1 = 0.0f;

	private:
		const PathScanline &scanline;
		PathFillMode mode;
		size_t i = 0;
		int nonzero_rule = 0;
	};
}

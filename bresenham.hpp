#pragma once


#include <boost/gil/gil_all.hpp>


template<typename Pixel, typename View>
void clip_pixel(std::int64_t const& x, std::int64_t const& y, Pixel const& pixel, View& view)
{
	if(x < 0 || y < 0)
	{
		return;
	}
	else if(x < view.width() && y < view.height())
	{
		view(x, y) = pixel;
	}
}


template<typename Pixel, typename View>
void bresenham_line(std::int64_t const& x1, std::int64_t const& y1, std::int64_t const& x2, std::int64_t const& y2, Pixel const& pixel, View& view)
{
	// Adapted from http://www.etechplanet.com/codesnippets/computer-graphics-draw-a-line-using-bresenham-algorithm.aspx

	std::int64_t const dx(x2 - x1);
	std::int64_t const dy(y2 - y1);
	std::int64_t const adx(std::abs(dx));
	std::int64_t const ady(std::abs(dy));
	std::int64_t px(2 * ady - adx);
	std::int64_t py(2 * adx - ady);
	std::int64_t x, y, xe, ye;
	if(adx < ady)
	{
		if(dy < 0)
		{
			x = x2;
			y = y2;
			ye = y1;
		}
		else
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		clip_pixel(x, y, pixel, view);

		for(std::int64_t i(0); y < ye; ++ i)
		{
			y = y + 1;
			if(py > 0)
			{
				if((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					x = x + 1;
				}
				else
				{
					x = x - 1;
				}
				py = py + 2 * (adx - ady);
			}
			else
			{
				py = py + 2 * adx;
			}
			clip_pixel(x, y, pixel, view);
		}
	}
	else
	{
		if(dx < 0)
		{
			x = x2;
			y = y2;
			xe = x1;
		}
		else
		{
			x = x1;
			y = y1;
			xe = x2;
		}
		clip_pixel(x, y, pixel, view);

		for(std::int64_t i(0); x < xe; ++ i)
		{
			x = x + 1;
			if(px < 0)
			{
				px = px + 2 * ady;
			}
			else
			{
				if((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					y = y + 1;
				}
				else
				{
					y = y - 1;
				}
				px = px + 2 * (ady - adx);
			}
			clip_pixel(x, y, pixel, view);
		}
	}
}

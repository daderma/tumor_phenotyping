#ifndef TUMOR_PHENOTYPING__BRESENHAM_HPP
#define TUMOR_PHENOTYPING__BRESENHAM_HPP


#include <boost/gil/gil_all.hpp>


template<typename View>
void putpixel(std::int64_t const& x, std::int64_t const& y, View& view)
{
	if(x < 0 || y < 0)
	{
		return;
	}
	else if(x < view.width() && y < view.height())
	{
		view(x, y)[0] = 255;
		view(x, y)[1] = 255;
		view(x, y)[2] = 255;
	}
}


template<typename View>
void bresenham_line(std::int64_t const& x1, std::int64_t const& y1, std::int64_t const& x2, std::int64_t const& y2, View& view)
{
	std::int64_t x, y, xe, ye;
	std::int64_t dx(x2 - x1);
	std::int64_t dy(y2 - y1);
	std::int64_t dx1(std::abs(dx));
	std::int64_t dy1(std::abs(dy));
	std::int64_t px(2 * dy1 - dx1);
	std::int64_t py(2 * dx1-dy1);
	if(dy1 <= dx1)
	{
		if(dx >= 0)
		{
			x = x1;
			y = y1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}
		putpixel(x, y, view);

		for(std::int64_t i(0); x < xe; ++ i)
		{
			x = x + 1;
			if(px < 0)
			{
				px = px + 2 * dy1;
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
				px = px + 2 * (dy1 - dx1);
			}
			putpixel(x, y, view);
		}
	}
	else
	{
		if(dy >= 0)
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		else
		{
			x = x2;
			y = y2;
			ye = y1;
		}
		putpixel(x, y, view);

		for(std::int64_t i(0); y < ye; ++ i)
		{
			y = y + 1;
			if(py <= 0)
			{
				py = py + 2 * dx1;
			}
			else
			{
				if((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
				{
					x = x + 1;
				}
				else
				{
					x = x - 1;
				}
				py = py + 2 * (dx1 - dy1);
			}
			putpixel(x,y,view);
		}
	}
}


#endif // TUMOR_PHENOTYPING__BRESENHAM_HPP